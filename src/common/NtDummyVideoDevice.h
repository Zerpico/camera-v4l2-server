#pragma once

#include "NtDeviceInterface.h"

class NtDummyVideoDevice : public NtDeviceInterface
{
private:
    /* data */
public:
    NtDummyVideoDevice(/* args */);
    ~NtDummyVideoDevice();
};
