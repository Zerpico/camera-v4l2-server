#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api
{
  class Channel : public drogon::HttpController<Channel>
  {
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    METHOD_ADD(Channel::getChannels, "/", Get); // path /api/channel/
    METHOD_ADD(Channel::getChannels, "", Get);  // path /api/channel
    METHOD_ADD(Channel::getInfo, "/{id}", Get); // path /api/channel/{id}
    METHOD_LIST_END

    void getInfo(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback,
                 std::string userId) const;

    void getChannels(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback) const;
  };
}