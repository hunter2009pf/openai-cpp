#pragma once

#ifndef __OPENAI_API__
#define __OPENAI_API__

#include <string>

class OpenAIApi {
private:
	OpenAIApi() {}

public:
	static const std::string LIST_MODELS;
	static const std::string RETRIEVE_MODEL;
	static const std::string CREATE_COMPLETION;
	static const std::string CREATE_CHAT_COMPLETION;
	static const std::string CREATE_EDIT;
	static const std::string CREATE_IMAGE;
	static const std::string CREATE_IMAGE_EDIT;
	static const std::string CREATE_IMAGE_VARIATION;
	static const std::string CREATE_EMBEDDINGS;
	static const std::string CREATE_TRANSCRIPTION;
	static const std::string CREATE_TRANSLATION;
	static const std::string LIST_UPLOAD_FILES;
	static const std::string DELETE_RETRIEVE_FILE;
	static const std::string RETRIEVE_FILE_CONTENT;
	static const std::string CREATE_LIST_FINE_TUNE;
	static const std::string RETRIEVE_FINE_TUNE;
	static const std::string CANCEL_FINE_TUNE;
	static const std::string LIST_FINE_TUNE_EVENTS;
	static const std::string DELETE_FINE_TUNE_MODEL;
	static const std::string CREATE_MODERATION;
};

#endif // !__OPENAI_API__

