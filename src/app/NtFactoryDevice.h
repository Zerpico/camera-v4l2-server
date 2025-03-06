#pragma once
#include "NtDeviceInterface.h"
#include <memory>
#include <string>
#include "NtDummyVideoDevice.h"

class NtFactoryDevice
{
private:
    NtFactoryDevice() = default;
    ~NtFactoryDevice() = default;

public:
    static std::shared_ptr<NtDeviceInterface> createDummyDevice(const DummyVideoDeviceParameters &params);
    // static std::shared_ptr<NtDeviceInterface> createFileDevice(std::string filePath);
};
