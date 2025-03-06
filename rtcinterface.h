#pragma once

#include <string>
#include <vector>
typedef uint64_t      uint64;  

class RtcClientInterface
{
public:

    RtcClientInterface() {}
    virtual ~RtcClientInterface() {}

public:
    
    virtual int load(std::string url, std::string appkey, std::string secretkey, bool enablelog) = 0;
    // virtual int load(std::string url, std::string token, bool enablelog) = 0;
    virtual int joinRoom(std::string roomid,  std::string selfUserId, std::string selfUserName) = 0;
    virtual int loadAndJoinRoom(std::string url, std::string token, bool enablelog, std::string roomid, std::string selfUserId, std::string selfUserName, int waitseonds) = 0;
    virtual int leave(int reason) = 0;
    virtual int sendPrivateMessage(int msgType, std::string message, std::string targetUserId) = 0;
    virtual int sendPublicMessage(int msgType, std::string message) = 0;

    virtual int publishAudioStream(char *data, int len) = 0;
    virtual int publishVedioStream(int w, int h, char *data, int len) = 0;

    virtual int subAudioStream(const std::string &targetUserId) = 0;
    // virtual std::vector<char> getAudioStream(const std::string &targetUserId, int dataSize ) = 0;
};