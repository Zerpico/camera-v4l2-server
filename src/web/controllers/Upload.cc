#include "Upload.h"
#include "utilities.hpp"
#include <string>
#include <iostream>
#include <filesystem>
#include <codecvt>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

using namespace api;

bool num_str_cmp(const fs::path &x, const fs::path &y)
{
    auto get_number = [](const fs::path &path)
    {
        auto str_num = path.stem().string();

        std::string num_part{};
        for (char c : str_num)
        {
            if (std::isdigit(c))
            {
                num_part += c;
            }
        }

        return !num_part.empty() ? std::stoi(num_part) : 0;
    };

    return get_number(x) < get_number(y);
}

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

    file.save();

    Json::Value ret;
    ret["file_upload"] = true;
    ret["file_md5"] = md5;
    auto resp = HttpResponse::newHttpJsonResponse(ret);

    spdlog::info("Upload file: '{0}'", file.getFileName());
    callback(resp);
}

void Upload::getFiles(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) const
{
    // setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    Json::Value filesJson;
    std::string path = drogon::app().getUploadPath();

    // get files in upload dir
    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
        if (!std::filesystem::is_regular_file(entry.status()))
            continue;

        auto filename = entry.path().filename();
        std::string converted_str = converter.to_bytes(filename.wstring());

        // ret json value
        Json::Value ret;
        ret["file_size"] = entry.file_size();
        ret["file_path"] = converted_str;
        filesJson.append(ret);
    }

    // response
    Json::Value res;
    res["files"] = filesJson;
    auto resp = HttpResponse::newHttpJsonResponse(filesJson);
    callback(resp);
}