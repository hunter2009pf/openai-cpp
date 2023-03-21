#pragma once

#ifndef __OPEN_AI_MODEL__
#define __OPEN_AI_MODEL__

#include <string>
#include <vector>
#include <ctime>
#include <json/json.h>

class OpenAIModel {
public:
    std::string id;
    std::string object;
    std::time_t created;
    std::string owned_by;

    struct Permission {
        std::string id;
        std::string object;
        std::time_t created;
        bool allow_create_engine;
        bool allow_sampling;
        bool allow_logprobs;
        bool allow_search_indices;
        bool allow_view;
        bool allow_fine_tuning;
        std::string organization;
        std::string group;
        bool is_blocking;
    };

    std::vector<Permission> permission;
    std::string root;
    std::string parent;

    // Constructor to initialize object from JSON object
    OpenAIModel(Json::Value& json) {
        id = json["id"].asString();
        object = json["object"].asString();
        created = json["created"].asUInt64();
        owned_by = json["owned_by"].asString();
        root = json["root"].asString();
        parent = json["parent"].asString();

        // Parse permission array
        Json::Value permissions = json["permission"];
        for (Json::ArrayIndex i = 0; i < permissions.size(); ++i) {
            Permission perm;
            perm.id = permissions[i]["id"].asString();
            perm.object = permissions[i]["object"].asString();
            perm.created = permissions[i]["created"].asUInt64();
            perm.allow_create_engine = permissions[i]["allow_create_engine"].asBool();
            perm.allow_sampling = permissions[i]["allow_sampling"].asBool();
            perm.allow_logprobs = permissions[i]["allow_logprobs"].asBool();
            perm.allow_search_indices = permissions[i]["allow_search_indices"].asBool();
            perm.allow_view = permissions[i]["allow_view"].asBool();
            perm.allow_fine_tuning = permissions[i]["allow_fine_tuning"].asBool();
            perm.organization = permissions[i]["organization"].asString();
            perm.group = permissions[i]["group"].asString();
            perm.is_blocking = permissions[i]["is_blocking"].asBool();
            permission.push_back(perm);
        }
    }
};

#endif // !__OPEN_AI_MODEL__

