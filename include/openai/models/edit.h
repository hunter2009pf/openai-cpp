#pragma once

#ifndef __OPEN_AI_EDIT__
#define __OPEN_AI_EDIT__

#include <string>
#include <vector>

class EditChoice {
public:
    std::string text;
    int index;

    EditChoice(Json::Value choice_json) {
        text = choice_json["text"].asString();
        index = choice_json["index"].asInt();
    }
};

class Edit {
public:
    std::string object;
    int created;
    std::vector<EditChoice> choices;
    Usage usage;

    Edit(Json::Value json_data) {
        object = json_data["object"].asString();
        created = json_data["created"].asInt();
        for (auto& choiceJson : json_data["choices"]) {
            EditChoice ec{ choiceJson };
            choices.push_back(ec);
        }
        usage.prompt_tokens = json_data["usage"]["prompt_tokens"].asInt();
        usage.completion_tokens = json_data["usage"]["completion_tokens"].asInt();
        usage.total_tokens = json_data["usage"]["total_tokens"].asInt();
    }
};


#endif // !1
