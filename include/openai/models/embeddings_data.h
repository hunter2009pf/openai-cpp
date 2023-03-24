#pragma once

#ifndef __OPEN_AI_EMBEDDINGS_DATA__
#define __OPEN_AI_EMBEDDINGS_DATA__

#include <string>
#include <vector>
#include "openai_constant.h"

class EmbeddingsData {
public:
	std::string model; // Required
	std::vector<std::string> input; // Required
	std::string user;

	long requestTimeOutInSecond = OpenAIConstant::DEFAULT_REQUEST_TIMEOUT;
};

#endif // !__OPEN_AI_EMBEDDINGS_DATA__

