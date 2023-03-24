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

	/**
	 * @brief Get the version of the client
	 * @return std::string The version of the client
	 * @example std::cout << "The version of the client is " << openaiService.GetVersion() << std::endl;
	*/
	std::string GetVersion();

	/**
	 * @brief Initiate the client
	 * @param newApiKey The API key
	 * @param newOrgID The organization ID
	 * @return void
	*/
	void InitClient(const std::string newApiKey, const std::string newOrgID);

	/**
	 * @brief Set the proxy
	 * @param proxy Proxy formated as: protocol://ip:port e.g. http://127.0.0.1:7890
	 * @return void
	*/
	void SetUpProxy(const std::string proxy);

	void EnableProxy(bool enable);

	/**
	 * @brief Set the CA file path
	 * @param caPath The absolute path to a CA file 
	 * e.g. "E:/openAI_cpp_sdk/openai-cpp/ssl_certificates/cacert.pem"
	*/
	void SetUpCAPath(const std::string caPath);

	void UseCustomCA(bool enable);

	/**
	 * @brief List all models available
	 * @return std::vector<OpenAIModel> A vector of models
	*/
	std::vector<OpenAIModel> ListModels();

	/**
	 * @brief Retrieve a model
	 * @param modelName The name of the model
	 * @return OpenAIModel* A pointer to the mode
	*/
	OpenAIModel* RetrieveModel(const std::string modelName);

    /**
	 * @brief Create a completion and all complitions i.e. text completion, 
	 * code completion are supported. Given a prompt, the model will return one 
	 * or more predicted completions, and can also return the probabilities of 
	 * alternative tokens at each position.
	 * @param data Data input for completion.
	 * @return Completion* A pointer to the completion
	*/
	Completion* CreateCompletion(CreateCompletionData data);

	/**
	 * @brief Create a chat completion. Given a chat conversation, the model will 
	 * return a chat completion response.
	 * @param data Data input for chat completion
	 * @return ChatCompletion* A pointer to the chat completion
	*/
	ChatCompletion* CreateChatCompletion(ChatCompletionData data);

	/**
	 * @brief Create an Edit. Given a prompt or an instruction, the model
	 *  will return an edited version of the prompt. 
	 * @param data Data input for edit
	 * @return Edit* A pointer to the edit
	*/
	Edit* CreateEdit(EditData data);

	/**
	 * @brief Given a prompt and/or an input image, the model will generate a new image.
	 * @param data Data input for image.
	 * 
	 * @return std::vector<std::string> A vector of captions
	*/
	std::vector<std::string> CreateImage(CreateImageData data);

	/**
	 * @brief Creates and image edit while an edited or extended image
	 * is provided, given an original image and a prompt.
	 * @param data An original image and a prompt. Must be a valid PNG file, 
	 * less than 4MB, and square.
	 * @return std::vector<std::string>
	*/
	std::vector<std::string> CreateImageEdit(ImageEditData data);

	/**
	 * @brief Creates a variation of a given image.
	 * @param data An image input. Must be a valid PNG file, less than 4MB, and square.
	 * @return std::vector<std::string>
	*/
	std::vector<std::string> CreateImageVariation(ImageVariationData data);

	/**
	 * @brief Creates an Embeding providing a vector representation of a given input
	 * that can be easily consumed by machine learning models and algorithms. See 
	 * @ref https://platform.openai.com/docs/guides/embeddings.
	 * @param data EmbeddingsData.
	 * @return std::vector<std::string>
	*/
	Embedding* CreateEmbeddings(EmbeddingsData data);

	/**
	 * @brief Transcribes audio into the input language given an audio file.
	 * @param data The input audio file wrapped in TranscriptionData
	 * @return std::string
	*/
	std::string CreateTranscription(TranscriptionData data);

	/**
	 * @brief Translates audio into into English.
	 * @param data The input text wrapped in TranslationData
	 * @return std::string
	*/
	std::string CreateTranslation(TranslationData data);

	/**
	 * @brief Returns a list of files that belong to the user's organization.
	 *  Files are used to upload documents that can be used with features like Fine-tuning.
	 * @return std::vector<OpenAIFile> A vector of files
	*/
	std::vector<OpenAIFile> ListFiles();

    /**
	 * @brief Upload a file that contains document(s) to be used across various endpoints/features.
	 *  Currently, the size of all the files uploaded by one organization can be up to 1 GB. 
	 * Please contact us if you need to increase the storage limit.
	 * @param data The file to be uploaded wrapped in UploadFileData
	 * @return OpenAIFile 
	*/
	OpenAIFile* UploadFile(UploadFileData data);

	/**
	 * @brief Delete a file that belongs to the user's organization.
	 * @param data file_id
	 * @return GeneralDeleteResult 
	*/
	GeneralDeleteResult* DeleteFile(GeneralFileData data);

	/**
	 * @brief Returns information about a specific file.
	 * @param data file_id
	 * @return OpenAIFile 
	*/
	OpenAIFile* RetrieveFile(GeneralFileData data);

	/**
	 * @brief Returns the content of a specific file.
	 * @param data savedFilePath
	*/
	void RetrieveFileContent(RetrieveFileContentData data);

	/**
	 * @brief Creates a job that fine-tunes a specified model from a given dataset. 
	 * Manage fine-tuning jobs to tailor a model to your specific training data.
	 * @param data 
	 * @return std::vector<FineTuneModel> A vector of fine-tuning models. Response includes
	 * details of the enqueued job including job status and the name of the fine-tuned models once complete.
	*/
	FineTune* CreateFineTune(CreateFineTuneData data);

	/**
	 * @brief List your organization's fine-tuning jobs.
	 * @return std::vector<FineTune> A vector of fine-tuning jobs
	*/
	std::vector<FineTune> ListFineTunes();

	/**
	 * @brief Gets info about the fine-tune job.
	 * @param data GeneralFineTuneData
	*/
	FineTune* RetrieveFineTune(GeneralFineTuneData data);

	/**
	 * @brief Immediately cancel a fine-tune job.
	 * @param data GeneralFineTuneData
	*/
	FineTune* CancelFineTune(GeneralFineTuneData data);

	/**
	 * @brief Get fine-grained status updates for a fine-tune job.
	 * @param data ListFineTuneEventsData
	 * @return std::vector<FineTuneModel> 
	*/
	std::vector<FineTuneEvent> ListFineTuneEvents(ListFineTuneEventsData data);

	/**
	 * @brief Delete a fine-tuned model. You must have the Owner role in your organization.
	 * @param data DeleteFineTuneModelData
	 * @return GeneralDeleteResult
	*/
	GeneralDeleteResult* DeleteFineTuneModel(DeleteFineTuneModelData data);

	/**
	 * @brief Given a input text, outputs if the model classifies it as violating OpenAI's 
	 * content policy. @related https://platform.openai.com/docs/guides/moderation
	*/
	Moderation* CreateModeration(CreateModerationData data);

	
	void TestCurl();

	void TestCurl2();
};

// TODO: 在此处引用程序需要的其他标头。
