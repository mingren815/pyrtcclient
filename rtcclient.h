#pragma once
#include "rtcclientbase.h"

static rtc::String _url = "https://v.nice2meet.cn";
static rtc::String _appkey = "demo_access";
static rtc::String _secretkey = "demo_secret";

static std::string g_url = _url;
static std::string g_appkey = _appkey;
static std::string g_secretkey = _secretkey;
static std::string g_audioOprate = "2";
static std::string g_videoOprate = "2";
static std::string g_roomid;
static std::string g_decodeable = "2";
static std::string g_inputfile = "VideoInput.h264";

static rtc::VideoCodec g_videocodec = rtc::codec_h264;
static rtc::CameraCapability g_cap(1280, 720, 20);

class RtcClient : public RtcClientBase
{
public:
    RtcClient();
    ~RtcClient();
    int InitEngine();
    void UninitEngine();
    void CloseRoom();
    int CreatRoom();
    int JoinRoom(rtc::String roomID);
private:
    virtual void onInitResult(Result result);
    virtual void onScheduleRoomResult(uint32 callId, Result result, const rtc::RoomId &roomId);
    virtual void onJoinResult(Result result);
    virtual void onConnectionStatus(rtc::ConnectionStatus status);
    virtual void onPublishCameraNotify(const Camera &camera);
    virtual void onUnpublishCameraNotify(const Camera &camera);
    virtual void onSubscribeResult(Result result, const DeviceId &fromId);
    virtual void onUnsubscribeResult(Result result, const DeviceId &fromId);
    virtual void onPublishLocalResult(Result result, const DeviceId &fromId);
    virtual void onUnpublishLocalResult(Result result, const DeviceId &fromId);

public:
    rtc::IRoom *m_roomobj;
    rtc::IMAudio *m_audio;
    rtc::IMVideo *m_video;
    EncodedCaptureFromFile *m_pipe;

private:
    std::string m_roomid;
    std::string m_userName;
    uint32 m_callid;
    rtc::User m_user;
    rtc::Camera m_fakeCamera;
    std::map<std::string, FakeEncodedFrameListener *> m_fakelistener;
    std::map<DeviceId, MyVideoRender *> m_rendermap;
};
