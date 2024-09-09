#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
//#include <boost/thread.hpp>
//#include <boost/shared_ptr.hpp>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sstream>
#include<fstream>
#include<iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>

//#include "comm.h"


#include "api/combase.h"
#include "api/common.h"
#include "api/room.h"
#include "api/avdengine.h"
#include "api/maudio.h"
#include "api/mvideo.h"
#include "api/musermanager.h"
#include "api/videocapture.h"
#include "api/devicemanager.h"
using namespace std;
using namespace tee3;
using namespace tee3::avd;

static tee3::String _url = "http://jh.3tee.cn";
static tee3::String _appkey = "demo_access";
static tee3::String _secretkey = "demo_secret";

std::string g_audioOprate;
std::string g_videoOprate;
std::string g_roomid;

tee3::avd::User user;
tee3::avd::Camera fakeCamera;
uint32 callid = 0;

void initdata(){
	user.userId = "1498201";
	user.userName = "kangfeng";
	fakeCamera.id = user.userId + "_123456789hahah264";
	fakeCamera.name = "ahahasdkfja";
}


const int g_processIntervalMS = 100;//ms

long timestamp()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}


/***************************************************************************************/
/****************************叁体引擎tee3::avd*********************************************/
/***************************************************************************************/

typedef tee3::avd::FakeVideoCapturer*	FakeVideoCapType;
const int read_buff_len = 1024 * 512;
class EncodedCaptureFromFile :
    public tee3::avd::FakeVideoCapturer::Listener {
public:
    EncodedCaptureFromFile(FourCC format, std::string filePath)
        : m_capture(NULL)
        , m_isStart(false)
        , m_format(format) {
        m_capture = FakeVideoCapturer::Create(this, m_format);
		cout << "EncodedCaptureFromFile";
        // m_networkThread = new rtc::Thread();
        // m_networkThread->SetName("H264CaptureFromFile Thread", NULL);
        // m_networkThread->Start(this);
		
		
		pthread_create(&m_thdId, NULL, EncodedCaptureFromFile::Run, (void *)this );
		
        m_threadActive = true;
	FilePaths = filePath;
        m_fd = fopen(FilePaths.c_str(), "rb");
        m_currread = 0;
        memset(m_buffer, 0, sizeof(m_buffer));
        m_currinput = read_buff_len - 4;
        printf(",open fd = %d\n", m_fd);
        fseek(m_fd, 0, SEEK_END);
        m_filesize = ftell(m_fd);
        fseek(m_fd, 0, SEEK_SET);
    }
    ~EncodedCaptureFromFile() {
        cout << "~EncodedCaptureFromFile";
        m_isStart = false;
        m_threadActive = false;
        // m_networkThread->Stop();
        // delete m_networkThread;
        // m_networkThread = NULL;
        FakeVideoCapturer::Destroy(m_capture);
        if (m_fd) {
            fclose(m_fd);
            m_fd = NULL;
        }
    }
    void setStartImported(bool enalbe) {
        m_isStart = enalbe;
    }
    virtual bool OnStart() {
        cout << "EncodedCaptureFromFile::OnStart";
        m_isStart = true;
        return true;
    }
    virtual void OnStop() {
        cout << "EncodedCaptureFromFile::OnStop";
        m_isStart = false;
        //m_capture = NULL;
    }
    static void* Run(void *pArg) {
		EncodedCaptureFromFile* pthis= (EncodedCaptureFromFile*)pArg;
	int size;
        while (pthis->m_threadActive) {
            if (!pthis->m_isStart) {
                sleep(1);
                continue;
            }
   //         cout<<"============== start run capture video."<<endl;
            int64_t before = timestamp();
		printf("ptread Failure!!!\n");
	    
            // scan buffer for a frame
            uint8* currbegin = NULL, *currend = NULL;
            int    currlen = 0;
            bool   findOneHeader = false;
            uint8* curr = &pthis->m_buffer[pthis->m_currinput];
            while (curr < &pthis->m_buffer[read_buff_len - 4]) {
                if (0 == *curr && 0 == *(curr + 1) && 0 == *(curr + 2) && 1 == *(curr + 3)) {	
                    if (!findOneHeader) {
                        findOneHeader = true;
                        currbegin = curr;
                    }
                    else if (curr - currbegin > 100) {
                        currend = curr;
                        currlen = currend - currbegin;
                        pthis->m_currinput = currend - &pthis->m_buffer[0];
                        break;
                    }
                }
                ++curr;
            }
		//	cout<<"==============currlen="<<currlen<<endl;
            if (currlen > 0) {
                //IncomingH264Data(currbegin, currlen);
				pthis->m_capture->inputEncodedFrame(0, 160, 128, currbegin, currlen);
		printf("Video read Failure\n");
				
            }
            else {
                pthis->m_currread -= (read_buff_len - pthis->m_currinput);
                if (pthis->m_currread < 0) {
                    pthis->m_currread = 0;
                }
                fseek(pthis->m_fd, pthis->m_currread, SEEK_SET);
		int size = fread(pthis->m_buffer, 1, read_buff_len, pthis->m_fd);
                if (size != read_buff_len) {
                    pthis->m_currread = 0;
                    pthis->m_currinput = 0;
                    curr = &pthis->m_buffer[0];
                    memset(pthis->m_buffer, 0, sizeof(pthis->m_buffer));
                    fseek(pthis->m_fd, 0, SEEK_SET);
                    continue;
                }
                pthis->m_currread += size;
                pthis->m_currinput = 0;
            }
            int64_t diff = 30 - (before - timestamp());
            //RTC_LOG_T_F(LS_INFO) << "diff: " << diff << ",currlen=" << currlen);
            if (diff > 10) {
				usleep((diff - 1)*1000);
				//thread->SleepMs((int)(diff - 1));
            }
        }
        cout<<"============== start run  end capture...";
		pthread_exit(NULL);
    }
    void IncomingH264Data(uint8* data, uint32 len) {
        if (!m_isStart) {
            return;
        }
        m_capture->inputEncodedFrame(0, 160, 128, data, len);

    }
    FakeVideoCapType GetCapture() {
        return m_capture;
    }
private:
    FakeVideoCapType	m_capture;
    bool				m_isStart;
    //rtc::Thread*		m_networkThread;
	pthread_t           m_thdId;
    bool				m_threadActive;
    FILE*				m_fd;
    int					m_currread;
    uint8			    m_buffer[read_buff_len];
    int					m_currinput;
    uint64              m_filesize;
    FourCC              m_format;
    std::string         FilePaths;
};

