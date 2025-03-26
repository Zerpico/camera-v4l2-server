#include "DeviceVideoSource.h"

DeviceVideoSource *DeviceVideoSource::createNew(UsageEnvironment &env, const std::shared_ptr<IObserverEvent> dispatcher, const std::shared_ptr<NtDeviceInterface> device)
{
    return new DeviceVideoSource(env, dispatcher, device);
}

DeviceVideoSource::DeviceVideoSource(UsageEnvironment &env, const std::shared_ptr<IObserverEvent> dispatcher, const std::shared_ptr<NtDeviceInterface> device)
    : FramedSource(env), _dispatcher(dispatcher), _device(device)
{
    // subscribe on new events
    auto listenerEvent = std::make_shared<ListenerEventProcessor>("DeviceVideoSource_" + device->getFd());
    listenerEvent->setOnEventFunc(std::bind(&DeviceVideoSource::OnMessage, this, std::placeholders::_1));
    _listenerEvent = listenerEvent;
    _dispatcher->subscribe(_listenerEvent);

    m_eventTriggerId = envir().taskScheduler().createEventTrigger(DeviceVideoSource::deliverFrameStub);
}

DeviceVideoSource::~DeviceVideoSource()
{
    envir().taskScheduler().deleteEventTrigger(m_eventTriggerId);
    // pthread_join(m_thid, NULL);
    // pthread_mutex_destroy(&m_mutex);
    // delete m_device;
}

void DeviceVideoSource::OnMessage(std::shared_ptr<BasePacketData> userdata)
{
    // PacketData *value = static_cast<PacketData *>(userdata);
    // spdlog::info("OnMessage called, size: {} , from SubscriberId {}", userdata->size(), _listener->GetSubscriberId());
    // delete value;
}

void DeviceVideoSource::deliverFrameStub(void *clientData)
{
    ((DeviceVideoSource *)clientData)->deliverFrame();
}

void DeviceVideoSource::deliverFrame()
{
}

void DeviceVideoSource::incomingPacketHandlerStub(void *clientData, int mask)
{
    ((DeviceVideoSource *)clientData)->incomingPacketHandler();
}

void DeviceVideoSource::incomingPacketHandler()
{
    if (this->getNextFrame() <= 0)
    {
        handleClosure(this);
    }
}

int DeviceVideoSource::getNextFrame()
{
    return 0;
}

void DeviceVideoSource::doGetNextFrame()
{
}
