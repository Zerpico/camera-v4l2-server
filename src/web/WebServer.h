#pragma once
#include <Observer.h>
#include <NtMediaChannels.h>

class WebServer
{

public:
    WebServer(CDispatcherBase *dispatcher);
    ~WebServer();
    void run();

private:
    CDispatcherBase *_dispatcher;
    std::shared_ptr<CListener> _listener;
    void OnMessage(void *userdata);
};
