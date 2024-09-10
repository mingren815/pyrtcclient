#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <map>
#include <sys/time.h>
#include <fcntl.h>

#include "api/combase.h"
#include "api/common.h"
#include "api/room.h"
#include "api/avdengine.h"
#include "api/maudio.h"
#include "api/mvideo.h"
#include "api/mchat.h"
#include "api/musermanager.h"
#include "api/videocapture.h"
#include "api/devicemanager.h"
#include "api/videorender.h"

using namespace std;
using namespace rtc;

/***************************************************************************************/
/****************************叁体引擎rtc*********************************************/
/***************************************************************************************/

class RtcClientBase
    : public rtc::IAVDEngine::IListener,
      public rtc::IRoom::IListener,
      public rtc::IMAudio::IListener,
      public rtc::IMVideo::IListener,
      public rtc::IMChat::IListener
{
public:
    RtcClientBase() {}
    ~RtcClientBase() {}

    // 初始化引擎后的callback
    virtual void onInitResult(Result result) {}
    virtual void onUninitResult(Result reason)
    {
        cout << "Deinitialization AVDEngine notification!!" << endl;
    }
    virtual void onGetRoomResult(uint32, Result, const rtc::RoomInfo &) {}
    virtual void onFindRoomsResult(uint32 callId, Result result, const rtc::RoomInfosType &rooms) {}
    virtual void onStartMixrecord(uint32 callId, Result result) {}
    virtual void onStopMixrecord(uint32 callId, Result result) {}
    virtual void onScheduleRoomResult(uint32 callId, Result result, const rtc::RoomId &roomId) {}
    virtual void onCancelRoomResult(uint32 callId, Result result, const rtc::RoomId &roomId) {}
    virtual void onGetUsersCountResult(uint32 callId, Result result, uint32 usersCount, const rtc::RoomId &roomId) {}
    virtual void onCallOutgoingDeviceResult(uint32 callId, Result result, const rtc::UserId &userId) {}

    //////room listener
    virtual void onJoinResult(Result result) {}
    virtual void onLeaveIndication(Result reason, const rtc::UserId &fromId) {}
    virtual void onPublicData(const char *data, uint32 len, const rtc::UserId &fromId) {}
    virtual void onPrivateData(const char *data, uint32 len, const rtc::UserId &fromId) {}
    virtual void onAppDataNotify(const std::string &key, const std::string &value) {}
    virtual void onRoomStatusNotify(rtc::RoomStatus status) {}
    virtual void onConnectionStatus(rtc::ConnectionStatus status) {}
    virtual void onOutgoingInviteStatusNotify(rtc::OutgoingInviteType type, std::string roomId, std::string addr, rtc::OutgoingStatus status, std::string msg) {}
    virtual void onStreamKeepAliveTimeoutNotify(std::string userId, std::string userName, int mediaType, std::string deviceId, int ssrc) {}
    ///////auido listener
    virtual void onMicrophoneStatusNotify(rtc::MicrophoneStatus status, const rtc::UserId &fromUserId)
    {
        cout << "onMicrophoneStatusNotify ,status=" << status << ",fromuid=" << fromUserId << endl;
    }
    virtual void onAudioLevelMonitorNotify(const rtc::AudioInfo &info) {}
    virtual void onOpenMicrophoneResult(Result result)
    {
        cout << "onOpenMicrophoneResult ,result=" << result << endl;
    }
    virtual void onCloseMicrophoneResult(Result result)
    {
        cout << "onCloseMicrophoneResult ,result=" << result << endl;
    }
    virtual void onSubscribeMicrophoneResult(Result result, const UserId &fromId) {}

    virtual void onUnsubscribeMicrophoneResult(Result result, const UserId &fromId) {}

    //////video listerner
    virtual void onCameraStatusNotify(CameraStatus status, const DeviceId &fromId)
    {
        cout << "onCameraStatusNotify ,status=" << status << ",DeviceId=" << fromId << endl;
    }
    virtual void onCameraDataNotify(int level, const String &description, const DeviceId &fromId)
    {
        cout << "onCameraDataNotify ,level=" << level << ",description=" << description << ",fromid=" << fromId << endl;
    }
    virtual void onCameraCapabilityNotify(const rtc::DeviceId &fromId, const uint32 width, const uint32 height, const uint32 fps)
    {
        cout << "onCameraCapabilityNotify ,fromId=" << fromId << ",width=" << width << ",height=" << height << endl;
    }
    virtual void onPublishCameraNotify(const Camera &camera)
    {
        cout << "onPublishCameraNotify ,camera.id=" << camera.id << ",camera.name=" << camera.name << endl;
    }
    virtual void onUnpublishCameraNotify(const Camera &camera)
    {
        cout << "onUnpublishCameraNotify ,camera.id=" << camera.id << ",camera.name=" << camera.name << endl;
    }
    virtual void onSubscribeResult(Result result, const DeviceId &fromId)
    {
        cout << "onSubscribeResult ,result=" << result << ",fromId=" << fromId << endl;
    }
    virtual void onUnsubscribeResult(Result result, const DeviceId &fromId)
    {
        cout << "onUnsubscribeResult ,result=" << result << ",fromId=" << fromId << endl;
    }
    virtual void onPublishLocalResult(Result result, const DeviceId &fromId)
    {
        cout << "onPublishLocalResult ,result=" << result << ",fromId=" << fromId << endl;
    }
    virtual void onUnpublishLocalResult(Result result, const DeviceId &fromId)
    {
        cout << "onUnpublishLocalResult ,result=" << result << ",DeviceId=" << fromId << endl;
    }

    // mchat 模块消息回调
    virtual void onPublicMessage(const AvdMessage &message) {};

    virtual void onPrivateMessage(const AvdMessage &message) {};

    /** 音频数据回调接口
     * @param[in] timestamp 时间戳, mi second。
     * @param[in] sampleRate 采样率。
     * @param[in] channels 通道数。
     * @param[in] buf 数据存储指针。
     * @param[in] len 数据长度。
     */
    virtual void onAudioData(const UserId &userId, uint64 timestamp, uint32 sampleRate, uint32 channels, const void *data, uint32 len) {}
};
