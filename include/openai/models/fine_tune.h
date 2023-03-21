#pragma once

#ifndef __OPEN_AI_FINE_TUNE__
#define __OPEN_AI_FINE_TUNE__

#include <vector>
#include <string>
#include <json/json.h>
#include "openai_file.h"

class Hyperparams {
public:
    int batch_size;
    float learning_rate_multiplier;
    int n_epochs;
    float prompt_loss_weight;
};

class FineTuneEvent {
public:
    std::string object;
    int created_at;
    std::string level;
    std::string message;

    FineTuneEvent(const Json::Value& json_data)
    {
        object = json_data["object"].asString();
        created_at = json_data["created_at"].asInt();
        level = json_data["level"].asString();
        message = json_data["message"].asString();
    }

    ~FineTuneEvent() {}
};

class FineTune {
public:
	std::string id;
	std::string object;
	std::string model;
	int created_at;
	std::vector<FineTuneEvent> events;
	std::string fine_tuned_model;
	Hyperparams hyperparams;
	std::string organization_id;
	std::vector<OpenAIFile> result_files;
	std::string status;
	std::vector<OpenAIFile> validation_files;
	std::vector<OpenAIFile> training_files;
	int updated_at;

	FineTune(const Json::Value &json_data)
	{
        id = json_data["id"].asString();
        object = json_data["object"].asString();
        model = json_data["model"].asString();
        created_at = json_data["created_at"].asInt();
        fine_tuned_model = json_data["fine_tuned_model"].asString();
        organization_id = json_data["organization_id"].asString();
        status = json_data["status"].asString();
        updated_at = json_data["updated_at"].asInt();

        for (const auto& event : json_data["events"]) {
            FineTuneEvent e{ event };
            events.push_back(e);
        }

        Hyperparams hp;
        hp.batch_size = json_data["hyperparams"]["batch_size"].asInt();
        hp.learning_rate_multiplier = json_data["hyperparams"]["learning_rate_multiplier"].asFloat();
        hp.n_epochs = json_data["hyperparams"]["n_epochs"].asInt();
        hp.prompt_loss_weight = json_data["hyperparams"]["prompt_loss_weight"].asFloat();
        hyperparams = hp;

        for (const auto& file : json_data["result_files"]) {
            OpenAIFile f{file};
            result_files.push_back(f);
        }

        for (const auto& file : json_data["validation_files"]) {
            OpenAIFile f{file};
            validation_files.push_back(f);
        }

        for (const auto& file : json_data["training_files"]) {
            OpenAIFile f{ file };
            training_files.push_back(f);
        }
	}
};

#endif // !__OPEN_AI_FINE_TUNE__

