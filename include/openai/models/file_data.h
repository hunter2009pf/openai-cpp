#pragma once

#ifndef __OPEN_AI_FILE_DATA__
#define __OPEN_AI_FILE_DATA__

#include <string>
#include "openai_constant.h"

class UploadFileData {
public:
	std::string filePath; // Required
	std::string purpose = "fine-tune";  // Required, default value is "fine-tune"

	long requestTimeOutInSecond = OpenAIConstant::DEFAULT_REQUEST_TIMEOUT;
};

class GeneralFileData {
public:
	std::string file_id; // Required

	long requestTimeOutInSecond = OpenAIConstant::DEFAULT_REQUEST_TIMEOUT;
};

class RetrieveFileContentData : public GeneralFileData {
public:
	std::string savedFilePath; // Required
};

#endif
