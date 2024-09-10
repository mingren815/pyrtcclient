#pragma once
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mutex>

#include "api/maudio.h"
#include "api/devicemanager.h"
#include "rtcbasefunc.h"

using namespace std;
using namespace rtc;

// audio proxy
const int g_processIntervalMS = 100; // ms
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
        processIntervalMS = g_processIntervalMS;
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
        : m_sampleRate(sampleRate), m_channels(channels), m_count(0), m_audiobuf(NULL)
    {

        m_inputsize = 0;
        m_audiobuf = (char *)malloc(sizeof(char) * 19200);
    }
    ~AudioInPipeOnly()
    {
        if (m_audiobuf)
        {
            free(m_audiobuf);
            m_audiobuf = NULL;
        }
    }
    void inputAudioSample(char *data, unsigned int len)
    {
        std::lock_guard<std::mutex> gurad(m_mtx);
        memcpy(m_audiobuf, data, len);
    }
    bool onInit(int &sampleRate, int &channels, int &processIntervalMS) override
    {
        sampleRate = m_sampleRate;
        channels = m_channels;
        processIntervalMS = g_processIntervalMS;
        return true;
    }
    int onReadData(int sampleRate,
                   int channels,
                   char *data,
                   unsigned int len) override
    {
        ++m_count;
        std::lock_guard<std::mutex> gurad(m_mtx);
        memcpy(data, m_audiobuf, len);
        return 0;
    }
    int m_inputsize;
    int m_sampleRate;
    int m_channels;
    int m_count;
    std::mutex m_mtx;
    char *m_audiobuf;
};

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
        processIntervalMS = g_processIntervalMS;
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
