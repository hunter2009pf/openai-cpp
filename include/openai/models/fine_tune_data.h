#pragma once

#ifndef __OPEN_AI_FINE_TUNE_DATA__
#define __OPEN_AI_FINE_TUNE_DATA__

#include <string>
#include <vector>
#include "openai_constant.h"

class CreateFineTuneData {
public:
	std::string training_file; // Required: The ID of an uploaded file that contains training data.
	std::string validation_file;
	std::string model = "curie"; // "ada", "babbage", "curie", "davinci", or a fine-tuned model created after 2022-04-21
	int n_epochs = 4;
	int batch_size;
	double learning_rate_multiplier;
	double prompt_loss_weight = 0.01;
	bool compute_classification_metrics;
	int classification_n_classes;
	std::string classification_positive_class;
	std::vector<double> classification_betas;
	std::string suffix;

	long requestTimeOutInSecond = OpenAIConstant::DEFAULT_REQUEST_TIMEOUT;
};

class GeneralFineTuneData {
public:
	std::string fine_tune_id;

	long requestTimeOutInSecond = OpenAIConstant::DEFAULT_REQUEST_TIMEOUT;;
};

class ListFineTuneEventsData : public GeneralFineTuneData {
public:
	bool stream;
};

class DeleteFineTuneModelData {
public:
	std::string model; // Required

	long requestTimeOutInSecond = OpenAIConstant::DEFAULT_REQUEST_TIMEOUT;;
};

#endif