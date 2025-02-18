#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api
{
  class Channel : public HttpController<Channel>
  {
  private:
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    METHOD_ADD(Channel::getChannels, "", Get);             // get /api/channel
    METHOD_ADD(Channel::newChannel, "/new", Post);         // post /api/channel/
    METHOD_ADD(Channel::getChannelInfo, "/{id}", Get);     // get /api/channel/{id}
    METHOD_ADD(Channel::updateChannelInfo, "/{id}", Post); // post /api/channel/{id}
    METHOD_LIST_END

    // get array for channel info
    void getChannels(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback) const;

    // request to create new empty channel
    void newChannel(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback) const;

    // get detail for channel with id
    void getChannelInfo(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback,
                        std::string channelId) const;

    // request to update channel
    void updateChannelInfo(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback,
                           std::string channelId) const;
  };
}