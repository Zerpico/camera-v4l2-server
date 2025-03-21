#include "NtChannelManager.h"
// #include "NtFactoryDevice.h"

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
    channels_[newChannel.id] = newChannel;

    // Уведомляем наблюдателей
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
        std::lock_guard<std::mutex> lock(file_readers_mutex_);
        if (auto it = file_readers_.find(channel.id); it != file_readers_.end())
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
            file_readers_[channel.id] = newDevice;

            if (channel.enable)
                newDevice->start();
            else
                newDevice->stop();
        }

        notifyChannelEvent(channel, ChannelEvent::Updated);
        channels_[channel.id] = channel; // Обновляем канал
        return true;
    }

    // handle missing channel
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
            // handle missing channel, for example, log
            return false;
        }
    }

    // Уведомляем наблюдателей
    notifyChannelEvent(channel, ChannelEvent::Removed);
    return true;
}

const std::vector<NtChannel> NtChannelManager::getChannels()
{
    std::lock_guard<std::mutex> lock(channels_mutex_);
    std::vector<NtChannel> chs;
    // заполняем m как-то

    for (auto it = channels_.begin(); it != channels_.end(); ++it)
    {
        chs.push_back(it->second);
    }
    return chs; // Возвращаем копию
}

void NtChannelManager::notifyChannelEvent(const NtChannel &channel, ChannelEvent event)
{
    std::lock_guard<std::mutex> lock(observers_mutex_);
    for (auto observer : observers_)
    {
        observer->onChannelEvent(channel, event);
    }
}
