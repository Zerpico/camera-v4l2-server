#include "CameraDeviceSource.h"
#include "spdlog/spdlog.h"

extern "C"
{
#include "libavcodec/avcodec.h"
}

CameraDeviceSource::CameraDeviceSource(/* args */)
{
    spdlog::info(av_version_info());
}

CameraDeviceSource::~CameraDeviceSource()
{
}
