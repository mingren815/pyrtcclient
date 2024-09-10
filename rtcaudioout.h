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
static const int g_outProcessIntervalMS = 100; // ms

class AudioOutProxyToFile : public rtc::AudioOutInterface
{
public:
    AudioOutProxyToFile(int sampleRate, int channels)
        : m_sampleRate(sampleRate), m_channels(channels), m_count(0), m_fd(0)
    {
        m_fd = fopen("AudioOutProxy.PCM", "wb");
    }
    ~AudioOutProxyToFile()
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
        processIntervalMS = g_outProcessIntervalMS;
        return true;
    }
    void onWriteData(int sampleRate,
                     int channels,
                     const int8 *data,
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
            // printf("WriteData: time=%d, count= %d, sampleRate=%d, channels=%d, len=%d \r\n", time, m_count, sampleRate, channels, len);
#endif // WIN32
        }
        if (m_fd)
        {
            fwrite(data, len, 1, m_fd);
        }
    }

    int m_sampleRate;
    int m_channels;
    int m_count;
    FILE *m_fd;
};

class AudioOutPipeOnly : public rtc::IAudioDataListener
{
public:
    AudioOutPipeOnly(int bufferSize) : m_sampleRate(0), m_channels(0), m_count(0), m_audioBuffer(0)
    {
        m_audioBuffer = new CircularBuffer(bufferSize);
    }
    ~AudioOutPipeOnly()
    {
        {
            std::lock_guard<std::mutex> gurad(m_mtx);
            delete m_audioBuffer;
            m_audioBuffer = NULL;
        }
    }
    void onAudioData(const UserId &userId, uint64 timestamp, uint32 sampleRate, uint32 channels, const void *data, uint32 len) override
    {
        m_userId = userId;
        m_sampleRate = sampleRate;
        m_channels = channels;
        m_count++;
        std::lock_guard<std::mutex> gurad(m_mtx);
        int writeSize = m_audioBuffer->put_data((uint8_t *)data, len);
        if (writeSize != len)
        {
            // buffer  is full,
        }
    }
    int getAudioData(const UserId &userId, char *data, int dataSize)
    {
        // user is different from sueber
        if (userId != m_userId)
        {
            return 0;
        }
        std::lock_guard<std::mutex> gurad(m_mtx);
        if (!m_audioBuffer)
        {
            return 0;
        }
        return m_audioBuffer->get_data((uint8_t *)data, dataSize);
    }
    int sampleRate() { return m_sampleRate; }
    int channels() { return m_channels; }

private:
    int m_sampleRate;
    int m_channels;
    int m_count;
    UserId m_userId;
    std::mutex m_mtx;
    CircularBuffer *m_audioBuffer;
};