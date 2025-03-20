#pragma once
#include <Observer.h>
#include <NtMediaChannels.h>
#include "DataPackets.h"
class IWebServer
{
public:
    virtual void run() = 0;
};

class WebServer : public IWebServer
{

public:
    WebServer(const std::shared_ptr<Observer> &dispatcher);
    ~WebServer();
    WebServer(const WebServer &) = delete;
    void run();

private:
    std::shared_ptr<Observer> _dispatcher;
    std::shared_ptr<Listener> _listener;
    void OnMessage(std::shared_ptr<PacketData> userdata);
    int webPort = 8080;
};
