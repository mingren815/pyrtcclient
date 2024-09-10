#pragma once
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "api/maudio.h"
#include "api/devicemanager.h"
#include "rtcbasefunc.h"

using namespace std;
using namespace rtc;

//audio proxy
const int g_processIntervalMS = 100;//ms
class AudioInProxy : public rtc::AudioInInterface {
public:
    AudioInProxy(int sampleRate, int channels)
        : m_sampleRate(sampleRate)
        , m_channels(channels)
        , m_count(0)
        , m_fd(0)
        , m_audiobuf(NULL) {

        m_fd = fopen("AudioInProxy.PCM", "rb");
        inputsize = 0;
        m_audiobuf = (char*)malloc(sizeof(char) * 19200);
    }
    ~AudioInProxy() {
        if (m_fd) {
            fclose(m_fd);
            m_fd = NULL;
        }
    }
    bool onInit(int& sampleRate, int& channels, int& processIntervalMS) override {
        sampleRate = m_sampleRate;
        channels = m_channels;
        processIntervalMS = g_processIntervalMS;
        return true;
    }
    int onReadData(int sampleRate,
                   int channels,
                   char* data,
                   unsigned int len) override {
        ++m_count;
        if (m_count % 10 == 0) {
            long time = timestamp();
#ifdef WIN32
            RTC_LOG_T_F(LS_INFO) 
                << ", time=" << time
                << ", count= " << m_count
                << ", sampleRate=" << sampleRate 
                << ", channels=" << channels 
                << ", len=" << len;
#else
            //printf("ReadData: time=%d, count= %d, sampleRate=%d, channels=%d, len=%d \r\n", time, m_count, sampleRate, channels, len);
#endif // WIN32

        }
        if(m_fd) {
            
            size_t count = fread(data, len, 1, m_fd);
            if (count <= 0) {
                fseek(m_fd, 0, SEEK_SET);
            }
            return count * len;
        }
    }
    int inputsize;
    int m_sampleRate;
    int m_channels;
    int m_count;
    FILE* m_fd;
    char *m_audiobuf;
};
class AudioOutProxy : public rtc::AudioOutInterface {
public:
    AudioOutProxy(int sampleRate, int channels)
        : m_sampleRate(sampleRate)
        , m_channels(channels)
        , m_count(0)
        , m_fd(0) {
        m_fd = fopen("AudioOutProxy.PCM", "wb");
    }
    ~AudioOutProxy() {
        if (m_fd) {
            fclose(m_fd);
            m_fd = NULL;
        }
    }
    bool onInit(int& sampleRate, int& channels, int& processIntervalMS)override {
        sampleRate = m_sampleRate;
        channels = m_channels;
        processIntervalMS = g_processIntervalMS;
        return true;
    }
    void onWriteData(int sampleRate,
                     int channels,
                     const int8* data,
                     unsigned int len) override {
        ++m_count;
        if (m_count % 10 == 0) {
            long time = timestamp();
#ifdef WIN32
            RTC_LOG_T_F(LS_INFO)
                << ", time=" << time
                << ", count= " << m_count
                << ", sampleRate=" << sampleRate
                << ", channels=" << channels
                << ", len=" << len;
#else
            //printf("WriteData: time=%d, count= %d, sampleRate=%d, channels=%d, len=%d \r\n", time, m_count, sampleRate, channels, len);
#endif // WIN32

        }
        if (m_fd) {
            fwrite(data, len, 1, m_fd);
        }
    }

    int m_sampleRate;
    int m_channels;
    int m_count;
    FILE* m_fd;
};



