#include "NtFactoryDevice.h"
#include "NtDummyVideoDevice.h"
#include "NtVideoFileDevice.h"

std::shared_ptr<NtDeviceInterface> NtFactoryDevice::createNtDevice(const NtChannel &channel)
{
    if (channel.type == ChannelSourceType::Dummy)
    {
        auto metadata = channel.metadata;
        auto deviceParam = DummyVideoDeviceParameters::Create(channel.id, metadata);
        return std::make_shared<NtDummyVideoDevice>(deviceParam);
    }
    else if (channel.type == ChannelSourceType::File)
    {
        auto metadata = channel.metadata;
        auto deviceParam = VideoFileParameters::Create(channel);
        return std::make_shared<NtVideoFileDevice>(deviceParam);
    }
    // ;
    return NULL;
}
