#include "rtcclient.h"

static rtc::VideoCodec g_videocodec = rtc::codec_h264;
static int g_sampelRate = 16000;
static int g_channels = 1;
static int g_sziePerSample = 2;

RtcClient::RtcClient() : m_roomobj(0), m_audio(0), m_video(0), m_chat(0), m_isInitSuccess(false), m_isJoind(false), m_cameraCap(1280, 720, 20), m_audioSubed(false)
{
    m_callid = 0;
    m_fakeCamera.id = m_user.userId + "_virturevideoid";
    m_fakeCamera.name = "fakevideoin";
    rtc::IAVDEngine::Instance();
}
RtcClient::~RtcClient()
{
    if (m_isJoind)
    {
        leave(0);
    }
    uninit();
}

int RtcClient::init(std::string url, std::string token)
{
    // token  需要转换成三体的token
    std::string tokenTee3;
    //
    cout << "AVDEngine start" << endl;
    int result = 0;
    rtc::IAVDEngine::Instance()->uninit();
    // 设置日志文件
    rtc::IAVDEngine::Instance()->setLogParams("verbose realtstamp", "mclient.log");

    rtc::IAVDEngine::Instance()->setOption(eo_video_codec_priority, &g_videocodec);
    rtc::IAVDEngine::Instance()->setOption(eo_camera_capability_default, &m_cameraCap);
    // 初始化
    result = rtc::IAVDEngine::Instance()->init(this, url, tokenTee3);

    if (result != AVD_OK)
    {
        cout << "AVDEngine registration failed!" << endl;
        cout << "url: " << url << endl;
        cout << "token: " << token << endl;
        return -1;
    }
    else
    {
        cout << "AVDEngine initialization successful !!" << endl;
    }
    cout << "AVDEngine  end!" << endl;
    return result;
}

int RtcClient::init(std::string url, std::string appkey, std::string secretkey)
{

    //
    cout << "AVDEngine start" << endl;
    int result = 0;
    rtc::IAVDEngine::Instance()->uninit();
    // 设置日志文件
    rtc::IAVDEngine::Instance()->setLogParams("verbose realtstamp", "mclient.log");

    rtc::IAVDEngine::Instance()->setOption(eo_video_codec_priority, &g_videocodec);
    rtc::IAVDEngine::Instance()->setOption(eo_camera_capability_default, &m_cameraCap);
    // 初始化
    result = rtc::IAVDEngine::Instance()->init(this, url, appkey, secretkey);

    if (result != AVD_OK)
    {
        cout << "AVDEngine registration failed!" << endl;
        cout << "url: " << url << endl;
        cout << "appkey: " << appkey << endl;
        cout << "secretkey: " << secretkey << endl;
        return -1;
    }
    else
    {
        cout << "AVDEngine initialization successful !!" << endl;
    }
    cout << "AVDEngine  end!" << endl;
    return result;
}
void RtcClient::uninit()
{
    rtc::IAVDEngine::Instance()->uninit();
    rtc::IAVDEngine::Instance()->release();
}

