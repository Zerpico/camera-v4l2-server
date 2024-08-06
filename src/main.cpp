#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "NtRtspApp.h"

int main()
{
    auto rtsp = new NtRtspApp();
    auto web = new WebServer();
    auto device = new CameraDeviceSource();

    rtsp->Start();
    web->run();
}