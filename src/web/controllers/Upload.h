#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api
{
    class Upload : public HttpController<Upload>
    {
    public:
        METHOD_LIST_BEGIN
        // // use METHOD_ADD to add your custom processing function here;
        METHOD_ADD(Upload::uploadFile, "", Post); // post /api/channel/{id}
        METHOD_LIST_END

        void uploadFile(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback) const;
    };
}