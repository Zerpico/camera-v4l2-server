#include "WebServer.h"
#include <iostream>
#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "globs.h"

using namespace drogon;

WebServer::WebServer(const std::shared_ptr<IObserverEvent> &dispatcher) : _dispatcher(dispatcher), webPort(8080)
{
    trantor::Logger::LogLevel logLevel = trantor::Logger::kInfo;
    app()
        .setLogLevel(logLevel)
        .setDocumentRoot("./www")
        .setClientMaxBodySize(200 * 1024 * 1024) // max 200 mb
        .enableSession(std::chrono::duration<double>(1200))
        .setLogPath(".", "log", 1000000, 1024 * 8096, true)
        .setupFileLogger()
        .addListener("0.0.0.0", webPort);

    getGlobalDictionary()[UPLOADPATH] = app().getUploadPath();

    auto logger = trantor::Logger::getSpdLogger();

    // set console logger
    spdlog::sink_ptr console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    logger->sinks().push_back(console_sink);

    logger->set_level((spdlog::level::level_enum)logLevel);
    // set log format for all sinks
    for (auto &sink : logger->sinks())
        sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%3!l%$] %v - %s:%#");

    spdlog::set_default_logger(logger);
    auto lev = logger->level();

    auto listener = std::make_shared<ListenerEventProcessor>("webserver");
    //_listener->onEvent()
    listener->setOnEventFunc(std::bind(&WebServer::OnMessage, this, std::placeholders::_1));
    // _listener->setOnEventFunc(std::bind(&WebServer::OnMessage, this, std::placeholders::_1));
    // _listener->SetMessageFunc(std::bind(&WebServer::OnMessage, this, std::placeholders::_1));
    _listener = listener;
    // _dispatcher->subscribe(_listener);
}

WebServer::~WebServer()
{
    app().quit();
    _dispatcher->unsubscribe(_listener);
}

void WebServer::run()
{
    spdlog::info("run http on :{}", webPort);
    app().run();
}

void WebServer::OnMessage(std::shared_ptr<BasePacketData> userdata)
{
    // int *value = static_cast<int *>(userdata);
    spdlog::info("OnMessage called, size: {} , from SubscriberId {}", userdata->size(), "webserver");
}