///////////////////////////////audio proxy

char *audiobuf = NULL;
class AudioInProxy : public tee3::avd::AudioInInterface {
public:
    AudioInProxy(int sampleRate, int channels)
        : m_sampleRate(sampleRate)
        , m_channels(channels)
        , m_count(0)
        , m_fd(0) {

        m_fd = fopen("AudioInProxy.PCM", "rb");
		inputsize = 0;
		audiobuf = (char*)malloc(sizeof(char) * 19200);
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
            printf("ReadData: time=%d, count= %d, sampleRate=%d, channels=%d, len=%d \r\n", time, m_count, sampleRate, channels, len);
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
};
class AudioOutProxy : public tee3::avd::AudioOutInterface {
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
            printf("WriteData: time=%d, count= %d, sampleRate=%d, channels=%d, len=%d \r\n", time, m_count, sampleRate, channels, len);
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
class MClient :public tee3::avd::IAVDEngine::IListener, public tee3::avd::IRoom::IListener,public tee3::avd::IMAudio::IListener,public tee3::avd::IMVideo::IListener{
public:
	MClient():m_roomobj(0),m_audio(0){
		
	}
	~MClient(){ UninitEngine(); }
	int InitEngine(){
		cout << "AVDEngine start" << endl;
		int result = 0;
		tee3::avd::IAVDEngine::Instance()->uninit();
		//设置日志文件
		tee3::avd::IAVDEngine::Instance()->setLogParams("info", "mclient.log");
		AudioInProxy* in = new AudioInProxy(48000, 2);
		AudioOutProxy* out = new AudioOutProxy(48000, 2);
		GlobalDeviceManager::SetAudioInterface(in, out);
		//初始化		
		result = tee3::avd::IAVDEngine::Instance()->init(this, _url, _appkey, _secretkey);	
		
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
		tee3::avd::IAVDEngine::Instance()->release();
	}
	void CloseRoom(){
		if(m_audio)
		{
			m_audio->closeMicrophone();
			m_audio->setListener(NULL);
		}
		if(m_video){
			m_video->unpublishLocalCamera(fakeCamera.id);
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
		int result = 0;
		int res = 1;
		cout << "this is CreatRoom!!" << endl;
		tee3::avd::RoomInfo rominfo;
		rominfo.roomName = "Room_Demo";
		rominfo.appRoomId = "12345678";
		rominfo.hostPassword = "";
		rominfo.duration = 3600;
		rominfo.maxAttendee = 5;
		rominfo.maxAudio = 3;
		rominfo.maxVideo = 2;
		rominfo.setRoomMode(tee3::avd::rm_mcu);
		rominfo.roomTopic = "123";
		//安排房间
		res = tee3::avd::IAVDEngine::Instance()->scheduleRoom(callid, rominfo);
		cout << "res: " << res << endl;
	}


	int JoinRoom(tee3::String roomID){
		cout << "join room ,roomid=" << roomID;
		if (!m_roomobj){
			user.userId = "1498201";
			user.userName = "kangfeng";
			m_roomid = roomID;
			m_roomobj = tee3::avd::IRoom::obtain(m_roomid);
			m_roomobj->setListener(this);
			m_audio = IMAudio::getAudio(m_roomobj);
			m_audio->setListener(this);
			m_video = IMVideo::getVideo(m_roomobj);
			m_video->setListener(this);
			m_roomobj->join(user, "", 0);
		}
		cout << "END join room ,roomid=" << roomID;
	}
	//初始化引擎后的回调
	void onInitResult(Result result){
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
	void 	onUninitResult(Result reason){
		cout << "Deinitialization AVDEngine notification!!" << endl;
	}
	void 	onGetRoomResult(uint32 callId,uint32, Result, const tee3::avd::RoomInfo&){}
	void 	onFindRoomsResult(uint32 callId, Result result, const tee3::avd::RoomInfosType &rooms){}

	void 	onScheduleRoomResult(uint32 callId, Result result, const tee3::RoomId &roomId){
		
		cout << "Room application successful!!" << endl;
		cout << "callid = " << callId << endl;
		cout << "result = " << result << endl;
		cout << "roomID = " << roomId << endl;
		user.userId = "1498201";
		user.userName = "kangfeng";
		m_roomid = roomId;
		//获取或创建房	
		m_roomobj = tee3::avd::IRoom::obtain(m_roomid);
		m_roomobj->setListener(this);
		m_roomobj->join(user, "", 0);
		//设置音频，获取房间音频	
		m_audio = IMAudio::getAudio(m_roomobj);
		m_audio->setListener(this);
		m_video = IMVideo::getVideo(m_roomobj);
		m_video->setListener(this);
	}
	void 	onCancelRoomResult(uint32 callId, Result result, const tee3::RoomId &roomId){}
	void 	onGetUsersCountResult(uint32 callId, Result result, uint32 usersCount, const tee3::RoomId &roomId){}
	void 	onCallOutgoingDeviceResult(uint32 callId, Result result, const tee3::UserId &userId){}
	
	//////room listener
	void  onJoinResult(Result result){
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
					cout << "publishLocalCamera1"<<endl;
					fakeCamera.publishedQualities.setStreamOptions(tee3::avd::stream_main, tee3::avd::quality_high, tee3::avd::codec_h264);
					m_pipe = new EncodedCaptureFromFile(tee3::avd::FOURCC_H264, "VideoInput.h264");
					m_video->publishLocalCamera(fakeCamera, m_pipe->GetCapture());
					m_pipe->setStartImported(true);
					m_video->setVideoBitrate(fakeCamera.id, 6000 * 1000, 6000 * 1000);

				}
				else if (g_videoOprate == "1"){ ///导出视频
					cout << "publishLocalCamera2"<<endl;
				}
				else if (g_videoOprate == "2"){ ///导入和导出视			
				    cout << "publishLocalCamera3"<<endl;
					fakeCamera.publishedQualities.setStreamOptions(tee3::avd::stream_main, tee3::avd::quality_high, tee3::avd::codec_h264);
					m_pipe = new EncodedCaptureFromFile(tee3::avd::FOURCC_H264, "VideoInput.h264");
					m_video->publishLocalCamera(fakeCamera, m_pipe->GetCapture());
					m_pipe->setStartImported(true);
					m_video->setVideoBitrate(fakeCamera.id, 6000 * 1000, 6000 * 1000);
				}
			}

		}
	}
	void  onLeaveIndication(Result reason, const tee3::UserId &fromId){}
	void  onPublicData(const char *data, uint32 len, const tee3::UserId &fromId){}
	void  onPrivateData(const char *data, uint32 len, const tee3::UserId &fromId){}
	void  onAppDataNotify(const std::string &key, const std::string &value){}
	void  onRoomStatusNotify(tee3::avd::RoomStatus status){}
	void  onConnectionStatus(tee3::avd::ConnectionStatus status){}
	void  onOutgoingInviteStatusNotify(tee3::avd::OutgoingInviteType type, std::string roomId, std::string addr, tee3::avd::OutgoingStatus status, std::string msg){}
	
