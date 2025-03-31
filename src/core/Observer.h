#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include "AsyncTaskQueue.h"
#include "DataPackets.h"
#include "string"
#include <thread>

// Интерфейс для наблюдателя (Listener) - получает уведомления
class IListenerEvent
{
public:
    virtual const std::string getRefId() const = 0;
    virtual ~IListenerEvent() = default;
    virtual void onEvent(const std::shared_ptr<BasePacketData> &event) = 0;
};

// Интерфейс для источника (IObserverEvent) - предоставляет события для прослушивания новых пакетов с источников INtDeviceInterface
class IObserverEvent
{
public:
    virtual ~IObserverEvent() = default;
    virtual void subscribe(std::shared_ptr<IListenerEvent> listener) = 0;
    virtual void unsubscribe(std::shared_ptr<IListenerEvent> listener) = 0;
    virtual void publishEvent(std::shared_ptr<BasePacketData> event) = 0;
};

// Конкретный класс Subject (Observable) - Реализация IObserverEvent
class ObserverEventSource : public IObserverEvent
{
public:
    ObserverEventSource() : isRunning(true), workerThread(&ObserverEventSource::worker, this) {}
    ~ObserverEventSource() {}

    void subscribe(std::shared_ptr<IListenerEvent> listener) override
    {
        std::lock_guard<std::mutex> lock(listenersMutex);
        if (listener)
            listeners.push_back(listener);
    }

    void unsubscribe(std::shared_ptr<IListenerEvent> listener) override
    {
        std::lock_guard<std::mutex> lock(listenersMutex);
        listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
    }

    void publishEvent(std::shared_ptr<BasePacketData> event)
    {
        std::vector<std::shared_ptr<IListenerEvent>> currentListeners; // Copy for thread safety
        {
            std::lock_guard<std::mutex> lock(listenersMutex);
            currentListeners = listeners;
        }

        for (const auto &listener : currentListeners)
        {
            if (listener->getRefId() == event->get_refId())
            {
                event->refIncrement();
                taskQueue.enqueue([listener, event]()
                                  {
                try {
                    listener->onEvent(event);
                } catch (const std::exception& e) {
                    std::cerr << "Exception in listener: " << e.what() << std::endl;
                } catch (...) {
                    std::cerr << "Unknown exception in listener" << std::endl;
                } });

                eventsUnused.push(event);
            }
        }

        event.reset();
    }

private:
    void worker()
    {
        std::chrono::milliseconds sleepMs(20);
        while (isRunning)
        {
            while (!eventsUnused.empty())
            {
                auto p = eventsUnused.front();
                if (p == nullptr)
                {
                    eventsUnused.pop();
                    break;
                }
                if (p->contRef())
                    break;
                eventsUnused.front().reset();
                eventsUnused.pop();
            }
            std::this_thread::sleep_for(sleepMs);
        }
    }

private:
    std::vector<std::shared_ptr<IListenerEvent>> listeners;
    std::mutex listenersMutex;
    AsyncTaskQueue taskQueue;
    std::atomic<bool> isRunning;
    std::thread workerThread;
    std::queue<std::shared_ptr<BasePacketData>> eventsUnused{};
};

// Пример конкретного Listener
class ListenerEventProcessor : public IListenerEvent
{
public:
    ListenerEventProcessor(const std::string &name) : name(name) {}

    const std::string getRefId() const
    {
        return name;
    }

    void setOnEventFunc(std::function<void(std::shared_ptr<BasePacketData>)> handler)
    {
        _handler = handler;
    }

    void onEvent(const std::shared_ptr<BasePacketData> &event) override
    {
        if (_handler != nullptr)
            _handler(event);
    }

private:
    std::string name;
    std::function<void(const std::shared_ptr<BasePacketData>)> _handler = NULL;
};