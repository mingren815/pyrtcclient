#include "rtcclient.h"

static rtc::VideoCodec g_videocodec = rtc::codec_h264;
static int g_sampelRate = 16000;
static int g_channels = 1;
static int g_sziePerSample = 2;

RtcClient::RtcClient()
    : m_roomobj(0), m_audio(0), m_video(0), m_chat(0), m_isInitSuccess(false), m_isJoind(false), m_cameraCap(1280, 720, 20), m_audioSubed(false), m_createRoomForTest(false)
{
    m_callid = 0;
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

// 初始化接口，开启叁体向我们鉴权之后不可用
int RtcClient::load(std::string url, std::string appkey, std::string secretkey, bool enablelog)
{

   //
   cout << "AVDEngine start" << endl;
   int result = 0;
   // 设置日志文件
   if (enablelog){
       rtc::IAVDEngine::Instance()->setLogParams("verbose realtstamp", "mclient.log");
   }
   // device must set before rtc::IAVDEngine::Instance()->init
   GlobalDeviceManager::SetAudioInterface(0, 0);

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

//初始化接口，开启叁体向我们鉴权之后传递uap的token
// int RtcClient::load(std::string url, std::string token, bool enablelog)
// {
//     cout << "AVDEngine start" << endl;
//     int result = 0;

//     // 设置日志文件
//     if (enablelog) {
//         rtc::IAVDEngine::Instance()->setLogParams("verbose realtstamp", "mclient.log");
//     }

//     // 设备必须在初始化引擎前设置  
//     GlobalDeviceManager::SetAudioInterface(0, 0);

//     // 设置引擎选项 
//     rtc::IAVDEngine::Instance()->setOption(eo_video_codec_priority, &g_videocodec);
//     rtc::IAVDEngine::Instance()->setOption(eo_camera_capability_default, &m_cameraCap);

//     // 设置叁体向我们鉴权,该选项开启后,必须使用uap的token，不需要携带Bearer 
//     rtc::IAVDEngine::Instance()->setCustomerTokenEnabled(true);

//     // 初始化
//     result = rtc::IAVDEngine::Instance()->init(this, url, token);
//     cout << "url: " << url << endl;
//     cout << "token: " << token << endl;

//     if (result != AVD_OK)
//     {
//         cout << "AVDEngine registration failed!" << endl;
//         cout << "url: " << url << endl;
//         cout << "token: " << token << endl;
//         return -1;
//     }
//     cout << "AVDEngine initialization successful !!" << endl;
//     cout << "AVDEngine end!" << endl;
//     return result;
// }

void RtcClient::uninit()
{
    rtc::IAVDEngine::Instance()->uninit();
    rtc::IAVDEngine::Instance()->release();
}

int RtcClient::joinRoom(std::string roomid, std::string userid, std::string username)
{
    std::lock_guard<std::mutex> gurad(m_mtx);
    int ret = 0;
    cout << "join room ,roomid=" << roomid << endl;
    cout << "join room ,userId=" << userid << endl;
    cout << "join room ,username=" << username << endl;
    m_isJoind = true;
    m_user.userId = userid;
    m_user.userName = username;
    m_roomid = roomid;
    m_fakeCamera.id = userid + "_virturevideoid";
    m_fakeCamera.name = "fakevideoin";

    if (m_isInitSuccess && rtc::IAVDEngine::Instance()->isWorking())
    {
        ret = JoinRoomInternal();
    }
    cout << "END join room ,roomid=" << roomid << "roomdcode=" << ret << endl;
    return ret;
}

int RtcClient::loadAndJoinRoom(std::string url, std::string token, bool enablelog, std::string roomid, std::string userid, std::string username, int waitseonds)
{
    std::lock_guard<std::mutex> gurad(m_mtx);
    cout << "AVDEngine start" << endl;
    int result = 0;

    m_isJoinSuccess = 0;
    m_isJoind = true;
    m_user.userId = userid;
    m_user.userName = username;
    m_roomid = roomid;
    m_fakeCamera.id = userid + "_virturevideoid";
    m_fakeCamera.name = "fakevideoin";

    // 设置日志文件
    if (enablelog) {
        rtc::IAVDEngine::Instance()->setLogParams("verbose realtstamp", "mclient.log");
    }

    // 设备必须在初始化引擎前设置  
    GlobalDeviceManager::SetAudioInterface(0, 0);

    // 设置引擎选项 
    rtc::IAVDEngine::Instance()->setOption(eo_video_codec_priority, &g_videocodec);
    rtc::IAVDEngine::Instance()->setOption(eo_camera_capability_default, &m_cameraCap);

    // 设置叁体向我们鉴权,该选项开启后,必须使用uap的token，不需要携带Bearer 
    rtc::IAVDEngine::Instance()->setCustomerTokenEnabled(true);

    // 初始化
    result = rtc::IAVDEngine::Instance()->init(this, url, token);
    cout << "url: " << url << endl;
    cout << "token: " << token << endl;

    if (result != AVD_OK)
    {
        cout << "AVDEngine registration failed!" << endl;
        cout << "url: " << url << endl;
        cout << "token: " << token << endl;
        return -1;
    }
    cout << "AVDEngine initialization successful !!" << endl;

    cout << "join room ,roomid=" << roomid << endl;
    cout << "join room ,userId=" << userid << endl;
    cout << "join room ,username=" << username << endl;
    cout << "Wait for join room" << endl;
    //循环等待m_isInitSuccess变为true，直到超时退出循环
    auto start = std::chrono::steady_clock::now();
    std::chrono::milliseconds timeout(1000 * waitseonds); // 10秒超时
    while (m_isJoinSuccess == 0 && (std::chrono::steady_clock::now() - start <= timeout)) {
        // 等待一段时间再检查，以减少CPU占用
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (m_isJoinSuccess == 1) {
        std::cout << "Join room success!!" << std::endl;
        return 0;
    }
    std::cerr << "Join room failed with result: " << m_isJoinSuccess << std::endl;
    return -1;
}

int RtcClient::leave(int reason)
{
    cout << "leave room " << endl;
    std::lock_guard<std::mutex> gurad(m_mtx);
    int ret = 0;
    m_isJoind = false;
    if (m_audio)
    {
        m_audio->closeMicrophone();
        for (const auto& pair : m_audioPcmOutMap) {
            cout << "leave room and unsubscribe audio stream, userid=" << pair.first << endl;
            m_audio->unsubscribe(pair.first);
            // delete pair.second;
        }
        m_audioPcmOutMap.clear();
        m_audio->setListener(NULL);
        m_audio = NULL;
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
    if (m_audioDeviceIn)
    {
        delete m_audioDeviceIn;
        m_audioDeviceIn = NULL;
    }
    if (m_audioDeviceOut)
    {
        delete m_audioDeviceOut;
        m_audioDeviceOut = NULL;
    }
    if (m_audioPcmOut)
    {
        delete m_audioPcmOut;
        m_audioPcmOut = NULL;
    }
    return ret;
}
int RtcClient::sendPrivateMessage(int msgType, std::string message, std::string userid)
{
    // need to repacket msgType
    if (m_chat)
    {
        return m_chat->sendPrivateMessage(message, userid);
    }
    return -1;
}
int RtcClient::sendPublicMessage(int msgType, std::string message)
{
    // need to repacket msgType
    try
    {
        if (m_chat)
        {
            return m_chat->sendPublicMessage(message);
        }
        return -1;
    }
    catch (const std::exception& e)
    {
        // 捕获所有标准异常，并打印异常信息
        cout << "Exception caught: " << e.what() << endl;
        return -1;
    }
    catch (...)
    {
        // 捕获所有其他类型的异常
        cout << "Unknown exception caught" << endl;
        return -1;
    }
}
std::vector<std::string> RtcClient::getPublicHistoryMessage(bool latest, uint32 begindex, uint32& ncount)
{

    if (m_chat)
    {
        MessagesType tempmsgs;
        std::vector<std::string> msgs;
        m_chat->getPublicHistoryMessage(latest, begindex, ncount, tempmsgs);
        for (const auto& msg : tempmsgs)
        {
            msgs.push_back(msg.message);
        }
        return msgs;
    }
    return std::vector<std::string>();
}

MessagesType RtcClient::getPublicHistoryMessageByType(bool latest, uint32 begindex, uint32& ncount)
{

    if (m_chat)
    {
        MessagesType tempmsgs;
        m_chat->getPublicHistoryMessage(latest, begindex, ncount, tempmsgs);
        return tempmsgs;
    }
    return MessagesType();
}

int RtcClient::publishAudioStream(char* data, int len)
{
    if (!m_audio || !m_audioDeviceIn)
    {
        return -1;
    }
    m_audioDeviceIn->inputAudioSample(data, len);
    return 0;
}
int RtcClient::publishVedioStream(int w, int h, char* data, int len)
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
    return m_videoPipeIn->inputCapturedFrame(w, h, (const uint8*)data, len);
}

int RtcClient::subAudioStream(const std::string& targetUserId)
{

    if (!m_audio)
    {
        return -1;
    }
    if (m_audio->isAudioPublished(targetUserId))
    {
        cout << "subscribeAudio ,targetUserId=" << targetUserId << endl;
        m_audioSubUserId = targetUserId;
        return m_audio->subscribe(targetUserId);
    }
    // wait for audio sub notify
    cout << "wait subscribeAudio ,targetUserId=" << targetUserId << endl;
    m_audioSubUserId = targetUserId;
    return 0;
}

std::vector<int16_t> RtcClient::getAudioStream(const std::string& targetUserId, int dataSize)
{

    //cout << "getAudioStream is called, targetUserId=" << targetUserId << endl;
    AudioPcmOut* pcmout = NULL;
    if (m_audioPcmOutMap.find(targetUserId) != m_audioPcmOutMap.end()) {
        //cout << "user is already subscribed, targetUserId=" << targetUserId << endl;
        pcmout = m_audioPcmOutMap[targetUserId];
    }
    else {
        //cout << "can not get AudioPcmOut by targetUserId=" << targetUserId << endl;
    }

    if (!m_audio || !pcmout)
    {
        return {}; // 返回空的 vector
    }
    // 确保分配的内存大小是 int16_t 的整数倍
    size_t allocSize = (dataSize / sizeof(int16_t)) * sizeof(int16_t);
    int16_t* data = new int16_t[allocSize / sizeof(int16_t)];

    int size = pcmout->getAudioData(targetUserId, reinterpret_cast<char*>(data), allocSize);
    if (size <= 0) {
        delete[] data;
        return {};
    }
    std::vector<int16_t> vecData(data, data + size / sizeof(int16_t));
    delete[] data; // 清理分配的内存
    return vecData;
}

int RtcClient::ScheduleRoom()
{
    if (m_isInitSuccess)
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
    else
    {
        m_createRoomForTest = true;
    }
    return -1;
}
int RtcClient::JoinRoomInternal()
{
    try
    {
        if (!m_roomobj)
        {
            m_roomobj = rtc::IRoom::obtain(m_roomid);
            if (!m_roomobj)
            {
                cout << "join room ,obtain room failed=" << endl;
                return -1;
            }
            cout << "join room ,obtain=" << endl;
            m_roomobj->setListener(this);
            m_audio = IMAudio::getAudio(m_roomobj);
            m_audio->setListener(this);
            m_video = IMVideo::getVideo(m_roomobj);
            m_video->setListener(this);
            m_chat = IMChat::getChat(m_roomobj);
            m_chat->setListener(this);

            cout << "join room ,obtain 2=" << endl;
            m_audioDeviceIn = new AudioDeviceInDumy(g_sampelRate, g_channels);
            m_audioDeviceOut = new AudioDeviceOutDumy(g_sampelRate, g_channels);
            cout << "join room ,obtain 3=" << endl;
            GlobalDeviceManager::SetAudioInterface(m_audioDeviceIn, m_audioDeviceOut);
            cout << "join room ,obtain 4=" << endl;
            m_audioPcmOut = new AudioPcmOut(g_sampelRate, g_channels);
            bool autoSubAudio = false;
            cout << "join room ,obtain 5=" << endl;
            m_roomobj->setOption(ro_audio_auto_subscribe, &autoSubAudio);
        }
    }
    catch (const std::exception& e)
    {
        // 捕获所有标准异常，并打印异常信息
        cout << "Exception caught: " << e.what() << endl;
        return -1;
    }
    catch (...)
    {
        // 捕获所有其他类型的异常
        cout << "Unknown exception caught" << endl;
        return -1;
    }
    return m_roomobj->join(m_user, "", 0);
}

void RtcClient::onInitResult(Result result)
{
    std::cout << "Initialization result = " << static_cast<int>(result) << std::endl;
    std::cout << "This is the callback after InitEngine is initialized !!!" << std::endl;
    if (result == AVD_OK) {
        m_isInitSuccess = true;

        // 如果初始化成功并且房间ID不为空，则尝试加入房间
        if (!m_roomid.empty()) {
            JoinRoomInternal();
        }

        // 如果初始化成功且需要创建测试房间，则安排房间创建
        if (m_createRoomForTest) {
            ScheduleRoom();
        }
    }
    else {
        // 处理初始化失败的情况（如果需要）
        std::cerr << "Initialization failed with result: " << static_cast<int>(result) << std::endl;
    }
}

void RtcClient::onJoinResult(Result result)
{
    if (result != 0)
    {
        m_isJoinSuccess = 2;
        cout << "Join failed" << endl;
    }
    else
    {
        m_isJoinSuccess = 1;
        cout << "Joinf success ,roomid=" << m_roomid << endl;
        if (m_audio)
        {
            std::cout << "openMicrophone" << endl;
            m_audio->openMicrophone();
        }
        // 导入和导出视频
        if (m_video)
        {
            std::cout << "publishLocalCamera" << endl;
            m_fakeCamera.publishedQualities.setStreamOptions(rtc::StreamType::stream_main, m_cameraCap, rtc::codec_h264);
            m_videoPipeIn = new YUV420CapturePipe(rtc::FOURCC_24BG);
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
void RtcClient::onPublishCameraNotify(const Camera& camera)
{
    cout << "onPublishCameraNotify ,camera.id=" << camera.id << ",camera.name=" << camera.name << endl;
}
void RtcClient::onUnpublishCameraNotify(const Camera& camera)
{
    cout << "onUnpublishCameraNotify ,camera.id=" << camera.id << ",camera.name=" << camera.name << endl;
}

void RtcClient::onPublishLocalResult(Result result, const DeviceId& fromId)
{
    cout << "onPublishLocalResult ,result=" << result << ",fromId=" << fromId << endl;
    m_video->setVideoBitrate(m_fakeCamera.id, 4000 * 1000, 6000 * 1000);
    m_video->updateCaptureCapability(m_fakeCamera.id, m_cameraCap);
}
void RtcClient::onUnpublishLocalResult(Result result, const DeviceId& fromId)
{
    cout << "onUnpublishLocalResult ,result=" << result << ",DeviceId=" << fromId << endl;
}

void RtcClient::onPublicMessage(const AvdMessage& message)
{
    std::cout << "onPublicMessage ,message==" << message.message << endl;
}
void RtcClient::onPrivateMessage(const AvdMessage& message)
{
    std::cout << "onPrivateMessage ,message=" << message.message << endl;
}

// 用户离开回调异步处理
void RtcClient::onLeaveIndication(Result reason, const rtc::UserId& fromId) {
    std::cout << "onLeaveIndication callback ,reason=" << reason << ",fromId=" << fromId << endl;
    if (leave_indication_callback_) {
        py::gil_scoped_acquire acquire;
        try {
            leave_indication_callback_(reason, fromId);
        }
        catch (const py::error_already_set& e) {
            PyErr_Print();
        }
    }
}

// 设置回调函数
void RtcClient::setLeaveIndicationCallback(py::object callback) {
    std::lock_guard<std::mutex> guard(callback_mutex_);
    leave_indication_callback_ = callback;
}

void RtcClient::onMicrophoneStatusNotify(rtc::MicrophoneStatus status, const rtc::UserId& fromUserId)
{
    cout << "onMicrophoneStatusNotify ,status=" << status << ",fromUserId=" << fromUserId << endl;
    if (!m_audio)
    {
        return;
    }
    if (rtc::MicrophoneStatus::ds_published == status)
    {
        m_audio->subscribe(fromUserId);
        m_audioSubed = true;
    }
    else if (rtc::MicrophoneStatus::ds_ready == status || rtc::MicrophoneStatus::ds_none == status || rtc::MicrophoneStatus::ds_muted == status)
    {
        m_audio->unregisterPCMDataListener(fromUserId);
        m_audio->unsubscribe(fromUserId);
        m_audioSubed = false;
    }
}
void RtcClient::onSubscribeMicrophoneResult(Result result, const UserId& fromId)
{
    cout << "onSubscribeMicrophoneResult ,result=" << result << ",fromUserId=" << fromId << ",m_audioSubUserId=" << m_audioSubUserId << endl;
    if (!m_audio)
    {
        return;
    }
    AudioPcmOut* pcmout = NULL;
    if (m_audioPcmOutMap.find(fromId) != m_audioPcmOutMap.end()) {
        cout << "user " << fromId << " is already in map" << fromId << endl;
        pcmout = m_audioPcmOutMap[fromId];
    }
    else {
        cout << "user " << fromId << " is not in map" << fromId << endl;
        pcmout = new AudioPcmOut(g_sampelRate, g_channels);
        m_audioPcmOutMap[fromId] = pcmout;
    }

    cout << "registerPCMDataListener " << ",m_audioPcmOut=" << m_audioPcmOut << endl;
    if (AVD_OK == result)
    {
        m_audioSubed = true;
        if (pcmout)
        {
            cout << "registerPCMDataListener " << ",fromUserId=" << fromId << endl;
            m_audio->registerPCMDataListener(fromId, pcmout, pcmout->sampleRate(), pcmout->channels());
        }
    }
    else {
        m_audioSubed = false;
    }
}
void RtcClient::onUnsubscribeMicrophoneResult(Result result, const UserId& fromId)
{
}
void RtcClient::onScheduleRoomResult(uint32 callId, Result result, const rtc::RoomId& roomId)
{
    if (AVD_OK == result)
    {
        cout << "Room application successful!!" << endl;
        cout << "callid = " << callId << endl;
        cout << "result = " << result << endl;
        cout << "roomID = " << roomId << endl;
        std::string userId = "AITestUserId";
        std::string userName = "AITestUserName";
        joinRoom(roomId, userId, userName);
    }
    else
    {
        cout << "Room application Failed!!" << endl;
        cout << "callid = " << callId << endl;
        cout << "result = " << result << endl;
        cout << "roomID = " << roomId << endl;
        cout << "Test end!!" << endl;
    }
}

void RtcClient::onStartMixrecord(uint32 callId, Result result, String msg, String recordid)
{
    cout << "onStartMixrecord ,callId=" << callId << ",result=" << result << ",msg=" << msg << ",recordid=" << recordid << endl;
}
void RtcClient::onStopMixrecord(uint32 callId, Result result, String msg, String data)
{
    cout << "onStopMixrecord ,callId=" << callId << ",result=" << result << ",msg=" << msg << ",data=" << data << endl;
}
void RtcClient::onCreateMedia(uint32 callId, Result result, String msg, String playid)
{
    cout << "onCreateMedia ,callId=" << callId << ",result=" << result << ",msg=" << msg << ",playid=" << playid << endl;
}
void RtcClient::onPlayMedia(uint32 callId, Result result, String msg, String data)
{
    cout << "onPlayMedia ,callId=" << callId << ",result=" << result << ",msg=" << msg << ",data=" << data << endl;
}
void RtcClient::onStopMedia(uint32 callId, Result result, String msg)
{
    cout << "onStopMedia ,callId=" << callId << ",result=" << result << ",msg=" << msg << endl;
}

class PyRtcClient : public RtcClient
{
public:
    /* Inherit the constructors */
    using RtcClient::RtcClient;
};

PYBIND11_MODULE(rtc_client, m)
{

    py::class_<RtcClient, PyRtcClient>(m, "RtcClient")
        .def(py::init([]()
            {
                auto* client = new RtcClient();
                // 初始化成员变量
                client->m_roomobj = 0;
                client->m_audio = 0;
                client->m_video = 0;
                client->m_chat = 0;
                client->m_isInitSuccess = false;
                client->m_callid = 0;
                // 调用任何必要的初始化代码
                rtc::IAVDEngine::Instance();
                return client; }))
        .def("__del__", [](RtcClient* self)
            {
                // 这里可以放置任何需要在对象销毁前执行的代码
                // 但是通常不需要，因为析构函数会自动被调用
                delete self; })
        .def("load", &RtcClient::load, "Loads the client with the given URL, token, and optionally enables logging.",
            py::arg("url"), py::arg("appkey"), py::arg("secretkey"),py::arg("enablelog") = false)
        .def("uninit", &RtcClient::uninit)
        .def("joinRoom", &RtcClient::joinRoom)
        .def("loadAndJoinRoom", &RtcClient::loadAndJoinRoom)
        .def("leave", &RtcClient::leave)
        .def("sendPrivateMessage", &RtcClient::sendPrivateMessage)
        .def("sendPublicMessage", &RtcClient::sendPublicMessage)
        .def("getPublicHistoryMessage", &RtcClient::getPublicHistoryMessage)
        .def("getPublicHistoryMessageByType", &RtcClient::getPublicHistoryMessageByType)
        .def("publishAudioStream", &RtcClient::publishAudioStream)
        .def("publishVedioStream", &RtcClient::publishVedioStream)
        .def("onInitResult", &RtcClient::onInitResult)
        .def("subAudioStream", &RtcClient::subAudioStream)
        .def("getAudioStream", &RtcClient::getAudioStream)
        .def("setLeaveIndicationCallback", &RtcClient::setLeaveIndicationCallback);
    // .def("getAudioStream", &RtcClient::getAudioStream,
    //      py::return_value_policy::take_ownership,
    //      py::implicitly_convertible<std::vector<char>, py::bytes>());

    py::class_<AvdMessage>(m, "AvdMessage")
        .def(py::init<>())
        .def_readwrite("timestamp", &AvdMessage::timestamp)
        .def_readwrite("fromId", &AvdMessage::fromId)
        .def_readwrite("fromName", &AvdMessage::fromName)
        .def_readwrite("message", &AvdMessage::message);

    py::class_<MessagesType>(m, "MessagesType")
        .def(py::init<>())
        .def("__iter__", [](const MessagesType& msgs)
            { return py::make_iterator(msgs.begin(), msgs.end()); }, py::keep_alive<0, 1>()) // Keep the(MessagesType) alive while iterator is used
        .def("__len__", &MessagesType::size);
}
