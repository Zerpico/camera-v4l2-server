#pragma once
#include <thread>
#include "NtUsageEnvironment.h"
#include "NtRtspServer.h"
#include <Observer.h>
#include <StreamReplicator.hh>
#include <list>

class INtRtspApp
{
public:
    virtual bool Start() = 0;
    virtual bool Stop() = 0;
};

class NtRtspApp : public INtRtspApp
{

public:
    // NtRtspApp(CDispatcherBase *dispatcher, unsigned short rtspPort = 554, int timeout = 10);
    NtRtspApp(const std::shared_ptr<CDispatcherBase> &dispatcher);
    ~NtRtspApp();
    bool Start();
    bool Stop();

    ServerMediaSession *AddUnicastSession(const std::string &url, StreamReplicator *videoReplicator, StreamReplicator *audioReplicator);
    ServerMediaSession *addSession(const std::string &sessionName, const std::list<ServerMediaSubsession *> &subSession);

private:
    void RunThread();

private:
    char f_state_;
    std::thread thread_capture_{};

    TaskScheduler *scheduler;
    NtUsageEnvironment *env;
    NtRTSPServer *rtsp_server;

    // CDispatcherBase *_dispatcher;
    std::shared_ptr<CDispatcherBase> _dispatcher;
    std::shared_ptr<CListener> _listener;
    void OnMessage(void *userdata);
};