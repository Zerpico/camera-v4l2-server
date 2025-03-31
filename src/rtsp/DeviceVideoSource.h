#pragma once
#include <liveMedia.hh>
#include "NtDeviceInterface.h"
#include <thread>
#include <Observer.h>
#include "ThreadsafeQueue.h"
#include "DataPackets.h"

class DeviceVideoSource : public FramedSource
{
protected:
    DeviceVideoSource(UsageEnvironment &env, const std::shared_ptr<IObserverEvent> dispatcher, const std::shared_ptr<NtDeviceInterface> device);
    virtual ~DeviceVideoSource();
    static void deliverFrameStub(void *clientData);
    void deliverFrame();
    static void incomingPacketHandlerStub(void *clientData, int mask);
    void incomingPacketHandler();
    int getNextFrame();
    // overide FramedSource
    virtual void doGetNextFrame();

public:
    static DeviceVideoSource *createNew(UsageEnvironment &env, const std::shared_ptr<IObserverEvent> dispatcher, const std::shared_ptr<NtDeviceInterface> device);
    static EventTriggerId eventTriggerId;
    std::string getAuxLine() { return m_auxLine; }
    const std::shared_ptr<NtDeviceInterface> getDevice() { return _device; }

private:
    void OnMessage(std::shared_ptr<BasePacketData> userdata);
    void RunThread();

private:
    EventTriggerId m_eventTriggerId;
    static unsigned referenceCount;
    // std::list<Frame *> m_captureQueue;
    timeval currentTime;
    const std::shared_ptr<IObserverEvent> _dispatcher;
    const std::shared_ptr<NtDeviceInterface> _device;
    // std::shared_ptr<NalParser> _nalParser;
    std::shared_ptr<IListenerEvent> _listenerEvent;
    int mStop = 1;
    bool mm = false;
    std::thread thread_capture{};
    // NoThrowMutex *queue_mutex;
    std::string m_sps;
    std::string m_pps;
    std::string m_auxLine;
    bool m_keepMarker;
    std::queue<std::shared_ptr<PacketData>> *_packetQueue;
};
