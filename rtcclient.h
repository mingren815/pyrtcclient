#pragma once
#include "rtcinterface.h"
#include "rtcclientbase.h"

static std::string g_audioOprate = "2";
static std::string g_videoOprate = "2";
static std::string g_roomid;
static std::string g_decodeable = "2";
static std::string g_inputfile = "VideoInput.h264";

static rtc::VideoCodec g_videocodec = rtc::codec_h264;
static rtc::CameraCapability g_cap(1280, 720, 20);

class RtcClient : public RtcClientInterface, public RtcClientBase
{
public:
    enum State
    {
         initing = 0,
         initsuccess = 1,
         joining = 2,
         joinsuccess = 3,
         initfailed = 4,
         joinfailed = 5,
    };
    RtcClient();
    ~RtcClient();

    int init(std::string url, std::string token) override;
    void uninit();
    int joinRoom(std::string roomid, std::string selfUserId, std::string selfUserName) override;
    int leave(int reason) override;
    int sendPrivateMessage(int msgType, std::string message, std::string targetUserId) override;
    int sendPublicMessage(int msgType, std::string message) override;

    int publishAuditStream(char *data, int len) override;
    int publishVedioStream(char *data, int len) override;

    int subAudioStream(const std::string &targetUserId) override;
    int getAudioStream(const std::string &targetUserId, char *data, int dataSize) override;
private:
    int getState();
    int CreatRoom();

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
    // 聊天消息回调
    virtual void onPublicMessage(const AvdMessage &message);
    virtual void onPrivateMessage(const AvdMessage &message);

public:
    rtc::IRoom *m_roomobj;
    rtc::IMAudio *m_audio;
    rtc::IMVideo *m_video;
    rtc::IMChat *m_chat;
    EncodedCaptureFromFile *m_pipe;

private:
    bool m_isInitSuccess;
    std::string m_roomid;
    std::string m_userName;
    uint32 m_callid;
    rtc::User m_user;
    rtc::Camera m_fakeCamera;
    std::map<std::string, FakeEncodedFrameListener *> m_fakelistener;
    std::map<DeviceId, MyVideoRender *> m_rendermap;
};
