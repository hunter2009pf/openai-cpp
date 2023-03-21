#pragma once

#ifndef __OPEN_AI_COMPLETION__
#define __OPEN_AI_COMPLETION__

#include <string>
#include <vector>
#include <json/json.h>

class Choice {
public:
    std::string text;
    int index;
    std::vector<int> logprobs;
    std::string finish_reason;
};

class Usage {
public:
    int prompt_tokens;
    int completion_tokens;
    int total_tokens;
};

class Completion {
public:
    std::string id;
    std::string object;
    long long created;
    std::string model;
    std::vector<Choice> choices;
    Usage usage;

    Completion()
    {
        
    }

    ~Completion()
    {
        
    }

    Completion(Json::Value json_data)
    {
        id = json_data["id"].asString();
        object = json_data["object"].asString();
        created = json_data["created"].asInt64();
        model = json_data["model"].asString();

        Json::Value choicesArray = json_data["choices"];
        for (Json::ArrayIndex i = 0; i < choicesArray.size(); ++i) {
            Choice choice;
            choice.text = choicesArray[i]["text"].asString();
            choice.index = choicesArray[i]["index"].asInt();

            Json::Value logprobs = choicesArray[i]["logprobs"];
            if (!logprobs.isNull()) {
                for (Json::ArrayIndex j = 0; j < logprobs.size(); ++j) {
                    choice.logprobs.push_back(logprobs[j].asInt());
                }
            }

            choice.finish_reason = choicesArray[i]["finish_reason"].asString();
            choices.push_back(choice);
        }

        Json::Value usageJson = json_data["usage"];
        usage.prompt_tokens = usageJson["prompt_tokens"].asInt();
        usage.completion_tokens = usageJson["completion_tokens"].asInt();
        usage.total_tokens = usageJson["total_tokens"].asInt();
    }
};

class ChatChoice {
public:
    Message message;
    std::string finish_reason;
    int index;
};

class ChatCompletion {
public:
    std::string id;
    std::string object;
    std::time_t created;
    std::string model;
    Usage usage;
    std::vector<ChatChoice> choices;

    ChatCompletion(Json::Value json_data)
    {
        id = json_data["id"].asString();
        object = json_data["object"].asString();
        created = json_data["created"].asInt64();
        model = json_data["model"].asString();
        usage.prompt_tokens = json_data["usage"]["prompt_tokens"].asInt();
        usage.completion_tokens = json_data["usage"]["completion_tokens"].asInt();
        usage.total_tokens = json_data["usage"]["total_tokens"].asInt();
        Json::Value choicesJsonArr = json_data["choices"];
        for (auto& choiceJson : choicesJsonArr) {
            ChatChoice c;
            c.message.role = choiceJson["message"]["role"].asString();
            c.message.content = choiceJson["message"]["content"].asString();
            c.finish_reason = choiceJson["finish_reason"].asString();
            c.index = choiceJson["index"].asInt();
            choices.push_back(c);
        }
    }
};


#endif // !__OPEN_AI_COMPLETION__
