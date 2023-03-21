// openai-cpp.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#define CURL_STATICLIB
#define _CRT_SECURE_NO_WARNINGS

//#define CPPHTTPLIB_OPENSSL_SUPPORT

//#include "httplib.h"

#include <string>
#include <vector>
#include <curl/curl.h>
#include "openai_model.h"
#include "completion_data.h"
#include "edit_data.h"
#include "image_data.h"
#include "embeddings_data.h"
#include "transcription_data.h"
#include "file_data.h"
#include "moderation_data.h"
#include "fine_tune_data.h"
#include "completion.h"
#include "edit.h"
#include "embedding.h"
#include "openai_file.h"
#include "moderation.h"
#include "fine_tune.h"

typedef struct memory {
	char* response;
	size_t size;
} memory;

class OpenAIService {
private:
	// HTTPS
	CURL* httpsClient = nullptr;
	struct curl_slist* defaultHeader = nullptr;
	CURLcode res;

	static size_t WriteResponse(void* data, size_t size, size_t nmemb, void* clientp);
	
	void CommonConfig(struct curl_slist* headerPtr, std::string requestUrl, long timeOutInSecond, memory* chunkPtr);

public:
	std::string GetVersion();

	void InitClient(const std::string newApiKey, const std::string newOrgID);

	void SetUpProxy(const std::string proxy);

	void EnableProxy(bool enable);

	void SetUpCAPath(const std::string caPath);

	void UseCustomCA(bool enable);

	std::vector<OpenAIModel> ListModels();

	OpenAIModel* RetrieveModel(const std::string modelName);

	Completion* CreateCompletion(CreateCompletionData data);

	ChatCompletion* CreateChatCompletion(ChatCompletionData data);

	Edit* CreateEdit(EditData data);

	std::vector<std::string> CreateImage(CreateImageData data);

	std::vector<std::string> CreateImageEdit(ImageEditData data);

	std::vector<std::string> CreateImageVariation(ImageVariationData data);

	Embedding* CreateEmbeddings(EmbeddingsData data);

	std::string CreateTranscription(TranscriptionData data);

	std::string CreateTranslation(TranslationData data);

	std::vector<OpenAIFile> ListFiles();

	OpenAIFile* UploadFile(UploadFileData data);

	GeneralDeleteResult* DeleteFile(GeneralFileData data);

	OpenAIFile* RetrieveFile(GeneralFileData data);

	void RetrieveFileContent(RetrieveFileContentData data);

	FineTune* CreateFineTune(CreateFineTuneData data);

	std::vector<FineTune> ListFineTunes();

	FineTune* RetrieveFineTune(GeneralFineTuneData data);

	FineTune* CancelFineTune(GeneralFineTuneData data);

	std::vector<FineTuneEvent> ListFineTuneEvents(ListFineTuneEventsData data);

	GeneralDeleteResult* DeleteFineTuneModel(DeleteFineTuneModelData data);

	Moderation* CreateModeration(CreateModerationData data);

	void TestCurl();

	void TestCurl2();
};

// TODO: 在此处引用程序需要的其他标头。
