#include "NtMediaChannel.h"

NtMediaChannel::NtMediaChannel(/* args */)
{
}

NtMediaChannel::~NtMediaChannel()
{
}

void NtMediaChannel::AddChannel(std::shared_ptr<NtDeviceInterface> device)
{
    auto id = device->getFd();
    if (id < 0)
        throw std::exception("invalid id of device");
    _channels[id] = device;
}