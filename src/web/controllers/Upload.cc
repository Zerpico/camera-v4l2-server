#include "Upload.h"
#include "utilities.hpp"
#include <string>
#include <iostream>
#include <filesystem>
#include <spdlog/spdlog.h>

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

    // save file to disk
    std::filesystem::path uploadPath(drogon::app().getUploadPath());
    uploadPath.append("media");
    int ret = file.save(uploadPath.string());
    if (ret != 0)
    {
        auto resp = Utility::makeFailedResponse("error saving file");
        callback(resp);
        return;
    }

    Json::Value jsonRet;
    jsonRet["file_upload"] = true;
    jsonRet["file_md5"] = md5;
    jsonRet["file_name"] = file.getFileName().c_str();
    auto resp = HttpResponse::newHttpJsonResponse(jsonRet);

    spdlog::info("Upload file: '{0}'", file.getFileName());
    callback(resp);
}

void Upload::getFiles(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value filesJson;
    std::filesystem::path filePath(drogon::app().getUploadPath());
    filePath.append("media");

    if (!std::filesystem::is_directory(filePath))
    {
        auto resp = Utility::makeNotFoundResponse("no uploaded files");
        callback(resp);
        return;
    }

    // get files in upload dir
    int fileCount = 0;
    for (const auto &entry : std::filesystem::directory_iterator(filePath))
    {
        if (!std::filesystem::is_regular_file(entry.status()))
            continue;

        auto filename = entry.path().filename();
        std::string converted_str = Utility::convertFromUtf(filename.wstring());

        // ret json value
        Json::Value ret;
        ret["file_size"] = entry.file_size();
        ret["file_path"] = converted_str;
        filesJson.append(ret);
        fileCount++;
    }

    // directory empty
    if (fileCount <= 0)
    {
        auto resp = Utility::makeNotFoundResponse("no uploaded files");
        callback(resp);
        return;
    }

    // response
    Json::Value res;
    res["files"] = filesJson;
    auto resp = HttpResponse::newHttpJsonResponse(filesJson);
    callback(resp);
}