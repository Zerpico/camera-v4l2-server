#include "WebServer.h"
#include <iostream>
#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"

using namespace drogon;

WebServer::WebServer()
{
    trantor::Logger::LogLevel logLevel = trantor::Logger::kInfo;
    app()
        .setLogLevel(logLevel)
        .setLogPath(".", "log", 1000000, 1024 * 8096, true)
        .setupFileLogger()
        .addListener("0.0.0.0", 8080);

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
}

WebServer::~WebServer()
{
    app().quit();
}

void WebServer::run()
{
    spdlog::info("run webserver on :8080");
    app().run();
}
