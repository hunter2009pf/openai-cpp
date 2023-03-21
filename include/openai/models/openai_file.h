#pragma once

#ifndef __OPEN_AI_FILE__
#define __OPEN_AI_FILE__

#include <string>
#include <json/json.h>

class OpenAIFile {
public:
    std::string id;
    std::string object;
    unsigned int bytes;
    int created_at;
    std::string filename;
    std::string purpose;

    // Default constructor
    OpenAIFile(const Json::Value &json_data) 
    {
        id = json_data["id"].asString();
        object = json_data["object"].asString();
        bytes = json_data["bytes"].asUInt();
        created_at = json_data["created_at"].asInt();
        filename = json_data["filename"].asString();
        purpose = json_data["purpose"].asString();

    }
};

class GeneralDeleteResult {
public:
    std::string id;
    std::string object;
    bool deleted;

    GeneralDeleteResult(const Json::Value& json) {
        id = json["id"].asString();
        object = json["object"].asString();
        deleted = json["deleted"].asBool();
    }

    Json::Value to_json() const {
        Json::Value json;
        json["id"] = id;
        json["object"] = object;
        json["deleted"] = deleted;
        return json;
    }
};


#endif