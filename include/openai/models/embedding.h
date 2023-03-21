#pragma once

#ifndef __OPEN_AI_EMBEDDING__
#define __OPEN_AI_EMBEDDING__

#include <string>
#include <vector>
#include <json/json.h>
#include "completion.h"

class EmbeddingData {
public:
    std::string object;
    int index;
    std::vector<double> embedding;
};

class Embedding {
public:
    std::string object;
    std::vector<EmbeddingData> data;
    std::string model;
    Usage usage;

    Embedding(Json::Value json_data)
    {
        // Convert JSON to Embedding object
        object = json_data["object"].asString();
        model = json_data["model"].asString();
        usage.prompt_tokens = json_data["usage"]["prompt_tokens"].asInt();
        usage.total_tokens = json_data["usage"]["total_tokens"].asInt();

        const auto& dataInReponse = json_data["data"];
        for (unsigned int i = 0; i < dataInReponse.size(); i++) {
            EmbeddingData embeddingData;
            embeddingData.object = dataInReponse[i]["object"].asString();
            embeddingData.index = dataInReponse[i]["index"].asInt();

            const auto& embeddingArr = dataInReponse[i]["embedding"];
            for (unsigned int j = 0; j < embeddingArr.size(); j++) {
                embeddingData.embedding.push_back(embeddingArr[j].asDouble());
            }

            data.push_back(embeddingData);
        }
    }
};

#endif // !__OPEN_AI_EMBEDDING__
