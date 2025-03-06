#pragma once
#include "rtcinterface.h"
#include "rtcclientbase.h"
#include "rtcyuvin.h"
#include "rtcyuvout.h"
#include "rtcaudioin.h"
#include "rtcaudioout.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include <cstdint>
        
namespace py = pybind11;

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

    int load(std::string url, std::string appkey, std::string secretkey, bool enablelog) override;
    // int load(std::string url, std::string token, bool enablelog) override;
    void uninit();
    int joinRoom(std::string roomid, std::string selfUserId, std::string selfUserName) override;
    int loadAndJoinRoom(std::string url, std::string token, bool enablelog, std::string roomid, std::string selfUserId, std::string selfUserName, int waitSeconds) override;
    int leave(int reason) override;
    int sendPrivateMessage(int msgType, std::string message, std::string targetUserId) override;
    int sendPublicMessage(int msgType, std::string message) override;

    int publishAudioStream(char *data, int len) override;
    int publishVedioStream(int w, int h, char *data, int len) override;

    int subAudioStream(const std::string &targetUserId) override;
    // int getAudioStream(const std::string &targetUserId, char *data, int dataSize) override;
    std::vector<int16_t> getAudioStream(const std::string &targetUserId, int dataSize );
    virtual void onInitResult(Result result);
    std::vector<std::string> getPublicHistoryMessage(bool latest, uint32 begindex, uint32& ncount);
    MessagesType getPublicHistoryMessageByType(bool latest, uint32 begindex, uint32& ncount);



    // 设置离开指示回调
    void setLeaveIndicationCallback(py::object callback);


public:
    int getState();
    int ScheduleRoom();
    int JoinRoomInternal();

private:
    void onJoinResult(Result result);
    void onConnectionStatus(rtc::ConnectionStatus status);
    void onPublishCameraNotify(const Camera &camera);
    void onUnpublishCameraNotify(const Camera &camera);
    void onPublishLocalResult(Result result, const DeviceId &fromId);
    void onUnpublishLocalResult(Result result, const DeviceId &fromId);
    // void onSubscribeResult(Result result, const DeviceId &fromId);
    // void onUnsubscribeResult(Result result, const DeviceId &fromId);
    // chat message callbak
    void onPublicMessage(const AvdMessage &message);
    void onPrivateMessage(const AvdMessage &message);
    void onLeaveIndication(Result reason, const rtc::UserId& fromId);

    // audio callback
    void onMicrophoneStatusNotify(rtc::MicrophoneStatus status, const rtc::UserId &fromUserId);
    void onSubscribeMicrophoneResult(Result result, const UserId &fromId);
    void onUnsubscribeMicrophoneResult(Result result, const UserId &fromId);

    // create room callback ,only for test 
    virtual void onScheduleRoomResult(uint32 callId, Result result, const rtc::RoomId &roomId);

    void onStartMixrecord(uint32 callId, Result result, String msg, String recordid);
    void onStopMixrecord(uint32 callId, Result result, String msg, String data);
    void onCreateMedia(uint32 callId, Result result, String msg, String playid);
    void onPlayMedia(uint32 callId, Result result, String msg, String data);
    void onStopMedia(uint32 callId, Result result, String msg);
public:
    rtc::IRoom *m_roomobj;
    rtc::IMAudio *m_audio;
    rtc::IMVideo *m_video;
    rtc::IMChat *m_chat;
    YUV420CapturePipe *m_videoPipeIn;
    AudioDeviceInDumy *m_audioDeviceIn;
    AudioDeviceOutDumy *m_audioDeviceOut;
    AudioPcmOut *m_audioPcmOut;
    bool m_isInitSuccess;
    int m_isJoinSuccess;
    uint32 m_callid;
    std::mutex m_mtx;

private:
    bool m_isJoind;
    std::string m_roomid;
    std::string m_userName;
    rtc::User m_user;
    rtc::Camera m_fakeCamera;
    std::string m_audioSubUserId;
    rtc::CameraCapability m_cameraCap;
    bool m_audioSubed;
    bool m_createRoomForTest;
    std::map<std::string, AudioPcmOut*>  m_audioPcmOutMap;

    // 回调相关的成员变量
    std::mutex callback_mutex_;
    py::object leave_indication_callback_;
};



