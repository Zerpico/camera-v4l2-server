#include "Channel.h"

using namespace api;

void Channel::getInfo(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      std::string channelId) const
{
    LOG_DEBUG << "User " << channelId << " get his information";

    // Verify the validity of the token, etc.
    // Read the database or cache to get user information
    Json::Value ret;
    ret["result"] = "ok";
    ret["channel_id"] = channelId;
    auto resp = HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}