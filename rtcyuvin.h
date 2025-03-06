#pragma once
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include <sys/time.h>
#include <fcntl.h>

#include "api/mvideo.h"
#include "api/videocapture.h"
#include "rtcbasefunc.h"

using namespace std;
using namespace rtc;

const int read_buff_len = 1024 * 1000;

typedef rtc::FakeVideoCapturer *FakeVideoCapType;
class YUV420CapturePipe : public rtc::FakeVideoCapturer::Listener
{
public:
    YUV420CapturePipe(FourCC format)
        : m_capture(NULL), m_isStart(false), m_format(format)
    {
        m_capture = FakeVideoCapturer::Create(this, m_format);
        cout << "EncodedCaptureFromFile";
    }
    ~YUV420CapturePipe()
    {
        m_isStart = false;
        if (m_capture)
        {
            FakeVideoCapturer::Destroy(m_capture);
            m_capture = NULL;
        }
    }
    int inputCapturedFrame(int w, int h, const uint8 *data, size_t len)
    {
        // 后期自己定义错误类型
        if (!m_capture || !m_isStart)
        {
            return -1;
        }
        return m_capture->inputCapturedFrame(0, m_format, w, h, data, len, 0, false);
    }
    FakeVideoCapType GetCapture()
    {
        return m_capture;
    }

private:
    bool OnStart() override
    {
        m_isStart = true;
        return m_isStart;
    }
    void OnStop() override
    {
        m_isStart = false;
    }
    FakeVideoCapType m_capture;
    bool m_isStart;
    FourCC m_format;
};
class EncodedCaptureFromFile : public rtc::FakeVideoCapturer::Listener
{
public:
    EncodedCaptureFromFile(FourCC format, std::string filePath)
        : m_capture(NULL), m_isStart(false), m_format(format)
    {
        m_capture = FakeVideoCapturer::Create(this, m_format);
        cout << "EncodedCaptureFromFile";
        // m_networkThread = new rtc::Thread();
        // m_networkThread->SetName("H264CaptureFromFile Thread", NULL);
        // m_networkThread->Start(this);

        pthread_create(&m_thdId, NULL, EncodedCaptureFromFile::Run, (void *)this);

        m_threadActive = true;
        FilePaths = filePath;
        m_fd = fopen(FilePaths.c_str(), "rb");
        m_currread = 0;
        memset(m_buffer, 0, sizeof(m_buffer));
        m_currinput = read_buff_len - 4;
        //printf(",open fd = %d\n", m_fd);
        fseek(m_fd, 0, SEEK_END);
        m_filesize = ftell(m_fd);
        fseek(m_fd, 0, SEEK_SET);
    }
    ~EncodedCaptureFromFile()
    {
        cout << "~EncodedCaptureFromFile";
        m_isStart = false;
        m_threadActive = false;
        // m_networkThread->Stop();
        // delete m_networkThread;
        // m_networkThread = NULL;
        FakeVideoCapturer::Destroy(m_capture);
        if (m_fd)
        {
            fclose(m_fd);
            m_fd = NULL;
        }
    }
    void setStartImported(bool enalbe)
    {
        m_isStart = enalbe;
    }
    virtual bool OnStart()
    {
        cout << "EncodedCaptureFromFile::OnStart" << endl;
        m_isStart = true;
        return true;
    }
    virtual void OnStop()
    {
        cout << "EncodedCaptureFromFile::OnStop" << endl;
        m_isStart = false;
        // m_capture = NULL;
    }
    static void *Run(void *pArg)
    {
        EncodedCaptureFromFile *pthis = (EncodedCaptureFromFile *)pArg;
        while (pthis->m_threadActive)
        {
            if (!pthis->m_isStart)
            {
                sleep(1);
                continue;
            }
            //         cout<<"============== start run capture video."<<endl;
            int64_t before = timestamp();
            // printf("ptread Failure!!!\n");

            // scan buffer for a frame
            uint8 *currbegin = NULL, *currend = NULL;
            int currlen = 0;
            bool findOneHeader = false;
            uint8 *curr = &pthis->m_buffer[pthis->m_currinput];
            while (curr < &pthis->m_buffer[read_buff_len - 4])
            {
                if (0 == *curr && 0 == *(curr + 1) && 0 == *(curr + 2) && 1 == *(curr + 3))
                {
                    if (!findOneHeader)
                    {
                        findOneHeader = true;
                        currbegin = curr;
                    }
                    else if (curr - currbegin > 100)
                    {
                        currend = curr;
                        currlen = currend - currbegin;
                        pthis->m_currinput = currend - &pthis->m_buffer[0];
                        break;
                    }
                }
                ++curr;
            }

            if (currlen > 0)
            {
                // IncomingH264Data(currbegin, currlen);
                pthis->m_capture->inputEncodedFrame(0, 160, 128, currbegin, currlen);
                printf("Video read input len=%d\n", currlen);
            }
            else
            {
                pthis->m_currread -= (read_buff_len - pthis->m_currinput);
                if (pthis->m_currread < 0)
                {
                    pthis->m_currread = 0;
                }
                fseek(pthis->m_fd, pthis->m_currread, SEEK_SET);
                int size = fread(pthis->m_buffer, 1, read_buff_len, pthis->m_fd);
                if (size != read_buff_len)
                {
                    pthis->m_currread = 0;
                    pthis->m_currinput = 0;
                    curr = &pthis->m_buffer[0];
                    memset(pthis->m_buffer, 0, sizeof(pthis->m_buffer));
                    fseek(pthis->m_fd, 0, SEEK_SET);
                    continue;
                }
                pthis->m_currread += size;
                pthis->m_currinput = 0;
            }
            int64_t diff = 30 - (before - timestamp());
            // RTC_LOG_T_F(LS_INFO) << "diff: " << diff << ",currlen=" << currlen);
            if (diff > 10)
            {
                usleep((diff - 1) * 1000);
                // thread->SleepMs((int)(diff - 1));
            }
        }
        cout << "============== start run  end capture...";
        pthread_exit(NULL);
    }
    // void static IncomingH264Data(uint8* data, uint32 len) {
    //     if (!pthis->m_isStart) {
    //         return;
    //     }
    //     pthis->m_capture->inputEncodedFrame(0, 160, 128, data, len);
    //
    // }
    FakeVideoCapType GetCapture()
    {
        return m_capture;
    }

private:
    FakeVideoCapType m_capture;
    bool m_isStart;
    // rtc::Thread*        m_networkThread;
    pthread_t m_thdId;
    bool m_threadActive;
    FILE *m_fd;
    int m_currread;
    uint8 m_buffer[read_buff_len];
    int m_currinput;
    uint64 m_filesize;
    FourCC m_format;
    std::string FilePaths;
};