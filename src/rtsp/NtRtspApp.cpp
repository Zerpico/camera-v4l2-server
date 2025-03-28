#include "NtRtspApp.h"
#include "NtRtspServer.h"
#include "spdlog/spdlog.h"
#include "UnicastServerMediaSubsession.h"
#include "DataPackets.h"

// NtRtspApp::NtRtspApp(CDispatcherBase *dispatcher, unsigned short rtspPort, int timeout) : _dispatcher(dispatcher)
NtRtspApp::NtRtspApp(const std::shared_ptr<IObserverEvent> &dispatcher,
                     const std::shared_ptr<INtChannelManager> &channelManager,
                     const std::shared_ptr<IDeviceManager> &deviceManager) : _dispatcher(dispatcher),
                                                                             _channelManager(channelManager),
                                                                             _deviceManager(deviceManager),
                                                                             rtspPort(554)
{
    scheduler = BasicTaskScheduler::createNew();
    env = NtUsageEnvironment::createNew(*scheduler, spdlog::level::debug);
    UserAuthenticationDatabase *authDB = nullptr;

    rtsp_server = NtRTSPServer::createNew(*env, rtspPort, authDB);

    if (rtsp_server == NULL)
    {
        spdlog::error("Failed to create rtsp server ::%s", env->getResultMsg());
        return;
    }

    OutPacketBuffer::increaseMaxSizeTo(6242880); // 1M

    // subscribe on update channels
    auto listenerChannel = std::make_shared<ObserverChannelSource>();
    listenerChannel->setOnChannelFunc(std::bind(&NtRtspApp::OnChannel, this, std::placeholders::_1, std::placeholders::_2));
    _listenerChannel = listenerChannel;
    _channelManager->subscribe(_listenerChannel);

    // subscribe on new events
    auto listenerEvent = std::make_shared<ListenerEventProcessor>("rtspserver");
    listenerEvent->setOnEventFunc(std::bind(&NtRtspApp::OnMessage, this, std::placeholders::_1));
    _listenerEvent = listenerEvent;
    // _dispatcher->subscribe(_listenerEvent);
}

NtRtspApp::~NtRtspApp()
{
    _dispatcher->unsubscribe(_listenerEvent);

    if (f_state_ == 1)
        return;

    f_state_ = 1;
    if (thread_capture_.joinable())
        thread_capture_.join();

    spdlog::info("Stop rtsp thread");
    Medium::close(rtsp_server);
    env->reclaim();
    delete scheduler;
}

void NtRtspApp::OnMessage(std::shared_ptr<BasePacketData> userdata)
{
    // PacketData *value = static_cast<PacketData *>(userdata);
    // spdlog::info("OnMessage called, size: {} , from SubscriberId {}", userdata->size(), _listener->GetSubscriberId());
    // delete value;
}

void NtRtspApp::OnChannel(const NtChannel &channel, ChannelEvent event)
{
    if (event == ChannelEvent::Added)
    {
        auto number = std::format("{:02}", channel.number);
        auto newSession = std::shared_ptr<ServerMediaSession>(ServerMediaSession::createNew(*env, number.c_str(), "LolKek_4eburek_RTSP", "LolKek_4eburek_RTSP"), [](ServerMediaSession *se)
                                                              { if (se) Medium::close(se); });
        _sessions[channel.id] = newSession;
    }

    if (event == ChannelEvent::Updated)
    {
        auto session = _sessions[channel.id];
        auto device = _deviceManager->getDevice(channel.id);
        auto format = device->getVideoFormat();
        if (channel.enable)
        {
            auto replicator = createStreamReplicator(env, device);
            session->addSubsession(UnicastServerMediaSubsession::createNew(*env, replicator));
            rtsp_server->addServerMediaSession(session.get());
            spdlog::info("start stream on {}", getRtspUrl(session.get()));
        }
    }

    if (event == ChannelEvent::Removed)
    {
        auto session = _sessions[channel.id];
        rtsp_server->deleteServerMediaSession(session.get());
    }
}

StreamReplicator *NtRtspApp::createStreamReplicator(UsageEnvironment *env, std::shared_ptr<NtDeviceInterface> devCapture)
{
    StreamReplicator *replicator = NULL;
    FramedSource *framedSource = DeviceVideoSource::createNew(*env, _dispatcher, devCapture); // DeviceSourceFactory::createFramedSource(env, format, devCapture, queueSize, captureMode, outfd, repeatConfig);
    if (framedSource != NULL)
    {
        // extend buffer size if needed
        // if (devCapture->getBufferSize() > OutPacketBuffer::maxSize)
        // {
        //     OutPacketBuffer::maxSize = devCapture->getBufferSize();
        // }
        replicator = StreamReplicator::createNew(*env, framedSource, false);
    }
    return replicator;
}

bool NtRtspApp::run()
{
    if (f_state_ == 0)
        return false;

    f_state_ = 0;
    thread_capture_ = std::thread(&NtRtspApp::runThread, this);

    return true;
}

bool NtRtspApp::stop()
{
    f_state_ = 1;
    return false;
}

void NtRtspApp::runThread()
{
    spdlog::info("run rtsp on :{}", rtspPort);
    // do the loop
    while (true)
    {
        // If we're stopping then exit loop, else try and open the stream again
        if (f_state_ != 0)
            break;

        /* The actual work is all carried out inside the LIVE555 Task scheduler */
        env->taskScheduler().doEventLoop(&f_state_); // does not return
    }
}

ServerMediaSession *NtRtspApp::addUnicastSession(const std::string &url, StreamReplicator *videoReplicator, StreamReplicator *audioReplicator)
{
    // Create Unicast Session
    std::list<ServerMediaSubsession *> subSession;
    if (videoReplicator)
    {
        subSession.push_back(UnicastServerMediaSubsession::createNew(*env, videoReplicator));
    }
    if (audioReplicator)
    {
        subSession.push_back(UnicastServerMediaSubsession::createNew(*env, audioReplicator));
    }
    return addSession(url, subSession);
}

ServerMediaSession *NtRtspApp::addSession(const std::string &sessionName, const std::list<ServerMediaSubsession *> &subSession)
{
    ServerMediaSession *sms = NULL;
    if (subSession.empty() == false)
    {
        sms = ServerMediaSession::createNew(*env, sessionName.c_str());
        if (sms != NULL)
        {
            std::list<ServerMediaSubsession *>::const_iterator subIt;
            for (subIt = subSession.begin(); subIt != subSession.end(); ++subIt)
            {
                sms->addSubsession(*subIt);
            }

            rtsp_server->addServerMediaSession(sms);

            char *url = rtsp_server->rtspURL(sms);
            if (url != NULL)
            {
                *env << "Play this stream using the URL \"" << url << "\"";
                delete[] url;
            }
        }
    }
    return sms;
}