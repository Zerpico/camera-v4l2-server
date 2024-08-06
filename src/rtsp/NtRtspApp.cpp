#include "NtRtspApp.h"
#include "NtRtspServer.h"
#include "spdlog/spdlog.h"

NtRtspApp::NtRtspApp(unsigned short rtspPort, int timeout)
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
}

NtRtspApp::~NtRtspApp()
{
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
