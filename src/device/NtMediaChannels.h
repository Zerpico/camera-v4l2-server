#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <memory>
#include "NtDeviceInterface.h"
#include <random>
#include <iomanip>
#include <mutex>
#include <functional>

// Перечисление для типов событий
enum class ChannelEvent
{
    Added,
    Removed,
    Updated
};

struct NtChannel
{
    std::string id;     // Уникальный ID канала
    std::string source; // Путь к файлу
    bool enable = false;
};

class INtMediaChannels
{
public:
    virtual NtChannel addChannel() = 0;
    virtual bool removeChannel(const std::string &id) = 0;
    virtual bool updateChannel(const NtChannel &newChannel) = 0;
    virtual const std::vector<NtChannel> getChannels() = 0;
    virtual void subscribe(std::function<void(ChannelEvent, NtChannel)> callback) = 0;
};

class NtMediaChannels : public INtMediaChannels
{

public:
    NtMediaChannels();
    ~NtMediaChannels();
    // NtMediaChannels(const NtMediaChannels &input) = default;
    NtChannel addChannel();
    bool removeChannel(const std::string &id);
    bool updateChannel(const NtChannel &newChannel);
    const std::vector<NtChannel> getChannels();

    void subscribe(std::function<void(ChannelEvent, NtChannel)> callback)
    {
        callbacks.push_back(callback);
    }

    INtMediaChannels *m_input;

private:
    std::map<int, std::shared_ptr<NtDeviceInterface>> _channels;
    std::vector<NtChannel> channels;
    std::mutex mutex;
    std::vector<std::function<void(ChannelEvent, NtChannel)>> callbacks;

    void notify(ChannelEvent typeEvent, NtChannel channel)
    {
        for (auto &callback : callbacks)
        {
            callback(typeEvent, channel);
        }
    }
};