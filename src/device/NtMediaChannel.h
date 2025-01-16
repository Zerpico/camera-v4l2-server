#pragma once
#include <map>
#include "NtDeviceInterface.h"
#include <memory>

class NtMediaChannel
{

public:
    NtMediaChannel();
    ~NtMediaChannel();
    NtMediaChannel(const NtMediaChannel &) = delete;
    void AddChannel(std::shared_ptr<NtDeviceInterface> device);

private:
    std::map<__int64, std::shared_ptr<NtDeviceInterface>> _channels;
};