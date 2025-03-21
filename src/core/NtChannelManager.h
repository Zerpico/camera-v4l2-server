#pragma once
#include <unordered_map>
#include <mutex>
#include "NtMediaChannels.h"
#include "INtFactoryDevice.h"

// Абстрактный класс для наблюдателей (observers)
class IListenerChannel
{
public:
    virtual void onChannelEvent(const NtChannel &channel, ChannelEvent event) = 0;
    virtual ~IListenerChannel() = default;
};

// Базовый интерфейс для управления каналами
class INtChannelManager
{
public:
    virtual const NtChannel addChannel() = 0;
    virtual bool updateChannel(const NtChannel &newChannel) = 0;
    virtual bool removeChannel(const std::string &id) = 0;

    virtual const std::vector<NtChannel> getChannels() = 0;
    virtual void subscribe(std::shared_ptr<IListenerChannel> observer) = 0;
};

// Класс для управления каналами
class NtChannelManager : public INtChannelManager
{
public:
    NtChannelManager(const std::shared_ptr<INtFactoryDevice> &deviceFactory);
    ~NtChannelManager();

    const NtChannel addChannel();
    bool updateChannel(const NtChannel &channel);
    bool removeChannel(const std::string &channelId);

    // Подписаться на события
    void subscribe(std::shared_ptr<IListenerChannel> observer)
    {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_.push_back(observer);
    }

    // Отписаться от событий
    void unsubscribe(std::shared_ptr<IListenerChannel> observer)
    {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
    }

    const std::vector<NtChannel> getChannels();

private:
    std::unordered_map<std::string, NtChannel> channels_;
    std::unordered_map<std::string, std::shared_ptr<NtDeviceInterface>> file_readers_;
    std::vector<std::shared_ptr<IListenerChannel>> observers_; // Observer list

    std::mutex channels_mutex_;
    std::mutex file_readers_mutex_;
    std::mutex observers_mutex_;
    std::shared_ptr<INtFactoryDevice> _deviceFactory;

    // Уведомить о событии канала
    void notifyChannelEvent(const NtChannel &channel, ChannelEvent event);

    std::string generateUniqueId()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> distrib(0, 255);

        std::stringstream ss;
        for (int i = 0; i < 16; ++i)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << distrib(gen);
        }
        return ss.str();
    }
};
