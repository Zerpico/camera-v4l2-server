#pragma once
#include <memory>
#include <string>
#include "NtDeviceInterface.h"
#include "NtMediaChannels.h"
#include "INtFactoryDevice.h"

class NtFactoryDevice : public INtFactoryDevice
{
public:
    NtFactoryDevice() = default;
    ~NtFactoryDevice() = default;
    NtFactoryDevice(const NtFactoryDevice &) = delete;

public:
    std::shared_ptr<NtDeviceInterface> createNtDevice(const NtChannel &channel);
};
