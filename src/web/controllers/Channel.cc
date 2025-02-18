#include "Channel.h"
#include <NtMediaChannels.h>
#include "mioc/mioc.h"
#include "utility.h"

using namespace api;

void Channel::getChannels(const HttpRequestPtr &req,
                          std::function<void(const HttpResponsePtr &)> &&callback) const
{
    auto mediaChannels = getContainer()->Resolve<INtMediaChannels>();
    auto channels = mediaChannels->getChannels();

    if (channels.empty())
    {
        callback(makeNotFoundResponse());
    }
    else
    {
        Json::Value channelJson;
        for (auto channel : channels)
        {
            Json::Value ret;
            ret["channel_id"] = channel.id;
            ret["channel_source"] = channel.source;
            ret["channel_enable"] = channel.enable;
            channelJson.append(ret);
        }
        Json::Value res;
        res["channels"] = channelJson;
        auto resp = HttpResponse::newHttpJsonResponse(channelJson);
        callback(resp);
    }
}

void Channel::newChannel(const HttpRequestPtr &req,
                         std::function<void(const HttpResponsePtr &)> &&callback) const
{
    auto mediaChannels = getContainer()->Resolve<INtMediaChannels>();
    auto newChannel = mediaChannels->addChannel();
    Json::Value ret;
    ret["channel_id"] = newChannel.id;
    auto resp = HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}

void Channel::getChannelInfo(const HttpRequestPtr &req,
                             std::function<void(const HttpResponsePtr &)> &&callback,
                             std::string channelId) const
{
    auto mediaChannels = getContainer()->Resolve<INtMediaChannels>();
    auto channels = mediaChannels->getChannels();
    auto result = std::find_if(begin(channels), end(channels), [&](const NtChannel &val)
                               { return val.id == channelId; });

    if (result == end(channels))
    {
        callback(makeNotFoundResponse("channel with id '" + channelId + "' not found"));
    }
    else
    {
        auto findChannel = *result;
        Json::Value ret;
        ret["channel_id"] = findChannel.id;
        ret["channel_source"] = findChannel.source;
        ret["channel_enable"] = findChannel.enable;
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
}

void Channel::updateChannelInfo(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback,
                                std::string channelId) const
{
    auto jsonPtr = req->getJsonObject();
    if (jsonPtr == nullptr)
    {
        callback(makeFailedResponse(req->getJsonError()));
        return;
    }

    auto mediaChannels = getContainer()->Resolve<INtMediaChannels>();
    auto channels = mediaChannels->getChannels();
    auto result = std::find_if(begin(channels), end(channels), [&](const NtChannel &val)
                               { return val.id == channelId; });

    if (result == end(channels))
    {
        callback(makeNotFoundResponse("channel with id '" + channelId + "' not found"));
        return;
    }

    auto body = *jsonPtr.get();
    std::string channel_source = body["channel_source"].asString();
    bool channel_enable = body["channel_enable"].asBool();
    NtChannel upChannel{};
    upChannel.id = channelId;
    upChannel.source = channel_source;
    upChannel.enable = channel_enable;

    auto resp = mediaChannels->updateChannel(upChannel) ? HttpResponse::newHttpResponse()
                                                        : makeFailedResponse("error when trying to update channel");
    callback(resp);
}

HttpResponsePtr makeFailedResponse(std::string errorText)
{
    Json::Value json;
    json["ok"] = false;
    json["error_text"] = errorText;
    auto resp = HttpResponse::newHttpJsonResponse(json);
    resp->setStatusCode(k500InternalServerError);
    return resp;
}
HttpResponsePtr makeNotFoundResponse(std::string errorText)
{
    Json::Value json;
    json["ok"] = false;
    json["error_text"] = errorText;
    auto resp = HttpResponse::newHttpJsonResponse(json);
    resp->setStatusCode(k404NotFound);
    return resp;
}