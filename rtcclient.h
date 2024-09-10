#pragma once
#include "rtcclientbase.h"

static rtc::String _url = "https://v.nice2meet.cn";
static rtc::String _appkey = "demo_access";
static rtc::String _secretkey = "demo_secret";


std::string g_url = _url;
std::string g_appkey = _appkey;
std::string g_secretkey = _secretkey;
std::string g_audioOprate = "2";
std::string g_videoOprate = "2";
std::string g_roomid;
std::string g_decodeable = "2";
std::string g_inputfile = "VideoInput.h264";


rtc::VideoCodec g_videocodec = rtc::codec_h264;
rtc::CameraCapability g_cap(1280, 720, 20);

class RtcClient : public RtcClientBase {
public:

RtcClient():m_roomobj(0),m_audio(0),m_video(0){
    m_callid = 0;
    m_user.userId = "1498201";
    m_user.userName = "kangfeng";
    m_fakeCamera.id = m_user.userId + "_123456789hahah264";
    m_fakeCamera.name = "ahahasdkfja";
    rtc::IAVDEngine::Instance();
}
~RtcClient(){ UninitEngine(); }
int InitEngine(){
        cout << "AVDEngine start" << endl;
        int result = 0;
        rtc::IAVDEngine::Instance()->uninit();
        //设置日志文件
        rtc::IAVDEngine::Instance()->setLogParams("verbose realtstamp", "mclient.log");
        AudioInProxy* in = new AudioInProxy(48000, 2);
        AudioOutProxy* out = new AudioOutProxy(48000, 2);
        GlobalDeviceManager::SetAudioInterface(in, out);
        rtc::IAVDEngine::Instance()->setOption(eo_video_codec_priority, &g_videocodec);
        rtc::IAVDEngine::Instance()->setOption(eo_camera_capability_default, &g_cap);
        //初始化
        result = rtc::IAVDEngine::Instance()->init(this, g_url, g_appkey, g_secretkey);    
        
        if (result != AVD_OK)
        {
            cout << "AVDEngine registration failed!" << endl;
            cout << "appkey: " << _appkey << endl;
            cout << "secretkey: " << _secretkey << endl;
            return -1;
        }
        else
        {
            cout << "AVDEngine initialization successful !!" << endl;
        }
        cout << "AVDEngine  end!" << endl;
        return 1;
    }
    void UninitEngine(){
        rtc::IAVDEngine::Instance()->release();
    }
    void CloseRoom(){
        if(m_audio)
        {
            m_audio->closeMicrophone();
            m_audio->setListener(NULL);
        }
        if(m_video){
            m_video->unpublishLocalCamera(m_fakeCamera.id);
            m_video->setListener(NULL);
            if(m_pipe){
            m_pipe->setStartImported(false);
            delete m_pipe;
            }
        }
        if(m_roomobj){
            m_roomobj->leave(0);
            m_roomobj->close();
        }
    }
    
    int CreatRoom(){
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
        //安排房间
        res = rtc::IAVDEngine::Instance()->scheduleRoom(m_callid, rominfo);
        cout << "res: " << res << endl;
        return res;
    }


