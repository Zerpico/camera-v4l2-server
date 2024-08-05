#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "CameraRtspServer.h"

int main()
{
    auto web = new WebServer();
    auto rtsp = new CameraRtspServer();
    web->run();
}