int RtcClient::joinRoom(std::string roomid, std::string userid, std::string username)
{
    int ret = 0;
    cout << "join room ,roomid=" << roomid;
    m_isJoind = true;
    m_user.userId = userid;
    m_user.userName = username;
    m_roomid = roomid;
    if (!m_roomobj)
    {
        m_roomobj = rtc::IRoom::obtain(m_roomid);
        m_roomobj->setListener(this);
        m_audio = IMAudio::getAudio(m_roomobj);
        m_audio->setListener(this);
        m_video = IMVideo::getVideo(m_roomobj);
        m_video->setListener(this);
        m_chat = IMChat::getChat(m_roomobj);
        m_chat->setListener(this);

        m_audioPipeIn = new AudioInPipeOnly(g_sampelRate, g_channels);
        GlobalDeviceManager::SetAudioInterface(m_audioPipeIn, NULL);
        m_audioPipeOut = new AudioOutPipeOnly(g_sampelRate, g_channels);
        bool autoSubAudio = false;
        m_roomobj->setOption(ro_audio_auto_subscribe, &autoSubAudio);
    }
    if (m_isInitSuccess)
    {
        ret = m_roomobj->join(m_user, "", 0);
    }
    cout << "END join room ,roomid=" << roomid;
    return ret;
}
int RtcClient::leave(int reason)
{
    int ret = 0;
    m_isJoind = false;
    if (m_audio)
    {
        m_audio->closeMicrophone();
        if (m_audioSubed)
        {
            m_audio->unsubscribe(m_audioSubUserId);
        }
        m_audio->setListener(NULL);
        m_audio = NULL;
        if (m_audioPipeIn)
        {
            delete m_audioPipeIn;
            m_audioPipeIn = NULL;
        }
        if (m_audioPipeOut)
        {
            delete m_audioPipeOut;
            m_audioPipeOut = NULL;
        }
    }
    if (m_video)
    {
        m_video->unpublishLocalCamera(m_fakeCamera.id);
        m_video->setListener(NULL);
        m_video = NULL;
        if (m_videoPipeIn)
        {
            delete m_videoPipeIn;
            m_videoPipeIn = NULL;
        }
    }
    if (m_chat)
    {
        m_chat->setListener(NULL);
        m_chat = NULL;
    }
    if (m_roomobj)
    {
        ret = m_roomobj->leave(reason);
        // 房间不是自己创建的 这里不关闭服务器上的房间号 自己离开就好
        // m_roomobj->close();
        m_roomobj->release();
        m_roomobj = NULL;
    }
    return ret;
}
int RtcClient::sendPrivateMessage(int msgType, std::string message, std::string userid)
{
    // need to repacket msgType
    if (m_chat)
    {
        m_chat->sendPrivateMessage(message, userid);
    }
}
int RtcClient::sendPublicMessage(int msgType, std::string message)
{
    // need to repacket msgType
    if (m_chat)
    {
        m_chat->sendPublicMessage(message);
    }
}

int RtcClient::publishAuditStream(char *data, int len)
{
    if (!m_audio || !m_audioPipeIn)
    {
        return -1;
    }
    m_audioPipeIn->inputAudioSample(data, len);
    return 0;
}
int RtcClient::publishVedioStream(int w, int h, char *data, int len)
{
    if (!m_video || !m_videoPipeIn)
    {
        return -1;
    }
    if (m_cameraCap.width != w || m_cameraCap.height != h)
    {
        m_cameraCap.width = w;
        m_cameraCap.height = h;
        m_video->updateCaptureCapability(m_fakeCamera.id, m_cameraCap);
    }
    return m_videoPipeIn->inputCapturedFrame(w, h, (const uint8 *)data, len);
}

int RtcClient::subAudioStream(const std::string &targetUserId)
{

    if (!m_audio)
    {
        return -1;
    }
    if (m_audio->isAudioPublished(targetUserId))
    {
        return m_audio->subscribe(targetUserId);
    }
    // wait for audio sub notify
    m_audioSubUserId = targetUserId;
    return 0;
}
int RtcClient::getAudioStream(const std::string &targetUserId, char *data, int dataSize)
{
    if (!m_audio || !m_audioPipeOut)
    {
        return 0;
    }
    return m_audioPipeOut->getAudioData(targetUserId, data, dataSize);
}
int RtcClient::CreatRoom()
{
    int res = -1;
    cout << "this is CreatRoom!!" << endl;
    rtc::RoomInfo rominfo;
    rominfo.roomName = "Room_Demo";
    rominfo.appRoomId = "";
    rominfo.hostPassword = "";
    rominfo.duration = 3600;
    rominfo.maxAttendee = 5;
    rominfo.maxAudio = 3;
    rominfo.maxVideo = 2;
    rominfo.setRoomMode(rtc::rm_mcu);
    rominfo.roomTopic = "123";
    // 安排房间
    res = rtc::IAVDEngine::Instance()->scheduleRoom(m_callid, rominfo);
    cout << "res: " << res << endl;
    return res;
}
void RtcClient::onInitResult(Result result)
{
    cout << "result = " << result << endl;
    cout << "This is the callback after InitEngine is initialized !!!" << endl;
    if (result == AVD_OK)
    {
        m_isInitSuccess = true;
    }
    if (m_isInitSuccess && !m_roomid.empty() && m_roomobj)
    {
        m_roomobj->join(m_user, "", 0);
    }
}

