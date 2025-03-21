#include "NtFactoryDevice.h"
#include "NtDummyVideoDevice.h"

std::shared_ptr<NtDeviceInterface> NtFactoryDevice::createNtDevice(const NtChannel &channel)
{
    if (channel.type == ChannelSourceType::Dummy)
    {
        auto metadata = channel.metadata;
        auto deviceParam = DummyVideoDeviceParameters::Create(channel.id, metadata);
        return std::make_shared<NtDummyVideoDevice>(deviceParam);
    }
    // ;
    return NULL;
}
