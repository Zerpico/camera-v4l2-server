#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "NtRtspApp.h"
#include "NtDummyVideoDevice.h"
#include <spdlog/spdlog.h>

int main()
{
    auto web = new WebServer();
    auto dummy_dev = NtDummyVideoDevice::createNew(DummyVideoDeviceParameters{});
    auto rtsp = new NtRtspApp();
    auto device = new CameraDeviceSource();

    rtsp->Start();
    web->run();
}