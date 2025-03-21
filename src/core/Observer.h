#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include "AsyncTaskQueue.h"
#include "DataPackets.h"

// Интерфейс для наблюдателя (Listener) - получает уведомления
class Listener
{
public:
    virtual ~Listener() = default;
    virtual void onEvent(const std::shared_ptr<BasePacketData> &event) = 0;
};

// Интерфейс для источника (Observer) - предоставляет события для прослушивания
class Observer
{
public:
    virtual ~Observer() = default;
    virtual void subscribe(std::shared_ptr<Listener> listener) = 0;
    virtual void unsubscribe(std::shared_ptr<Listener> listener) = 0;
    virtual void publishEvent(const std::shared_ptr<BasePacketData> &event) = 0;
};

// Конкретный класс Subject (Observable) - Реализация Observer
class EventSource : public Observer
{
public:
    EventSource() {}
    ~EventSource() {}

    void subscribe(std::shared_ptr<Listener> listener) override
    {
        std::lock_guard<std::mutex> lock(listenersMutex);
        listeners.push_back(listener);
    }

    void unsubscribe(std::shared_ptr<Listener> listener) override
    {
        std::lock_guard<std::mutex> lock(listenersMutex);
        listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
    }

    void publishEvent(const std::shared_ptr<BasePacketData> &event)
    {
        std::vector<std::shared_ptr<Listener>> currentListeners; // Copy for thread safety
        {
            std::lock_guard<std::mutex> lock(listenersMutex);
            currentListeners = listeners;
        }

        for (const auto &listener : currentListeners)
        {
            taskQueue.enqueue([listener, event]()
                              {
                try {
                    listener->onEvent(event);
                } catch (const std::exception& e) {
                    std::cerr << "Exception in listener: " << e.what() << std::endl;
                } catch (...) {
                    std::cerr << "Unknown exception in listener" << std::endl;
                } });
        }
    }

private:
    std::vector<std::shared_ptr<Listener>> listeners;
    std::mutex listenersMutex;
    AsyncTaskQueue taskQueue;
};

// Пример конкретного Listener
class DataProcessor : public Listener
{
public:
    DataProcessor(const std::string &name) : name(name) {}

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

// Пример "генератора" событий
class EventGenerator
{
public:
    EventGenerator(std::shared_ptr<EventSource> eventSource) : eventSource(eventSource), running(true) {}

    ~EventGenerator()
    {
        running = false;
        if (eventThread.joinable())
        {
            eventThread.join();
        }
    }

    void startGeneratingEvents()
    {
        eventThread = std::thread([this]()
                                  {
            int i = 0;
            while (running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                auto event = std::make_shared<PacketData>();                
                eventSource->publishEvent(event);
            } });
    }

private:
    std::shared_ptr<EventSource> eventSource;
    std::atomic<bool> running;
    std::thread eventThread;
};
