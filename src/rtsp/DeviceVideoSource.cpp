#include "DeviceVideoSource.h"
#include <GroupsockHelper.hh>

DeviceVideoSource *DeviceVideoSource::createNew(UsageEnvironment &env, const std::shared_ptr<IObserverEvent> dispatcher, const std::shared_ptr<NtDeviceInterface> device)
{
    return new DeviceVideoSource(env, dispatcher, device);
}

DeviceVideoSource::DeviceVideoSource(UsageEnvironment &env, const std::shared_ptr<IObserverEvent> dispatcher, const std::shared_ptr<NtDeviceInterface> device)
    : FramedSource(env), _dispatcher(dispatcher), _device(device)
{
    _packetQueue = new std::queue<std::shared_ptr<PacketData>>();
    // subscribe on new events
    auto listenerEvent = std::make_shared<ListenerEventProcessor>(device->getFd());
    listenerEvent->setOnEventFunc(std::bind(&DeviceVideoSource::OnMessage, this, std::placeholders::_1));
    _listenerEvent = listenerEvent;
    _dispatcher->subscribe(_listenerEvent);

    m_eventTriggerId = envir().taskScheduler().createEventTrigger(DeviceVideoSource::deliverFrameStub);
    // thread_capture = std::thread(&DeviceVideoSource::RunThread, this);
}

DeviceVideoSource::~DeviceVideoSource()
{
    // mStop = 0;
    // if (thread_capture.joinable())
    //     thread_capture.join();

    while (!_packetQueue->empty())
        _packetQueue->pop();

    delete _packetQueue;

    envir().taskScheduler().deleteEventTrigger(m_eventTriggerId);
    // pthread_join(m_thid, NULL);
    // pthread_mutex_destroy(&m_mutex);
    // delete m_device;
}

void DeviceVideoSource::RunThread()
{
    // http://git.vipaks.local/domination/backend/3d-camera/-/blob/feature/work_with_rtspFix/OpenRtsp/OpenRtspLib/Sources/DummyDeviceFramedSource.h
    // mStop = 1;
    // while (mStop)
    // {
    //     std::shared_ptr<PacketData> packet;
    //     _packetQueue->wait_and_pop(packet);
    //     _packetQueue2->push(packet);

    //     while (_packetQueue2->size() >= 10)
    //     {
    //         _packetQueue2->pop();
    //     }

    //     envir().taskScheduler().triggerEvent(m_eventTriggerId, this);
    // }
}

void DeviceVideoSource::OnMessage(std::shared_ptr<BasePacketData> userdata)
{

    auto format = static_cast<PacketData *>(userdata.get())->get_format();

    auto newPacket = std::make_shared<PacketData>(format);
    newPacket->copy(userdata->data(), userdata->size());

    // _packetQueue->push(newPacket);
    _packetQueue->push(newPacket);

    while (_packetQueue->size() >= 10)
    {
        _packetQueue->front().reset();
        _packetQueue->pop();
    }
    envir().taskScheduler().triggerEvent(m_eventTriggerId, this);
}

void DeviceVideoSource::deliverFrameStub(void *clientData)
{
    ((DeviceVideoSource *)clientData)->deliverFrame();
}

void DeviceVideoSource::deliverFrame()
{
    if (!isCurrentlyAwaitingData())
        return;

    // while (_packetQueue->size() >= 10)
    // {
    //     _packetQueue->pop();
    // }

    auto packet = _packetQueue->front();
    _packetQueue->pop();

    fFrameSize = packet->size();
    u_int8_t *newFrameDataStart = packet->data();
    int trancate = 0;
    int readFrameSize = 0;
    if (fFrameSize >= 4 && newFrameDataStart[readFrameSize + 0] == 0 && newFrameDataStart[readFrameSize + 1] == 0 && newFrameDataStart[readFrameSize + 2] == 0 && newFrameDataStart[readFrameSize + 3] == 1)
    {
        trancate = 4;
    }
    else
    {
        if (fFrameSize >= 3 && newFrameDataStart[readFrameSize + 0] == 0 && newFrameDataStart[readFrameSize + 1] == 0 && newFrameDataStart[readFrameSize + 2] == 1)
        {
            trancate = 3;
        }
    }

    gettimeofday(&fPresentationTime, NULL);

    memcpy(fTo, &newFrameDataStart[readFrameSize + trancate], fFrameSize - trancate);

    // Inform the reader that he has data:
    envir().taskScheduler().triggerEvent(m_eventTriggerId, this);

    if (fFrameSize > 0)
    {
        // send Frame to the consumer
        FramedSource::afterGetting(this);
    }
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
