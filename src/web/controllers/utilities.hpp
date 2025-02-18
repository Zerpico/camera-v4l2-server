#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

// HttpResponsePtr makeFailedResponse(std::string errorText = "");
// HttpResponsePtr makeNotFoundResponse(std::string errorText = "");

class Utility
{
private:
    Utility();
    ~Utility();

public:
    static HttpResponsePtr makeFailedResponse(std::string errorText = "")
    {
        Json::Value json;
        json["ok"] = false;
        json["error_text"] = errorText;
        auto resp = HttpResponse::newHttpJsonResponse(json);
        resp->setStatusCode(k500InternalServerError);
        return resp;
    }

    static HttpResponsePtr makeNotFoundResponse(std::string errorText = "")
    {
        Json::Value json;
        json["ok"] = false;
        json["error_text"] = errorText;
        auto resp = HttpResponse::newHttpJsonResponse(json);
        resp->setStatusCode(k404NotFound);
        return resp;
    }
};
