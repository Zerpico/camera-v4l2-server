#include "WebServer.h"
#include <drogon/drogon.h>

using namespace drogon;

WebServer::WebServer()
{
    app()
        .setLogLevel(trantor::Logger::kDebug)
        .addListener("0.0.0.0", 80);
}

WebServer::~WebServer()
{
    app().quit();
}

void WebServer::run()
{
    app().run();
}