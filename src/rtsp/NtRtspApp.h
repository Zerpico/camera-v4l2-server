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
    virtual bool run() = 0;
    virtual bool stop() = 0;
};

class NtRtspApp : public INtRtspApp
{

public:
    // NtRtspApp(CDispatcherBase *dispatcher, unsigned short rtspPort = 554, int timeout = 10);
    NtRtspApp(const std::shared_ptr<CDispatcherBase> &dispatcher);
    ~NtRtspApp();
    bool run();
    bool stop();

    ServerMediaSession *addUnicastSession(const std::string &url, StreamReplicator *videoReplicator, StreamReplicator *audioReplicator);
    ServerMediaSession *addSession(const std::string &sessionName, const std::list<ServerMediaSubsession *> &subSession);

private:
    void runThread();

private:
    std::atomic_char f_state_{1};
    std::thread thread_capture_{};

    TaskScheduler *scheduler;
    NtUsageEnvironment *env;
    NtRTSPServer *rtsp_server;

    // CDispatcherBase *_dispatcher;
    std::shared_ptr<CDispatcherBase> _dispatcher;
    std::shared_ptr<CListener> _listener;
    void OnMessage(void *userdata);

    int rtspPort = 554;
};