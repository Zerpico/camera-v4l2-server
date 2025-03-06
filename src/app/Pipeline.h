#pragma once
#include "NtMediaChannels.h"

class IPipeline
{
};

class Pipeline : public IPipeline
{

public:
    Pipeline(const std::shared_ptr<INtMediaChannels> &channels);
    ~Pipeline();
    Pipeline(const Pipeline &) = delete;

private:
    void addChannel(const NtChannel &channel);
    void removeChannel(const std::string &channelId);
    void updateChannel(NtChannel &channel);
    void onChangeChannel(ChannelEvent typeEvent, NtChannel channel);
    void updateDevice(NtDeviceInterface *device);

private:
    std::mutex pipelineMutex;
    std::map<std::string, std::shared_ptr<NtDeviceInterface>> channelSources;
};
