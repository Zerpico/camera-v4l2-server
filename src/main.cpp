#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "NtRtspApp.h"
#include "NtDummyVideoDevice.h"

int main()
{
    auto dummy_dev = NtDummyVideoDevice::createNew(DummyVideoDeviceParameters{});

    auto rtsp = new NtRtspApp();
    auto web = new WebServer();
    auto device = new CameraDeviceSource();

    rtsp->Start();
    web->run();
}