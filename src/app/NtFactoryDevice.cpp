#include "NtFactoryDevice.h"

std::shared_ptr<NtDeviceInterface> NtFactoryDevice::createDummyDevice(const DummyVideoDeviceParameters &params)
{
    std::shared_ptr<NtDeviceInterface> device(new NtDummyVideoDevice(params));
    return device;
}
