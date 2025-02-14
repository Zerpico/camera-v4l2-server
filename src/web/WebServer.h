#pragma once
#include <Observer.h>
#include <NtMediaChannels.h>

class IWebServer
{
public:
    virtual void run() = 0;
};

class WebServer : public IWebServer
{

public:
    WebServer(const std::shared_ptr<CDispatcherBase> &dispatcher);
    ~WebServer();
    WebServer(const WebServer &) = delete;
    void run();

private:
    std::shared_ptr<CDispatcherBase> _dispatcher;
    std::shared_ptr<CListener> _listener;
    void OnMessage(void *userdata);
};
