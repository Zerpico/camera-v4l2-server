#include "CameraRtspServer.h"
#include "BasicUsageEnvironment.hh"
#include "spdlog/spdlog.h"

CameraRtspServer::CameraRtspServer(/* args */)
{
    auto scheduler = BasicTaskScheduler::createNew();
    spdlog::info("run rtsp");
}

CameraRtspServer::~CameraRtspServer()
{
}
