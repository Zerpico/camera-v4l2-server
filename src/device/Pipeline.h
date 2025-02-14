#pragma once
#include "NtMediaChannels.h"

class Pipeline
{

public:
    Pipeline(std::shared_ptr<NtMediaChannels> channels);
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