    int JoinRoom(rtc::String roomID){
        int ret =-1;
        cout << "join room ,roomid=" << roomID;
        if (!m_roomobj){
            m_user.userId = "1498201";
            m_user.userName = "kangfeng";
            m_roomid = roomID;
            m_roomobj = rtc::IRoom::obtain(m_roomid);
            m_roomobj->setListener(this);
            m_audio = IMAudio::getAudio(m_roomobj);
            m_audio->setListener(this);
            m_video = IMVideo::getVideo(m_roomobj);
            m_video->setListener(this);
            ret = m_roomobj->join(m_user, "", 0);
        }
        cout << "END join room ,roomid=" << roomID;
        return ret;
    }
private:
    virtual void onInitResult(Result result){
        cout << "result = " << result << endl;
        cout << "This is the callback after InitEngine is initialized !!!" << endl;
        if (result == AVD_OK)
        {
            cout << "g_roomid = " << g_roomid<<endl;
            if (g_roomid.empty()){
                CreatRoom();
            }
            else{
                JoinRoom(g_roomid);
            }
        }

    }
    virtual void onScheduleRoomResult(uint32 callId, Result result, const rtc::RoomId &roomId){
        
        cout << "Room application successful!!" << endl;
        cout << "callid = " << callId << endl;
        cout << "result = " << result << endl;
        cout << "roomID = " << roomId << endl;
        m_user.userId = "1498201";
        m_user.userName = "kangfeng";
        m_roomid = roomId;
        //获取或创建房    
        m_roomobj = rtc::IRoom::obtain(m_roomid);
        m_roomobj->setListener(this);
        m_roomobj->join(m_user, "", 0);
        //设置音频，获取房间音
        m_audio = IMAudio::getAudio(m_roomobj);
        m_audio->setListener(this);
        m_video = IMVideo::getVideo(m_roomobj);
        m_video->setListener(this);
    }
    virtual void  onJoinResult(Result result){
        if (result != 0)
        {
            cout << "Join failed" << endl;

        }
        else{
            cout << "Joinf success ,roomid="<<m_roomid<<endl;
            if (!g_audioOprate.empty()){
                
                if (g_audioOprate == "0"){  ///导入音频
                    cout << "JopenMicrophone1"<<endl;
                    m_audio->openMicrophone();
                    printf("AudioInProxy!!\n");
                    //OneUser_openMicrophone;
                }
                else if (g_audioOprate == "1"){  ///导出音频
                    cout << "JopenMicrophone2"<<endl;
                    m_audio->openMicrophone();
                    //OneUser_openMicrophone;
                }
                else if (g_audioOprate == "2"){ ///导入和导出音                    
                    cout << "JopenMicrophone3"<<endl;
                    m_audio->openMicrophone();
                }
            }
            if (!g_videoOprate.empty()){
                if (g_videoOprate == "0"){ ///导入视频
                    std::cout << "publishLocalCamera0"<<endl;
                    CameraCapability cap(1280, 720, 25);
                    m_fakeCamera.publishedQualities.setStreamOptions(rtc::StreamType::stream_main, cap, rtc::codec_h264);
                    m_pipe = new EncodedCaptureFromFile(rtc::FOURCC_H264, g_inputfile.c_str());
                    m_video->publishLocalCamera(m_fakeCamera, m_pipe->GetCapture());
                    m_pipe->setStartImported(true);

                }
                else if (g_videoOprate == "1"){ ///导出视频
                    std::cout << "publishLocalCamera1"<<endl;
                }
                else if (g_videoOprate == "2"){ ///导入和导出视            
                    std::cout << "publishLocalCamera2"<<endl;
                    CameraCapability cap(1280, 720, 25);
                    m_fakeCamera.publishedQualities.setStreamOptions(rtc::StreamType::stream_main, cap, rtc::codec_h264);
                    m_pipe = new EncodedCaptureFromFile(rtc::FOURCC_H264, g_inputfile.c_str());
                    m_video->publishLocalCamera(m_fakeCamera, m_pipe->GetCapture());

                    m_pipe->setStartImported(true);
                }
            }

        }
    }
    virtual void  onConnectionStatus(rtc::ConnectionStatus status){
        std::cout<<"onConnectionStatus ,status="<<status<<endl;
        if(status == cs_connectFailed)
        {
            std::cout<<"connect failed ,leave room now !"<<endl;
            m_roomobj->leave(0);
        }
    }
    virtual void onPublishCameraNotify(const Camera& camera){
        std::string filename = "video_" + camera.id+".h264";
        if (g_videoOprate == "1" || g_videoOprate == "2" ) {
            if(g_decodeable == "2" || g_decodeable == "0") {
                FakeEncodedFrameListener * encodlistener = new FakeEncodedFrameListener(filename);
                std::map<std::string, FakeEncodedFrameListener *>::iterator it = m_fakelistener.find(camera.id);
                if (it == m_fakelistener.end()) {
                    m_fakelistener.insert(std::make_pair(camera.id, encodlistener));
                }
                m_video->subscribe(camera.id, encodlistener, true);
            }
            else if(g_decodeable == "1"){
                m_video->subscribe(camera.id);
            }
            
        }
        cout << "onPublishCameraNotify ,camera.id=" << camera.id << ",camera.name=" << camera.name <<",filename="<<filename<< endl;
    }
    virtual void onUnpublishCameraNotify(const Camera& camera){
        if (g_videoOprate == "1" || g_videoOprate == "2") {
            std::map<std::string, FakeEncodedFrameListener *>::iterator it = m_fakelistener.find(camera.id);
            if (it != m_fakelistener.end()) {
                m_video->unsubscribe(camera.id);
                delete it->second;
                m_fakelistener.erase(it);
            }
            
        }
        cout << "onUnpublishCameraNotify ,camera.id=" << camera.id << ",camera.name=" << camera.name << endl;
    }
    virtual void onSubscribeResult(Result result, const DeviceId& fromId) {
        cout<<"onSubscribeResult ,result="<<result<<",fromId="<<fromId<<endl;
        if(result == AVD_OK){
            if(g_decodeable == "1" || g_decodeable == "2"){
                std::map<DeviceId,MyVideoRender*>::iterator it = m_rendermap.find(fromId);
                if(it == m_rendermap.end()){
                    MyVideoRender* render= new MyVideoRender(fromId);
                    m_video->attachRender(fromId,render);
                }else{
                    m_video->attachRender(fromId,it->second);
                }
            }
        }
    }
    virtual void onUnsubscribeResult(Result result, const DeviceId& fromId){
        cout<<"onUnsubscribeResult ,result="<<result<<",fromId="<<fromId<<endl;
        if(result == AVD_OK){
            std::map<DeviceId,MyVideoRender*>::iterator it = m_rendermap.find(fromId);
            if(it != m_rendermap.end()){
                m_video->detachRender(it->second);
                delete it->second;
                m_rendermap.erase(it);
            }
        }
    }
    virtual void onPublishLocalResult(Result result, const DeviceId& fromId) {
        cout<<"onPublishLocalResult ,result="<<result<<",fromId="<<fromId<<endl;
        m_video->setVideoBitrate(m_fakeCamera.id, 4000 * 1000, 6000 * 1000);
    }
    virtual void onUnpublishLocalResult(Result result, const DeviceId& fromId) {
        cout<<"onUnpublishLocalResult ,result="<<result<<",DeviceId="<<fromId<<endl;
    }
public:
    rtc::IRoom* m_roomobj;
    rtc::IMAudio* m_audio;
    rtc::IMVideo* m_video;
    EncodedCaptureFromFile* m_pipe;
private:
    std::string m_roomid;
    std::string m_userName;
    uint32 m_callid;
    rtc::User m_user;
    rtc::Camera m_fakeCamera;
    std::map<std::string, FakeEncodedFrameListener *> m_fakelistener;
    std::map<DeviceId,MyVideoRender*> m_rendermap;
};


