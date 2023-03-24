#pragma once

#ifndef __OPENAI_CONSTANT__
#define __OPENAI_CONSTANT__

#include <string>

class OpenAIConstant {
private:
	OpenAIConstant() {}

public:
	static const std::string AUTHERIZATION;
	static const std::string ORGANIZATION;
	static const long DEFAULT_REQUEST_TIMEOUT;
};

class ImageResponseFormat {
private:
	ImageResponseFormat() {}

public:
	static const std::string URL;
	static const std::string B64_JSON;
};

#endif // !__OPENAI_CONSTANT__
