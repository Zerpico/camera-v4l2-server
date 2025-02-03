#include "NtMediaChannel.h"

NtMediaChannel::NtMediaChannel(/* args */)
{
}

NtMediaChannel::~NtMediaChannel()
{
}

void NtMediaChannel::AddChannel(std::shared_ptr<NtDeviceInterface> device)
{
    auto id = _channels.rbegin()->first;
    if (device->getFd() < 0)
        throw std::runtime_error("invalid id of device");
    _channels[id + 1] = device;
}

bool NtMediaChannel::RemoveChannel(int id)
{
    auto channel = _channels.find(id);
    if (channel == _channels.end())
        return false;

    channel->second->~NtDeviceInterface();
    _channels.erase(id);
    return true;
}
