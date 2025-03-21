#pragma once

class INtFactoryDevice
{
public:
    virtual std::shared_ptr<NtDeviceInterface> createNtDevice(const NtChannel &channel) = 0;
};