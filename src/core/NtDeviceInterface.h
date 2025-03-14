#pragma once
#include <list>
#include "NtDeviceFormat.h"
#include <cstdint>
#include <cstddef>
#include <string>

// ---------------------------------
// Device Interface
// ---------------------------------
class NtDeviceInterface
{
public:
    // virtual ~NtDeviceInterface() {};
    // for read
    virtual std::string getFd() = 0;
    //  for video
    virtual int getWidth() { return -1; }
    virtual int getHeight() { return -1; }
    virtual NtDeviceFormat getVideoFormat() { return NtDeviceFormat::FMT_NONE; }
    // for audio
    virtual int getSampleRate() { return -1; }
    virtual int getChannels() { return -1; }
    virtual NtDeviceFormat getAudioFormat() { return NtDeviceFormat::FMT_NONE; }

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool update(void *userData) = 0;
};