void RtcClient::onJoinResult(Result result)
{
    if (result != 0)
    {
        cout << "Join failed" << endl;
    }
    else
    {
        cout << "Joinf success ,roomid=" << m_roomid << endl;
        if (m_audio)
        {
            m_audio->openMicrophone();
        }
        // 导入和导出视
        if (m_video)
        {
            std::cout << "publishLocalCamera" << endl;
            CameraCapability cap(1280, 720, 25);
            m_fakeCamera.publishedQualities.setStreamOptions(rtc::StreamType::stream_main, cap, rtc::codec_h264);
            m_videoPipeIn = new YUV420CapturePipe(rtc::FOURCC_I420);
            m_video->publishLocalCamera(m_fakeCamera, m_videoPipeIn->GetCapture());
        }
    }
}
void RtcClient::onConnectionStatus(rtc::ConnectionStatus status)
{
    std::cout << "onConnectionStatus ,status=" << status << endl;
    if (status == cs_connectFailed)
    {
        std::cout << "connect failed ,leave room now !" << endl;
        m_roomobj->leave(0);
    }
}
void RtcClient::onPublishCameraNotify(const Camera &camera)
{
    cout << "onPublishCameraNotify ,camera.id=" << camera.id << ",camera.name=" << camera.name << endl;
}
void RtcClient::onUnpublishCameraNotify(const Camera &camera)
{
    cout << "onUnpublishCameraNotify ,camera.id=" << camera.id << ",camera.name=" << camera.name << endl;
}

void RtcClient::onPublishLocalResult(Result result, const DeviceId &fromId)
{
    cout << "onPublishLocalResult ,result=" << result << ",fromId=" << fromId << endl;
    m_video->setVideoBitrate(m_fakeCamera.id, 4000 * 1000, 6000 * 1000);
}
void RtcClient::onUnpublishLocalResult(Result result, const DeviceId &fromId)
{
    cout << "onUnpublishLocalResult ,result=" << result << ",DeviceId=" << fromId << endl;
}

void RtcClient::onPublicMessage(const AvdMessage &message)
{
}
void RtcClient::onPrivateMessage(const AvdMessage &message)
{
}
void RtcClient::onMicrophoneStatusNotify(rtc::MicrophoneStatus status, const rtc::UserId &fromUserId)
{
    if (!m_audio)
    {
        return;
    }
    if (rtc::MicrophoneStatus::ds_published == status && fromUserId == m_audioSubUserId)
    {
        m_audio->subscribe(m_audioSubUserId);
        m_audioSubed = true;
    }
    else if (rtc::MicrophoneStatus::ds_ready == status && fromUserId == m_audioSubUserId && m_audioSubed)
    {
        m_audio->unregisterPCMDataListener(m_audioSubUserId);
        m_audio->unsubscribe(m_audioSubUserId);
        m_audioSubed = false;
    }
}
void RtcClient::onSubscribeMicrophoneResult(Result result, const UserId &fromId)
{
    if (!m_audio)
    {
        return;
    }
    if (AVD_OK == result && fromId == m_audioSubUserId && m_audioPipeOut)
    {
        m_audio->registerPCMDataListener(m_audioSubUserId, m_audioPipeOut, m_audioPipeOut->sampleRate(), m_audioPipeOut->channels());
    }
}
void RtcClient::onUnsubscribeMicrophoneResult(Result result, const UserId &fromId)
{
}