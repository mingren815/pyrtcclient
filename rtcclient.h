#pragma once
#include "rtcinterface.h"
#include "rtcclientbase.h"
#include "rtcyuvin.h"
#include "rtcyuvout.h"
#include "rtcaudioin.h"
#include "rtcaudioout.h"

class RtcClient : public RtcClientInterface, public RtcClientBase
{
public:
    enum RoomState
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
    int init(std::string url, std::string appkey, std::string secretkey) override;
    void uninit();
    int joinRoom(std::string roomid, std::string selfUserId, std::string selfUserName) override;
    int leave(int reason) override;
    int sendPrivateMessage(int msgType, std::string message, std::string targetUserId) override;
    int sendPublicMessage(int msgType, std::string message) override;

    int publishAuditStream(char *data, int len) override;
    int publishVedioStream(int w, int h, char *data, int len) override;

    int subAudioStream(const std::string &targetUserId) override;
    int getAudioStream(const std::string &targetUserId, char *data, int dataSize) override;

private:
    int getState();
    int CreatRoom();

private:
    virtual void onInitResult(Result result);
    virtual void onJoinResult(Result result);
    virtual void onConnectionStatus(rtc::ConnectionStatus status);
    virtual void onPublishCameraNotify(const Camera &camera);
    virtual void onUnpublishCameraNotify(const Camera &camera);
    virtual void onPublishLocalResult(Result result, const DeviceId &fromId);
    virtual void onUnpublishLocalResult(Result result, const DeviceId &fromId);
    // chat message callbak
    virtual void onPublicMessage(const AvdMessage &message);
    virtual void onPrivateMessage(const AvdMessage &message);

    // audio callback
    virtual void onMicrophoneStatusNotify(rtc::MicrophoneStatus status, const rtc::UserId &fromUserId);
    virtual void onSubscribeMicrophoneResult(Result result, const UserId &fromId);
    virtual void onUnsubscribeMicrophoneResult(Result result, const UserId &fromId);

public:
    rtc::IRoom *m_roomobj;
    rtc::IMAudio *m_audio;
    rtc::IMVideo *m_video;
    rtc::IMChat *m_chat;
    YUV420CapturePipe *m_videoPipeIn;
    AudioInPipeOnly *m_audioPipeIn;
    AudioOutPipeOnly *m_audioPipeOut;

private:
    bool m_isInitSuccess;
    bool m_isJoind;
    std::string m_roomid;
    std::string m_userName;
    uint32 m_callid;
    rtc::User m_user;
    rtc::Camera m_fakeCamera;
    std::string m_audioSubUserId;
    rtc::CameraCapability m_cameraCap;
    bool m_audioSubed;
};
