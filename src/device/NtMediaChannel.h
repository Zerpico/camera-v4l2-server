#pragma once
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include "NtDeviceInterface.h"

class NtMediaChannel
{

public:
    NtMediaChannel();
    ~NtMediaChannel();
    NtMediaChannel(const NtMediaChannel &) = delete;
    void AddChannel(std::shared_ptr<NtDeviceInterface> device);
    bool RemoveChannel(int id);

private:
    std::map<int, std::shared_ptr<NtDeviceInterface>> _channels;
};