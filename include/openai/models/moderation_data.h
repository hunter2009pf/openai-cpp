#pragma once

#ifndef __OPEN_AI_MODERATION_DATA__
#define __OPEN_AI_MODERATION_DATA__

#include "openai_constant.h"

class CreateModerationData {
public:
	std::string model = "text-moderation-latest"; // < text-moderation-stable | text-moderation-latest >
	std::vector<std::string> input; // Required

	long requestTimeOutInSecond = OpenAIConstant::DEFAULT_REQUEST_TIMEOUT;
};

#endif // !__OPEN_AI_MODERATION_DATA__

