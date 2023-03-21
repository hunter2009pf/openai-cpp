#pragma once

#ifndef __OPEN_AI_CREATE_COMPLETION_DATA__
#define __OPEN_AI_CREATE_COMPLETION_DATA__

#include <string>
#include <map>

class BaseCompletionData {
public:
	std::string model; // required
	int max_tokens; // Most models have a context length of 2048 tokens (except for the newest models, which support 4096).
	double temperature;
	double top_p;
	int n;
	bool stream;
	std::string stop;
	double presence_penalty;
	double frequency_penalty;
	std::map<std::string, int> logit_bias;
	std::string user;
};

class CreateCompletionData : public BaseCompletionData {
public:
	std::string prompt;
	std::string suffix;
	int logprobs;
	bool echo;
	int best_of;

	CreateCompletionData()
	{
		max_tokens = 16;
		temperature = 1.0;
		top_p = 1.0;
		n = 1;
		stream = false;
		echo = false;
		presence_penalty = 0.0;
		frequency_penalty = 0.0;
		best_of = 1;
	}
};

class Message {
public:
	std::string role; // only three choices currently: ['system', 'assistant', 'user']
	std::string content;

	Json::Value toJson()
	{
		Json::Value json;
		json["role"] = role;
		json["content"] = content;
		return json;
	}
};

class ChatCompletionData : public BaseCompletionData {
public:
	std::vector<Message> messages; // required

	ChatCompletionData()
	{
		temperature = 1.0;
		top_p = 1.0;
		n = 1;
		stream = false;
		max_tokens = 0;
		presence_penalty = 0.0;
		frequency_penalty = 0.0;
	}
};

#endif