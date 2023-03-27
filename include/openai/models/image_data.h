#pragma once

#ifndef __OPEN_AI_IMAGE_DATA__
#define __OPEN_AI_IMAGE_DATA__

#include <string>
#include "openai_constant.h"

class BaseImageData {
public:
	int n = 1;
	std::string size = "1024x1024"; // Must be one of 256x256, 512x512, or 1024x1024.
	std::string response_format = ImageResponseFormat::URL; // Must be one of url or b64_json.
	std::string user;

	long requestTimeOutInSecond = OpenAIConstant::DEFAULT_REQUEST_TIMEOUT;; // The default request timeout is 10 seconds.
};

class CreateImageData : public BaseImageData {
public:
	std::string prompt; // Required
};

class ImageEditData : public BaseImageData {
public:
	std::string imagePath; // Required
	std::string maskPath;
	std::string prompt; // required: The maximum length is 1000 characters.
};

class ImageVariationData : public BaseImageData {
public:
	std::string imagePath; // Required: Must be a valid PNG file, less than 4MB, and square.
};

#endif // !__OPEN_AI_IMAGE_DATA__
