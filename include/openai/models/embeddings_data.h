#pragma once

#ifndef __OPEN_AI_EMBEDDINGS_DATA__
#define __OPEN_AI_EMBEDDINGS_DATA__

#include <string>
#include <vector>

class EmbeddingsData {
public:
	std::string model; // Required
	std::vector<std::string> input; // Required
	std::string user;

	long requestTimeOutInSecond = 10L;
};

#endif // !__OPEN_AI_EMBEDDINGS_DATA__

