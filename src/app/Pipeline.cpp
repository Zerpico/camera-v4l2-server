#include "Pipeline.h"
#include <functional>
#include <spdlog/spdlog.h>
#include "NtDummyVideoDevice.h"
#include "NtFactoryDevice.h"

Pipeline::Pipeline(const std::shared_ptr<INtMediaChannels> &channels)
{
    // channels->subscribe(std::bind(&Pipeline::onChangeChannel, this, std::placeholders::_1));
    //  callback = std::bind(&foo, "test", std::placeholders::_1, 3.f);
    channels->subscribe([this](ChannelEvent event, NtChannel channel)
                        { this->onChangeChannel(event, channel); });
}

Pipeline::~Pipeline()
{
}

void Pipeline::onChangeChannel(ChannelEvent typeEvent, NtChannel channel)
{
    switch (typeEvent)
    {
    case ChannelEvent::Added:
        addChannel(channel);
        break;
    case ChannelEvent::Removed:
        removeChannel(channel.id);
        break;
    case ChannelEvent::Updated:
        updateChannel(channel);
        break;
    default:
        spdlog::error("Unknown ChannelEvent: {0}, on channel: {1}", static_cast<int>(typeEvent), channel.id);
        break;
    }
}

void Pipeline::addChannel(const NtChannel &channel)
{
    std::lock_guard<std::mutex> lock(pipelineMutex);
    // Создаем канал
    // NtDeviceFactory::createNew(channel);
    // auto reader = std::make_unique<FileSourceReader>();
    std::unique_ptr<NtDeviceInterface> source;

    // Сохраняем канал
    channelSources[channel.id] = std::move(source);
    spdlog::info("Channel added (id: {0})", channel.id);
}

void Pipeline::removeChannel(const std::string &channelId)
{
    std::lock_guard<std::mutex> lock(pipelineMutex);
    // Проверяем, существует ли канал
    if (channelSources.find(channelId) == channelSources.end())
    {
        spdlog::error("Channel not found (id: {0})", channelId);
        return;
    }

    // Останавливаем канал
    channelSources[channelId]->stop();
    // Удаляем из словаря
    channelSources.erase(channelId);

    spdlog::info("Channel removed (id: {0})", channelId);
}

void Pipeline::updateChannel(NtChannel &channel)
{
    std::lock_guard<std::mutex> lock(pipelineMutex);
    // Проверяем, существует ли канал
    if (channelSources.find(channel.id) == channelSources.end())
    {
        spdlog::error("Channel not found (id: {0})", channel.id);
        return;
    }

    switch (channel.type)
    {
    case ChannelSourceType::Dummy:
        // addChannel(channel);
        break;

    default:
        spdlog::error("Unknown Channel type: {0}, on channel: {1}", static_cast<int>(channel.type), channel.id);
        return;
    }

    // Обновляем параметры канала
    auto device = channelSources[channel.id];
    if (!device)
    {
        device = NtFactoryDevice::createDummyDevice(DummyVideoDeviceParameters::Create(channel.metadata));
    }

    // channelSources[channel.id]->setChannel(channel);
    spdlog::info("Channel updated (id: {0})", channel.id);
}

void Pipeline::updateDevice(NtDeviceInterface *device)
{
}