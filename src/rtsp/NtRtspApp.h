#pragma once
#include <thread>
#include "NtUsageEnvironment.h"
#include "NtRtspServer.h"

class NtRtspApp
{

public:
    NtRtspApp(unsigned short rtspPort = 554, int timeout = 10);
    ~NtRtspApp();
    bool Start();
    bool Stop();

private:
    void RunThread();

private:
    char f_state_;
    std::thread thread_capture_{};

    TaskScheduler *scheduler;
    NtUsageEnvironment *env;
    NtRTSPServer *rtsp_server;
};