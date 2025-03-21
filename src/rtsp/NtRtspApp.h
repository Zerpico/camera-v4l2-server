#pragma once
#include <thread>
#include "NtUsageEnvironment.h"
#include "NtRtspServer.h"
#include <Observer.h>
#include <StreamReplicator.hh>
#include <list>
#include "DataPackets.h"
#include "NtChannelManager.h"

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
    NtRtspApp(const std::shared_ptr<IObserverEvent> &dispatcher, const std::shared_ptr<INtChannelManager> &channelManager);
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

    std::shared_ptr<IObserverEvent> _dispatcher;
    std::shared_ptr<INtChannelManager> _channelManager;

    std::shared_ptr<IListenerEvent> _listenerEvent;
    std::shared_ptr<IListenerChannel> _listenerChannel;
    void OnMessage(std::shared_ptr<BasePacketData> userdata);
    void OnChannel(const NtChannel &channel, ChannelEvent event);
    int rtspPort = 554;
};

class ObserverChannelSource : public IListenerChannel
{
public:
    ObserverChannelSource() {}

    void setOnChannelFunc(std::function<void(const NtChannel, ChannelEvent)> handler)
    {
        _handler = handler;
    }

    void onChannelEvent(const NtChannel &channel, ChannelEvent event)
    {
        if (_handler != nullptr)
            _handler(channel, event);
    }

private:
    std::function<void(const NtChannel &, ChannelEvent)> _handler = NULL;
};