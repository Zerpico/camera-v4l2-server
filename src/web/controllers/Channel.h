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
    METHOD_ADD(Channel::getInfo, "/{1}", Get);
    METHOD_LIST_END

    void getInfo(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback,
                 std::string userId) const;
  };
}