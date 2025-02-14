#include "Channel.h"
#include <NtMediaChannels.h>
#include "mioc/mioc.h"
#include <vector>
#include <algorithm>
using namespace api;

void Channel::getInfo(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      std::string channelId) const
{
    auto mediaChannels = getContainer()->Resolve<INtMediaChannels>();
    auto channels = mediaChannels->getChannels();
    auto result = std::find_if(begin(channels), end(channels), [&](const NtChannel &val)
                               { return val.id == channelId; });

    if (result == end(channels))
    {
        auto resp = HttpResponse::newNotFoundResponse();
        callback(resp);
    }
    else
    {
        auto findChannel = *result;
        Json::Value ret;
        ret["channel_id"] = findChannel.id;
        ret["channel_source"] = findChannel.source;
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
}

void Channel::getChannels(const HttpRequestPtr &req,
                          std::function<void(const HttpResponsePtr &)> &&callback) const
{
    auto mediaChannels = getContainer()->Resolve<INtMediaChannels>();
    auto channels = mediaChannels->getChannels();

    if (channels.empty())
    {
        auto resp = HttpResponse::newNotFoundResponse();
        callback(resp);
    }
    else
    {
        Json::Value channelJson;
        for (auto channel : channels)
        {
            Json::Value ret;
            ret["channel_id"] = channel.id;
            ret["channel_source"] = channel.source;
            channelJson.append(ret);
        }
        Json::Value res;
        res["channels"] = channelJson;
        auto resp = HttpResponse::newHttpJsonResponse(channelJson);
        callback(resp);
    }
}