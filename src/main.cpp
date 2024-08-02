#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "CameraRtspServer.h"

int main()
{
    auto rtsp = new CameraRtspServer();
    auto web = new WebServer();
    web->run();
}