#include <drogon/drogon.h>
using namespace drogon;

int main()
{
    LOG_INFO << "User constructor!";
    app()
        .setLogPath("./")
        .setLogLevel(trantor::Logger::kWarn)
        .addListener("0.0.0.0", 80)
        .setThreadNum(16)
        .run();
    // app().loadConfigFile("./config.json").run();
}