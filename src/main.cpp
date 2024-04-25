#include "CameraDevice.h"
#include "RtspServer.h"
#include "WebServer.h"

int main()
{
    auto camera = new CameraDevice();
    auto webserver = new WebServer();
    auto rtsp = new RtspServer();
    webserver->run();
}