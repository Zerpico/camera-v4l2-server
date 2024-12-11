#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "NtRtspApp.h"
#include "NtDummyVideoDevice.h"
#include <spdlog/spdlog.h>
#include "avcodec_utils.h"

int main()
{
    auto web = new WebServer();

    set_external_avlogger(spdlog::default_logger());

    auto dummy_dev = NtDummyVideoDevice::createNew(DummyVideoDeviceParameters{});

    // auto rtsp = new NtRtspApp();

    // rtsp->Start();
    // web->run();
}