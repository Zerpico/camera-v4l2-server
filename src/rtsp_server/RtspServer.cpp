#include "RtspServer.h"
#include "NtUsageEnvironment.h"
#include "BasicUsageEnvironment.hh"
#include "spdlog/spdlog.h"

RtspServer::RtspServer(/* args */)
{
    auto sheduler = BasicTaskScheduler::createNew();
    auto env = NtUsageEnvironment::createNew(*sheduler);

    spdlog::info("create rtsp");
}

RtspServer::~RtspServer()
{
}
