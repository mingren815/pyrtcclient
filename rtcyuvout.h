#pragma once
#include <string.h>
#include "api/videorender.h"
#include "api/mvideo.h"

using namespace std;
using namespace rtc;

class MyVideoRender : public IVideoRender
{
public:
    MyVideoRender(const DeviceId &id) : m_id(id), m_fp(0)
    {
        std::string filename = "data_" + m_id + ".yuv";
        if (!m_fp)
        {
            m_fp = fopen(filename.c_str(), "wb");
        }
    }
    ~MyVideoRender()
    {
        if (m_fp)
        {
            fclose(m_fp);
            m_fp = 0;
        }
    }
    virtual void SetSize(int width, int height) { ; }
    virtual void RenderFrame(const IVideoFrame &frame)
    {
        if (m_fp)
        {
            int nLen = frame.strideY() * frame.height();
            fwrite(frame.dataY(), 1, nLen, m_fp);
            nLen = frame.strideU() * frame.height() / 2;
            fwrite(frame.dataU(), 1, nLen, m_fp);
            fwrite(frame.dataV(), 1, nLen, m_fp);
        }
    }

private:
    DeviceId m_id;
    FILE *m_fp;
};

////out remote video
class FakeEncodedFrameListener : public rtc::IEncodedFrameListener
{
public:
    FakeEncodedFrameListener(std::string file)
        : count(0), m_fd(0)
    {
        m_fd = fopen(file.c_str(), "wb");
    }
    void onEncodedFrame(VideoCodec codec, uint64 timestamp, bool isKeyFrame, int width, int height, const uint8 *data, unsigned int len)
    {
        if (!isKeyFrame && count == 0)
        {
            printf("!isKeyFrame && count==0");
            return;
        }
        ++count;
        size += len;
        if (count % 10 == 0)
        {
        }
        printf("IEncodedFrameListener: %d Frames, %d Byte\r\n", count, size);
        if (m_fd)
        {
            fwrite(data, len, 1, m_fd);
        }
    }
    ~FakeEncodedFrameListener()
    {

        if (m_fd)
        {
            fclose(m_fd);
            m_fd = NULL;
        }
    }
    int count;
    int size;
    FILE *m_fd;
};
