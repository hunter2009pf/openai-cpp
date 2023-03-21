#pragma once

#ifndef __OPEN_AI_FILE_DATA__
#define __OPEN_AI_FILE_DATA__

#include <string>

class UploadFileData {
public:
	std::string filePath; // Required
	std::string purpose = "fine-tune";  // Required, default value is "fine-tune"

	long requestTimeOutInSecond = 10L;
};

class GeneralFileData {
public:
	std::string file_id; // Required

	long requestTimeOutInSecond = 10L;
};

class RetrieveFileContentData : public GeneralFileData {
public:
	std::string savedFilePath; // Required
};

#endif
