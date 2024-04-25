#pragma once

#if defined(_MSC_VER)
//  Microsoft
#ifdef MAKEDLL
#define EXPORT __declspec(dllexport)
#endif
#else
#define EXPORT
#endif

class EXPORT WebServer
{

public:
    WebServer(/* args */);
    ~WebServer();
    void run();
};