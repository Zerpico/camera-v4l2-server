#include "NtMediaChannels.h"
#include <spdlog/spdlog.h>

// Генератор уникальных ID
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

NtMediaChannels::NtMediaChannels(/* args */)
{
}

NtMediaChannels::~NtMediaChannels()
{
}

NtChannel NtMediaChannels::addChannel()
{
    std::lock_guard<std::mutex> lock(mutex);
    NtChannel newChannel;
    newChannel.id = generateUniqueId();
    channels.push_back(newChannel);
    notify(ChannelEvent::Added, newChannel);
    return newChannel;
}

bool NtMediaChannels::removeChannel(const std::string &id)
{
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::remove_if(channels.begin(), channels.end(),
                             [&](const NtChannel &c)
                             { return c.id == id; });
    if (it != channels.end())
    {
        NtChannel removedChannel = *it; // Сохраняем удаленный канал для уведомления
        channels.erase(it, channels.end());
        notify(ChannelEvent::Removed, removedChannel);
        return true; // Канал был удален
    }
    return false; // Канал не найден
}

bool NtMediaChannels::updateChannel(const NtChannel &newChannel)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (auto &channel : channels)
    {
        if (channel.id == newChannel.id)
        {
            NtChannel oldChannel = channel;
            channel = newChannel; // Обновляем канал
            notify(ChannelEvent::Updated, newChannel);
            return true;
        }
    }
    // Если канал не найден
    spdlog::error("MediaChannel not found '{0}'", newChannel.id);
    return false;
}

const std::vector<NtChannel> NtMediaChannels::getChannels()
{
    std::lock_guard<std::mutex> lock(mutex);
    return channels; // Возвращаем копию
}