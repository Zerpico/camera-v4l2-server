#include "Upload.h"
#include "utilities.hpp"

using namespace api;

void Upload::uploadFile(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback) const
{
    MultiPartParser fileUpload;
    if (fileUpload.parse(req) != 0 || fileUpload.getFiles().size() != 1)
    {
        callback(Utility::makeFailedResponse("must only be one file"));
        return;
    }

    auto &file = fileUpload.getFiles()[0];
    auto md5 = file.getMd5();
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody("The server has calculated the file's MD5 hash to be " + md5);
    file.save();
    LOG_INFO << "The uploaded file has been saved to the ./uploads "
                "directory";
    callback(resp);
}