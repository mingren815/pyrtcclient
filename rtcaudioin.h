#pragma once
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mutex>

#include "api/maudio.h"
#include "api/devicemanager.h"
#include "rtcbasefunc.h"
#include "rtccircularbuffer.h"

using namespace std;
using namespace rtc;

// audio proxy
static const int g_inProcessIntervalMS = 100; // ms
class AudioInProxyFromFile : public rtc::AudioInInterface
{
public:
    AudioInProxyFromFile(int sampleRate, int channels)
        : m_sampleRate(sampleRate), m_channels(channels), m_count(0), m_fd(0), m_audiobuf(NULL)
    {

        m_fd = fopen("AudioInProxy.PCM", "rb");
        inputsize = 0;
        m_audiobuf = (char *)malloc(sizeof(char) * 19200);
    }
    ~AudioInProxyFromFile()
    {
        if (m_fd)
        {
            fclose(m_fd);
            m_fd = NULL;
        }
    }
    bool onInit(int &sampleRate, int &channels, int &processIntervalMS) override
    {
        sampleRate = m_sampleRate;
        channels = m_channels;
        processIntervalMS = g_inProcessIntervalMS;
        return true;
    }
    int onReadData(int sampleRate,
                   int channels,
                   char *data,
                   unsigned int len) override
    {
        ++m_count;
        if (m_count % 10 == 0)
        {
            long time = timestamp();
#ifdef WIN32
            RTC_LOG_T_F(LS_INFO)
                << ", time=" << time
                << ", count= " << m_count
                << ", sampleRate=" << sampleRate
                << ", channels=" << channels
                << ", len=" << len;
#else
            // printf("ReadData: time=%d, count= %d, sampleRate=%d, channels=%d, len=%d \r\n", time, m_count, sampleRate, channels, len);
#endif // WIN32
        }
        if (m_fd)
        {

            size_t count = fread(data, len, 1, m_fd);
            if (count <= 0)
            {
                fseek(m_fd, 0, SEEK_SET);
            }
            return count * len;
        }
        return 0;
    }
    int inputsize;
    int m_sampleRate;
    int m_channels;
    int m_count;
    FILE *m_fd;
    char *m_audiobuf;
};

class AudioInPipeOnly : public rtc::AudioInInterface
{
public:
    AudioInPipeOnly(int sampleRate, int channels)
        : m_sampleRate(sampleRate), m_channels(channels), m_count(0), m_audioBuffer(NULL)
    {

        std::lock_guard<std::mutex> gurad(m_mtx);
        m_audioBuffer = new CircularBuffer(sampleRate * channels * 2);
    }
    ~AudioInPipeOnly()
    {
        {
            std::lock_guard<std::mutex> gurad(m_mtx);
            delete m_audioBuffer;
            m_audioBuffer = NULL;
        }
    }
    void inputAudioSample(char *data, unsigned int len)
    {
        std::lock_guard<std::mutex> gurad(m_mtx);
        m_audioBuffer->put_data((const uint8_t *)data, len);
    }
    bool onInit(int &sampleRate, int &channels, int &processIntervalMS) override
    {
        sampleRate = m_sampleRate;
        channels = m_channels;
        processIntervalMS = g_inProcessIntervalMS;
        return true;
    }
    int onReadData(int sampleRate,
                   int channels,
                   char *data,
                   unsigned int len) override
    {
        ++m_count;
        std::lock_guard<std::mutex> gurad(m_mtx);
        return m_audioBuffer->get_data((uint8_t *)data, len);
    }
    int m_sampleRate;
    int m_channels;
    int m_count;
    std::mutex m_mtx;
    CircularBuffer *m_audioBuffer;
};
