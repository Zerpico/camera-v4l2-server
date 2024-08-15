#pragma once

#include "NtDeviceInterface.h"
#include <string>

struct DummyVideoDeviceParameters
{
    DummyVideoDeviceParameters(const int width = 640, const int height = 480, const double fps = 25.0, const char *title_text = NULL, const bool add_timer = false)
        : m_devName("dummy_video"), m_width(width), m_height(height), m_fps(fps), m_title(title_text), m_add_timer(add_timer)
    {
    }

    std::string m_devName;
    int m_width;
    int m_height;
    double m_fps;
    std::string m_title;
    bool m_add_timer;
};

class NtDummyVideoDevice : public NtDeviceInterface
{

public:
    static NtDummyVideoDevice *createNew(const DummyVideoDeviceParameters &params);
    virtual ~NtDummyVideoDevice();
    void close();

protected:
    NtDummyVideoDevice(const DummyVideoDeviceParameters &params);

public:
    virtual size_t read(char *buffer, size_t bufferSize) { return 0; }
    virtual int getFd() { return -1; }
    virtual unsigned long getBufferSize() { return 0; }

    virtual int getWidth() { return m_params.m_width; }
    virtual int getHeight() { return m_params.m_height; }
    // virtual int getVideoFormat() { return m_device->getFormat(); }

private:
    DummyVideoDeviceParameters m_params;
};
