#include "NtRtspApp.h"
#include "NtRtspServer.h"
#include "spdlog/spdlog.h"
#include "UnicastServerMediaSubsession.h"

NtRtspApp::NtRtspApp(CDispatcherBase *dispatcher, unsigned short rtspPort, int timeout) : _dispatcher(dispatcher)
{
    scheduler = BasicTaskScheduler::createNew();
    env = NtUsageEnvironment::createNew(*scheduler);
    UserAuthenticationDatabase *authDB = nullptr;

    rtsp_server = NtRTSPServer::createNew(*env, rtspPort, authDB);

    if (rtsp_server == NULL)
    {
        spdlog::error("Failed to create rtsp server ::%s", env->getResultMsg());
        return;
    }

    _listener = std::make_shared<CListener>();
    _listener->SetMessageFunc(std::bind(&NtRtspApp::OnMessage, this, std::placeholders::_1));
    _dispatcher->Subscribe(_listener);
}

NtRtspApp::~NtRtspApp()
{
    _dispatcher->Unsubscribe(_listener->GetSubscriberId());

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

void NtRtspApp::OnMessage(void *userdata)
{
    int *value = static_cast<int *>(userdata);
    spdlog::info("OnMessage called, value: {} , from SubscriberId {}", *value, _listener->GetSubscriberId());
}

bool NtRtspApp::Start()
{
    if (f_state_ == 0)
        return false;

    thread_capture_ = std::thread(&NtRtspApp::RunThread, this);
    f_state_ = 0;

    return true;
}

bool NtRtspApp::Stop()
{
    return false;
}

void NtRtspApp::RunThread()
{
    spdlog::info("Start rtsp thread");

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

ServerMediaSession *NtRtspApp::AddUnicastSession(const std::string &url, StreamReplicator *videoReplicator, StreamReplicator *audioReplicator)
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