	///////auido listener
	virtual void onMicrophoneStatusNotify(tee3::avd::MicrophoneStatus status, const tee3::UserId& fromUserId) {
		cout<<"onMicrophoneStatusNotify ,status="<<status<<",fromuid="<<fromUserId<<endl;
	}
	virtual void onAudioLevelMonitorNotify(const tee3::avd::AudioInfo& info)  {}
	virtual void onOpenMicrophoneResult(Result result)  {
		cout<<"onOpenMicrophoneResult ,result="<<result<<endl;
	}
	virtual void onCloseMicrophoneResult(Result result) {
		cout<<"onCloseMicrophoneResult ,result="<<result<<endl;
	}
	// virtual void onSubscribeResult(Result result, const tee3::UserId& fromId){}
	// virtual void onUnsubscribeResult(Result result, const tee3::UserId& fromId){}
	
	//////video listerner
	virtual void onCameraStatusNotify(CameraStatus status, const DeviceId& fromId){
		cout<<"onCameraStatusNotify ,status="<<status<<",DeviceId="<<fromId<<endl;
	}
	virtual void onCameraDataNotify(int level, const String& description, const DeviceId& fromId) {
		cout<<"onCameraDataNotify ,level="<<level<<",description="<<description<<",fromid="<<fromId<<endl;
	}
	virtual void onPublishCameraNotify(const Camera& camera){
		cout<<"onPublishCameraNotify ,camera.id="<<camera.id<<",camera.name="<<camera.name<<endl;
	}
	virtual void onUnpublishCameraNotify(const Camera& camera){
		cout<<"onUnpublishCameraNotify ,camera.id="<<camera.id<<",camera.name="<<camera.name<<endl;
	}
	virtual void onSubscribeResult(Result result, const DeviceId& fromId) {
		cout<<"onSubscribeResult ,result="<<result<<",fromId="<<fromId<<endl;
		
	}
	virtual void onUnsubscribeResult(Result result, const DeviceId& fromId){
		cout<<"onUnsubscribeResult ,result="<<result<<",fromId="<<fromId<<endl;
	}
	virtual void onPublishLocalResult(Result result, const DeviceId& fromId) {
		cout<<"onPublishLocalResult ,result="<<result<<",fromId="<<fromId<<endl;
	}
	virtual void onUnpublishLocalResult(Result result, const DeviceId& fromId) {
		cout<<"onUnpublishLocalResult ,result="<<result<<",DeviceId="<<fromId<<endl;
	}
public:
	tee3::avd::IRoom* m_roomobj;
	tee3::avd::IMAudio* m_audio;
	tee3::avd::IMVideo* m_video;
	EncodedCaptureFromFile* m_pipe;
private:
	std::string m_roomid;
	uint32 M_callid;
};

void getvalue(const std::string argument, const std::string tag, std::string&value){
	if(argument.find(tag) == std::string::npos)
		return;
	size_t pos = argument.find("=");
	if (pos != std::string::npos){
		value = argument.substr(pos + 1, argument.length() - pos - 1);
	}
}
void printfhelp(){
	printf("--help,show the help\n");
	printf("--audio=0|1|2 ,0:import,1:export,2:all\n");
	printf("--video=0|1|2 ,0:import,1:export,2:all\n");
	printf("--roomid=12345678917, if not set will shecdule new roomid.\n");
}
/**************************************************************************************/


int main(int argc, char *argv[])
{
	
	std::string shelp;
	if (argc == 0){
		printfhelp();
		return 0;
	}
	for (int i = 0; i < argc; i++){
		printf("===========argument[%d]is:%s\n", i, argv[i]);
		//getvalue(argv[i], "--help", shelp);
		getvalue(argv[i], "--audio", g_audioOprate);
		getvalue(argv[i], "--video", g_videoOprate);
		getvalue(argv[i], "--roomid", g_roomid);
	}
	
	if (g_audioOprate.empty() && g_videoOprate.empty()){
		printf("--roomid:%s,--audio;%s,--video:%s\n", g_roomid.c_str(), g_audioOprate.c_str(), g_videoOprate.c_str());
		printfhelp();
		return 0;
	}
	printf("--roomid:%s,--audio;%s,--video:%s\n", g_roomid.c_str(), g_audioOprate.c_str(), g_videoOprate.c_str());
	initdata();
	MClient mcc;
	int res = 0;
	res = mcc.InitEngine();
	if (res < 0)
		return -1;

	char quit;
	while (true){
		cin >> quit;
		if (quit == 'Q' || quit == 'q'){
			break;
		}
		sleep(1);
	}
	mcc.CloseRoom();
	mcc.UninitEngine();
	return 0;
}


