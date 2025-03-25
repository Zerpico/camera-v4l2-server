#include "NtChannelManager.h"
#include "spdlog/spdlog.h"
#include <condition_variable>
#include <future>

NtChannelManager::NtChannelManager(const std::shared_ptr<INtFactoryDevice> &deviceFactory) : _deviceFactory(deviceFactory)
{
}

NtChannelManager::~NtChannelManager()
{
}

const NtChannel NtChannelManager::addChannel()
{
    std::lock_guard<std::mutex> lock(channels_mutex_);
    NtChannel newChannel;
    newChannel.id = generateUniqueId();
    newChannel.number = getNextChannelNumber();
    channels_[newChannel.id] = newChannel;

    // Уведомляем наблюдателей
    spdlog::info("Channel added (id: {0})", newChannel.id);
    notifyChannelEvent(newChannel, ChannelEvent::Added);
    return newChannel;
}

bool NtChannelManager::updateChannel(const NtChannel &channel)
{
    std::lock_guard<std::mutex> lock(channels_mutex_);
    auto it = channels_.find(channel.id);
    if (it != channels_.end())
    {
        NtChannel oldChannel = it->second;

        // Обновляем или создаем новый NtDeviceInterface
        {
            std::lock_guard<std::mutex> lock(device_readers_mutex_);
            if (auto it = device_readers_.find(channel.id); it != device_readers_.end())
            {
                auto device = it->second;
                device->update((void *)&channel);

                if (channel.enable)
                    device->start();
                else
                    device->stop();
            }
            else
            {
                // Создаем для нового канала
                auto newDevice = _deviceFactory->createNtDevice(channel);
                if (!newDevice)
                {
                    spdlog::error("Unknown Channel type: {0}, on channel: {1}", static_cast<int>(channel.type), channel.id);
                    return false;
                }

                device_readers_[channel.id] = newDevice;

                if (channel.enable)
                    newDevice->start();
                else
                    newDevice->stop();
            }
        }

        spdlog::info("Channel updated (id: {0})", channel.id);
        notifyChannelEvent(channel, ChannelEvent::Updated);
        channels_[channel.id] = channel; // Обновляем канал
        return true;
    }

    // handle missing channel
    spdlog::error("Channel not found (id: {0})", channel.id);
    return false;
}

bool NtChannelManager::removeChannel(const std::string &channelId)
{
    NtChannel channel;
    {
        std::lock_guard<std::mutex> lock(channels_mutex_);
        auto it = channels_.find(channelId);
        if (it != channels_.end())
        {
            channel = it->second;
            channels_.erase(it);
        }
        else
        {
            // handle missing channel
            spdlog::error("Channel not found (id: {0})", channel.id);
            return false;
        }
    }

    // Уведомляем наблюдателей
    spdlog::info("Channel removed (id: {0})", channelId);
    notifyChannelEvent(channel, ChannelEvent::Removed);
    return true;
}

const std::vector<NtChannel> NtChannelManager::getChannels()
{
    std::lock_guard<std::mutex> lock(channels_mutex_);
    std::vector<NtChannel> chs;

    for (auto it = channels_.begin(); it != channels_.end(); ++it)
    {
        chs.push_back(it->second);
    }
    return chs; // Возвращаем копию
}

const std::shared_ptr<NtDeviceInterface> NtChannelManager::getDevice(const std::string &id)
{
    std::lock_guard<std::mutex> lock(device_readers_mutex_);
    if (auto it = device_readers_.find(id); it != device_readers_.end())
    {
        return it->second;
    }
    return nullptr;
};

void NtChannelManager::notifyChannelEvent(const NtChannel &channel, ChannelEvent event)
{
    std::vector<std::shared_ptr<IListenerChannel>> observers_copy;
    {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_copy = observers_; // Create a copy of the observers
    }

    // Asynchronously notify each observer
    for (auto observer : observers_copy)
    {
        std::async(std::launch::async, [observer, channel, event]()
                   { observer->onChannelEvent(channel, event); });
    }
}
