#include "CameraRtspServer.h"
#include "WebServer.h"

int main()
{
    auto rtsp = new CameraRtspServer();
    auto web = new WebServer();
    web->run();
    // auto servis();
    // LOG_INFO << "User constructor!";
    // app()
    //     .setLogPath("./")
    //     .setLogLevel(trantor::Logger::kWarn)
    //     .addListener("0.0.0.0", 80)
    //     .setThreadNum(16)
    //     .run();
    // app().loadConfigFile("./config.json").run();
}