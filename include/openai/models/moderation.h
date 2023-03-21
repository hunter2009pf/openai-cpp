#pragma once

#ifndef __OPEN_AI_MODERATION__
#define __OPEN_AI_MODERATION__

#include <json/json.h>

class Moderation {
public:
    std::string id;
    std::string model;
    struct Results {
        struct Categories {
            bool hate;
            bool hate_threatening;
            bool self_harm;
            bool sexual;
            bool sexual_minors;
            bool violence;
            bool violence_graphic;
        } categories;
        struct CategoryScores {
            double hate;
            double hate_threatening;
            double self_harm;
            double sexual;
            double sexual_minors;
            double violence;
            double violence_graphic;
        } category_scores;
        bool flagged;
    } results;

    Moderation(const Json::Value& json) {
        id = json["id"].asString();
        model = json["model"].asString();

        const Json::Value& results_json = json["results"][0];
        results.categories.hate = results_json["categories"]["hate"].asBool();
        results.categories.hate_threatening = results_json["categories"]["hate/threatening"].asBool();
        results.categories.self_harm = results_json["categories"]["self-harm"].asBool();
        results.categories.sexual = results_json["categories"]["sexual"].asBool();
        results.categories.sexual_minors = results_json["categories"]["sexual/minors"].asBool();
        results.categories.violence = results_json["categories"]["violence"].asBool();
        results.categories.violence_graphic = results_json["categories"]["violence/graphic"].asBool();

        results.category_scores.hate = results_json["category_scores"]["hate"].asDouble();
        results.category_scores.hate_threatening = results_json["category_scores"]["hate/threatening"].asDouble();
        results.category_scores.self_harm = results_json["category_scores"]["self-harm"].asDouble();
        results.category_scores.sexual = results_json["category_scores"]["sexual"].asDouble();
        results.category_scores.sexual_minors = results_json["category_scores"]["sexual/minors"].asDouble();
        results.category_scores.violence = results_json["category_scores"]["violence"].asDouble();
        results.category_scores.violence_graphic = results_json["category_scores"]["violence/graphic"].asDouble();

        results.flagged = results_json["flagged"].asBool();
    }
};

#endif