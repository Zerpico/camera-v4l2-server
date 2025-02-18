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
    void updateChannel(const NtChannel &channel);
    void onChangeChannel(ChannelEvent typeEvent, NtChannel channel);
    std::mutex pipelineMutex;

    std::map<std::string, std::unique_ptr<NtDeviceInterface>> channelSources;
};
