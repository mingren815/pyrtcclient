#pragma once

#include <string>

class RtcClientInterface
{
public:

    RtcClientInterface() {}
    virtual ~RtcClientInterface() {}

public:

    
    virtual int init(std::string url, std::string token) = 0;
    virtual int init(std::string url, std::string appkey, std::string secretkey)  = 0;
    virtual int joinRoom(std::string roomid,  std::string selfUserId, std::string selfUserName) = 0;
    virtual int leave(int reason) = 0;
    virtual int sendPrivateMessage(int msgType, std::string message, std::string targetUserId) = 0;
    virtual int sendPublicMessage(int msgType, std::string message) = 0;

    virtual int publishAuditStream(char *data, int len) = 0;
    virtual int publishVedioStream(int w, int h, char *data, int len) = 0;

    virtual int subAudioStream(const std::string &targetUserId) = 0;
    virtual int getAudioStream(const std::string &targetUserId, char *data, int dataSize) = 0;
};