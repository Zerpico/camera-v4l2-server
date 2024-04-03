#include "CameraRtspServer.h"
#include "WebServer.h"

int main()
{
    auto rtsp = new CameraRtspServer();
    auto web = new WebServer();
    web->run();
}