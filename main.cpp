#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <map>
#include <sys/time.h>
#include <fcntl.h>

#include "rtcclient.h"

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
    printf("--audio=0|1|2 ,0:import,1:export,2:all,default is 2\n");
    printf("--video=0|1|2 ,0:import,1:export,2:all,default is 2\n");
    printf("--decodeable=0|1|2,0:only export encoded data,1:only export decoded data,2:endoded and yuv data all will be exported,deault is 2\n");
    printf("--roomid=12345678917, if not set will shecdule new roomid.\n");
    printf("--url ,server site url,default is https://v.nice2meet.cn\n");
    printf("--appkey ,appkey ,default is demo_access.\n");
    printf("--secretkey ,secretkey,default is demo_secret.\n");
    printf("--inputfile,default is VideoInput.h264 in the current directory ");
}
/**************************************************************************************/
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

int main(int argc, char *argv[])
{
    
    std::string shelp;
    if (argc == 0){
        printfhelp();
        return 0;
    }
        
    for (int i = 0; i < argc; i++){
        printf("===========argument[%d]is:%s\n", i, argv[i]);
        std::string argstr = argv[i];
        if(argstr.find("--help") != std::string::npos){
            printfhelp();
        return 0;
        }
        getvalue(argv[i], "--audio", g_audioOprate);
        getvalue(argv[i], "--video", g_videoOprate);
        getvalue(argv[i], "--roomid", g_roomid);
        getvalue(argv[i], "--url", g_url);
        getvalue(argv[i], "--appkey", g_appkey);
        getvalue(argv[i], "--secretkey", g_secretkey);
        getvalue(argv[i], "--decodeable", g_decodeable);
        getvalue(argv[i], "--inputfile", g_inputfile);
    }
    
    if (g_audioOprate.empty() && g_videoOprate.empty()){
        printf("--roomid:%s,--audio;%s,--video:%s\n", g_roomid.c_str(), g_audioOprate.c_str(), g_videoOprate.c_str());
        printfhelp();
        return 0;
    }
    printf("--decodeable:%s,--inputfile;%s\n", g_decodeable.c_str(), g_inputfile.c_str());
    printf("--url:%s,--appkey;%s,--secretkey:%s\n", g_url.c_str(), g_appkey.c_str(), g_secretkey.c_str());
    printf("--roomid:%s,--audio;%s,--video:%s\n", g_roomid.c_str(), g_audioOprate.c_str(), g_videoOprate.c_str());
    RtcClient mcc;
    int res = 0;
    res = mcc.init(g_url, g_appkey, g_secretkey);
    printf("============================================================res:%d...\n",res);
    if (res < 0)
        return -1;

    char quit;
    if (g_roomid.empty()){
       mcc.CreatRoom();
    }
    else{
       mcc.joinRoom(g_roomid, "AITestUserId","AITestUserName");
    }
    while (true){
        cin>>quit;
        printf("============================================================step2...\n");
        if (quit == 'Q' || quit == 'q'){
            printf("quit...\n");
            break;
        }
    }
    printf("end program\n");
    mcc.leave(0);
    mcc.uninit();
    return 0;
}


