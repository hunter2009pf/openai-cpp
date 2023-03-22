// openai-cpp.cpp: 定义应用程序的入口点。

#include "openai_service.h"
#include "openai_config.h"
#include "openai_api.h"
#include "openai_constant.h"
#include "json_util.h"
#include <iostream>
#include <fstream>

using namespace std;

string OpenAIService::GetVersion() 
{
	printf("The version of OpenAI C++ SDK is %s\n", OpenAIConfig::SDK_VERSION.c_str());
	return OpenAIConfig::SDK_VERSION;
}

void OpenAIService::InitClient(const string newApiKey, const string newOrgID) 
{
	OpenAIConfig::api_key = newApiKey;
	OpenAIConfig::organization_id = newOrgID;
	/* In windows, this will init the winsock stuff */
	/* use openssl instead of winssl */
	// Initialize OpenSSL library
	//SSL_library_init();
	//SSL_load_error_strings();

	CURLsslset result = curl_global_sslset(CURLSSLBACKEND_OPENSSL, NULL, NULL);
	if (result == CURLsslset::CURLSSLSET_OK) {
		printf("set up openssl successfully\n");
	}
	else if (result == CURLsslset::CURLSSLSET_NO_BACKENDS) {
		printf("set up openssl failed\n");
	}
	else {
		printf("other backends\n");
	}
	curl_global_init(CURL_GLOBAL_ALL);
	if ( defaultHeader != NULL) 
	{
		curl_slist_free_all(defaultHeader); /* free the pointer of default header */
	}
	char auth[128];
	sprintf(auth, OpenAIConfig::AUTHENTICATION_FORMAT.c_str(), OpenAIConfig::api_key.c_str());
	string orgPair = OpenAIConstant::ORGANIZATION + ":" + OpenAIConfig::organization_id;
	defaultHeader = curl_slist_append(defaultHeader, auth);
	defaultHeader = curl_slist_append(defaultHeader, orgPair.c_str());
	//curl_easy_setopt(httpsClient, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_1);
}

void OpenAIService::SetUpProxy(const string proxy) 
{
	OpenAIConfig::proxy = proxy;
}

void OpenAIService::EnableProxy(bool enable)
{
	OpenAIConfig::enableProxy = enable;
}

void OpenAIService::SetUpCAPath(const std::string caPath)
{
	OpenAIConfig::ca_file_path = caPath;
}

void OpenAIService::UseCustomCA(bool enable)
{
	OpenAIConfig::enableCustomCA = enable;
}

vector<OpenAIModel> OpenAIService::ListModels()
{
	/* to be returned result */
	vector<OpenAIModel> modelList{};
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::LIST_MODELS;

		CommonConfig(defaultHeader, requestUrl, 10L, &chunk);
		/* use a GET to fetch this */
		curl_easy_setopt(httpsClient, CURLOPT_HTTPGET, 1L);
		// Set TLS version to 1.2
		//curl_easy_setopt(httpsClient, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;

			if (chunk.response != NULL) {
				Json::Value json;
				Json::CharReaderBuilder builder;
				std::string errors;
				std::istringstream iss(chunk.response);
				bool success = Json::parseFromStream(builder, iss, &json, &errors);
				if (success) {
					Json::Value modelArray = json["data"];
					for (unsigned int i = 0; i < modelArray.size(); i++) {
						OpenAIModel model{ modelArray[i] };
						modelList.push_back(model);
					}
				}
				else {
					std::cerr << "Error parsing JSON: " << errors << std::endl;
				}
			}
		}
		else 
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return modelList;
}

OpenAIModel* OpenAIService::RetrieveModel(const string modelName)
{
	OpenAIModel* model = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		char route[128];
		sprintf(route, OpenAIApi::RETRIEVE_MODEL.c_str(), modelName.c_str());
		string requestUrl = OpenAIConfig::BASE_URL + route;

		CommonConfig(defaultHeader, requestUrl, 10L, &chunk);
		/* use a GET to fetch this */
		curl_easy_setopt(httpsClient, CURLOPT_HTTPGET, 1L);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					model = new OpenAIModel{ json };
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return model;
}

Completion* OpenAIService::CreateCompletion(CreateCompletionData data)
{
	Completion *completion = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_COMPLETION;

		char auth[128];
		sprintf(auth, OpenAIConfig::AUTHENTICATION_FORMAT.c_str(), OpenAIConfig::api_key.c_str());
		string orgPair = OpenAIConstant::ORGANIZATION + ":" + OpenAIConfig::organization_id;
		char contentType[] = "Content-Type: application/json";
		struct curl_slist* customHeader{};
		customHeader = curl_slist_append(customHeader, auth);
		customHeader = curl_slist_append(customHeader, orgPair.c_str());
		customHeader = curl_slist_append(customHeader, contentType);

		CommonConfig(customHeader, requestUrl, 10L, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);
		/* print request and response details */
		curl_easy_setopt(httpsClient, CURLOPT_VERBOSE, 1L);

		// write post data
		Json::Value postData{};
		postData["model"] = data.model;
		postData["prompt"] = data.prompt;
		if (!data.suffix.empty()) {
			postData["suffix"] = data.suffix;
		}
		postData["max_tokens"] = data.max_tokens;
		if (data.temperature != 1.0) {
			postData["temperature"] = data.temperature;
		}
		else if (data.top_p != 1.0) {
			postData["top_p"] = data.top_p;
		}
		else {
			postData["temperature"] = data.temperature;
		}
		postData["n"] = data.n;
		if (data.stream) {
			postData["stream"] = data.stream;
		}
		if (data.logprobs != 0) {
			postData["logprobs"] = data.logprobs;
		}
		if (data.echo) {
			postData["echo"] = data.echo;
		}
		if (!data.stop.empty()) {
			postData["stop"] = data.stop;
		}
		if (data.presence_penalty != 0.0) {
			postData["presence_penalty"] = data.presence_penalty;
		}
		if (data.frequency_penalty != 0.0) {
			postData["frequency_penalty"] = data.frequency_penalty;
		}
		if (data.best_of != 1) {
			postData["best_of"] = data.best_of;
		}
		if (!data.logit_bias.empty()) {
			// Create a JSON object
			Json::Value jsonMap(Json::objectValue);

			// Convert the map to JSON
			for (auto const& [key, val] : data.logit_bias) {
				jsonMap[key] = val;
			}
			postData["logit_bias"] = jsonMap;
		}
		if (!data.user.empty()) {
			postData["user"] = data.user;
		}
		string output = JsonUtil::toUnstyledString(postData);
		curl_easy_setopt(httpsClient, CURLOPT_POSTFIELDS, output.c_str());

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					completion = new Completion(json);
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return completion;
}

/*
{
  "error": {
	"message": "'Ricky' is not one of ['system', 'assistant', 'user'] - 'messages.1.role'",
	"type": "invalid_request_error",
	"param": null,
	"code": null
  }
}
*/
ChatCompletion* OpenAIService::CreateChatCompletion(ChatCompletionData data)
{
	ChatCompletion* chatCompletion = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_CHAT_COMPLETION;

		char auth[128];
		sprintf(auth, OpenAIConfig::AUTHENTICATION_FORMAT.c_str(), OpenAIConfig::api_key.c_str());
		string orgPair = OpenAIConstant::ORGANIZATION + ":" + OpenAIConfig::organization_id;
		char contentType[] = "Content-Type: application/json";
		struct curl_slist* customHeader{};
		customHeader = curl_slist_append(customHeader, auth);
		customHeader = curl_slist_append(customHeader, orgPair.c_str());
		customHeader = curl_slist_append(customHeader, contentType);

		CommonConfig(customHeader, requestUrl, 10L, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		// write post data
		Json::Value postData{};
		postData["model"] = data.model;
		
		if (!data.messages.empty()) {
			Json::Value msgsJsonArr{};
			for (int i = 0; i < data.messages.size(); i++) {
				msgsJsonArr.append(data.messages[i].toJson());
			}
			postData["messages"] = msgsJsonArr;
		}

		if (data.max_tokens > 0) {
			postData["max_tokens"] = data.max_tokens;
		}
		if (data.temperature != 1.0) {
			postData["temperature"] = data.temperature;
		}
		else if (data.top_p != 1.0) {
			postData["top_p"] = data.top_p;
		}
		else {
			postData["temperature"] = data.temperature;
		}
		postData["n"] = data.n;
		if (data.stream) {
			postData["stream"] = data.stream;
		}
		if (!data.stop.empty()) {
			postData["stop"] = data.stop;
		}
		if (data.presence_penalty != 0.0) {
			postData["presence_penalty"] = data.presence_penalty;
		}
		if (data.frequency_penalty != 0.0) {
			postData["frequency_penalty"] = data.frequency_penalty;
		}
		if (!data.logit_bias.empty()) {
			// Create a JSON object
			Json::Value jsonMap(Json::objectValue);

			// Convert the map to JSON
			for (auto const& [key, val] : data.logit_bias) {
				jsonMap[key] = val;
			}
			postData["logit_bias"] = jsonMap;
		}
		if (!data.user.empty()) {
			postData["user"] = data.user;
		}
		string output = JsonUtil::toUnstyledString(postData);
		curl_easy_setopt(httpsClient, CURLOPT_POSTFIELDS, output.c_str());

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					chatCompletion = new ChatCompletion(json);
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return chatCompletion;
}

Edit* OpenAIService::CreateEdit(EditData data)
{
	Edit* edit = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_EDIT;

		char auth[128];
		sprintf(auth, OpenAIConfig::AUTHENTICATION_FORMAT.c_str(), OpenAIConfig::api_key.c_str());
		string orgPair = OpenAIConstant::ORGANIZATION + ":" + OpenAIConfig::organization_id;
		char contentType[] = "Content-Type: application/json";
		struct curl_slist* customHeader{};
		customHeader = curl_slist_append(customHeader, auth);
		customHeader = curl_slist_append(customHeader, orgPair.c_str());
		customHeader = curl_slist_append(customHeader, contentType);

		CommonConfig(customHeader, requestUrl, 10L, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		// write post data
		Json::Value postData{};
		postData["model"] = data.model;

		if (!data.input.empty()) {
			postData["input"] = data.input;
		}
		postData["instruction"] = data.instruction;
		if (data.temperature != 1.0) {
			postData["temperature"] = data.temperature;
		}
		else if (data.top_p != 1.0) {
			postData["top_p"] = data.top_p;
		}
		else {
			postData["temperature"] = data.temperature;
		}
		postData["n"] = data.n;
		string output = JsonUtil::toUnstyledString(postData);
		curl_easy_setopt(httpsClient, CURLOPT_POSTFIELDS, output.c_str());

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					edit = new Edit(json);
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return edit;
}

vector<string> OpenAIService::CreateImage(CreateImageData data)
{
	vector<string> result{};
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_IMAGE;

		char auth[128];
		sprintf(auth, OpenAIConfig::AUTHENTICATION_FORMAT.c_str(), OpenAIConfig::api_key.c_str());
		string orgPair = OpenAIConstant::ORGANIZATION + ":" + OpenAIConfig::organization_id;
		char contentType[] = "Content-Type: application/json";
		struct curl_slist* customHeader{};
		customHeader = curl_slist_append(customHeader, auth);
		customHeader = curl_slist_append(customHeader, orgPair.c_str());
		customHeader = curl_slist_append(customHeader, contentType);

		CommonConfig(customHeader, requestUrl, 10L, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		// write post data
		Json::Value postData{};
		postData["prompt"] = data.prompt;
		postData["n"] = data.n;
		postData["size"] = data.size;
		postData["response_format"] = data.response_format;
		if (!data.user.empty()) {
			postData["user"] = data.user;
		}
		string output = JsonUtil::toUnstyledString(postData);
		curl_easy_setopt(httpsClient, CURLOPT_POSTFIELDS, output.c_str());

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					Json::Value imageDataArr = json["data"];
					for (unsigned int i = 0; i < imageDataArr.size(); i++) {
						result.push_back(imageDataArr[i][data.response_format].asString());
					}
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return result;
}

// Uploaded image must be a PNG and less than 4 MB.
vector<string> OpenAIService::CreateImageEdit(ImageEditData data)
{
	vector<string> result{};
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_IMAGE_EDIT;

		CommonConfig(defaultHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		// Set the form data
		curl_mime* mime;
		curl_mimepart* part;

		// Initialize mime structure
		mime = curl_mime_init(httpsClient);

		// Add "image" to mime structure
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "image");
		curl_mime_filedata(part, data.imagePath.c_str());
		curl_mime_type(part, "application/octet-stream");

		// Add "mask" to mime structure
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "mask");
		curl_mime_filedata(part, data.maskPath.c_str());
		curl_mime_type(part, "application/octet-stream");

		// Add "prompt"/"text" pair to mime structure
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "prompt");
		curl_mime_data(part, data.prompt.c_str(), CURL_ZERO_TERMINATED);

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "n");
		curl_mime_data(part, to_string(data.n).c_str(), CURL_ZERO_TERMINATED);

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "size");
		curl_mime_data(part, data.size.c_str(), CURL_ZERO_TERMINATED);

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "response_format");
		curl_mime_data(part, data.response_format.c_str(), CURL_ZERO_TERMINATED);

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "user");
		curl_mime_data(part, data.user.c_str(), CURL_ZERO_TERMINATED);

		// Attach mime structure to curl handle
		curl_easy_setopt(httpsClient, CURLOPT_MIMEPOST, mime);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					Json::Value imageDataArr = json["data"];
					for (unsigned int i = 0; i < imageDataArr.size(); i++) {
						result.push_back(imageDataArr[i][data.response_format].asString());
					}
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_mime_free(mime);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return result;
}

// Uploaded image must be a PNG and less than 4 MB.
vector<string> OpenAIService::CreateImageVariation(ImageVariationData data)
{
	vector<string> result{};
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_IMAGE_VARIATION;

		CommonConfig(defaultHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		// Set the form data
		curl_mime* mime;
		curl_mimepart* part;

		// Initialize mime structure
		mime = curl_mime_init(httpsClient);

		// Add "image" to mime structure
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "image");
		curl_mime_filedata(part, data.imagePath.c_str());
		curl_mime_type(part, "application/octet-stream");

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "n");
		curl_mime_data(part, to_string(data.n).c_str(), CURL_ZERO_TERMINATED);

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "size");
		curl_mime_data(part, data.size.c_str(), CURL_ZERO_TERMINATED);

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "response_format");
		curl_mime_data(part, data.response_format.c_str(), CURL_ZERO_TERMINATED);

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "user");
		curl_mime_data(part, data.user.c_str(), CURL_ZERO_TERMINATED);

		// Attach mime structure to curl handle
		curl_easy_setopt(httpsClient, CURLOPT_MIMEPOST, mime);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					Json::Value imageDataArr = json["data"];
					for (unsigned int i = 0; i < imageDataArr.size(); i++) {
						result.push_back(imageDataArr[i][data.response_format].asString());
					}
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_mime_free(mime);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return result;
}

Embedding* OpenAIService::CreateEmbeddings(EmbeddingsData data)
{
	Embedding* embeddingPtr = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_EMBEDDINGS;

		char auth[128];
		sprintf(auth, OpenAIConfig::AUTHENTICATION_FORMAT.c_str(), OpenAIConfig::api_key.c_str());
		string orgPair = OpenAIConstant::ORGANIZATION + ":" + OpenAIConfig::organization_id;
		char contentType[] = "Content-Type: application/json";
		struct curl_slist* customHeader{};
		customHeader = curl_slist_append(customHeader, auth);
		customHeader = curl_slist_append(customHeader, orgPair.c_str());
		customHeader = curl_slist_append(customHeader, contentType);

		CommonConfig(customHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		// write post data
		Json::Value postData{};
		postData["model"] = data.model;
		
		// add input(string or array of string)
		if (data.input.size() == 1) {
			postData["input"] = data.input[0];
		}
		else if (data.input.size() > 1) {
			Json::Value inputArr{};
			for (int i = 0; i < data.input.size(); i++) {
				inputArr.append(data.input[i]);
			}
			postData["input"] = inputArr;
		}
		else {
			return embeddingPtr;
		}

		// add user if it is not empty
		if (!data.user.empty()) {
			postData["user"] = data.user;
		}
		string output = JsonUtil::toUnstyledString(postData);
		curl_easy_setopt(httpsClient, CURLOPT_POSTFIELDS, output.c_str());

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					embeddingPtr = new Embedding{ json };
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return embeddingPtr;
}

string OpenAIService::CreateTranscription(TranscriptionData data)
{
	string text = "";
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_TRANSCRIPTION;

		char auth[128];
		sprintf(auth, OpenAIConfig::AUTHENTICATION_FORMAT.c_str(), OpenAIConfig::api_key.c_str());
		string orgPair = OpenAIConstant::ORGANIZATION + ":" + OpenAIConfig::organization_id;
		char contentType[] = "Content-Type: multipart/form-data";
		struct curl_slist* customHeader{};
		customHeader = curl_slist_append(customHeader, auth);
		customHeader = curl_slist_append(customHeader, orgPair.c_str());
		customHeader = curl_slist_append(customHeader, contentType);

		CommonConfig(customHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		// Set the form data
		curl_mime* mime;
		curl_mimepart* part;

		// Initialize mime structure
		mime = curl_mime_init(httpsClient);

		// Add "image" to mime structure
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "file");
		curl_mime_filedata(part, data.filePath.c_str());
		curl_mime_type(part, "application/octet-stream");

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "model");
		curl_mime_data(part, data.model.c_str(), CURL_ZERO_TERMINATED);

		if (!data.prompt.empty()) {
			part = curl_mime_addpart(mime);
			curl_mime_name(part, "prompt");
			curl_mime_data(part, data.prompt.c_str(), CURL_ZERO_TERMINATED);
		}

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "response_format");
		curl_mime_data(part, data.response_format.c_str(), CURL_ZERO_TERMINATED);

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "temperature");
		curl_mime_data(part, to_string(data.temperature).c_str(), CURL_ZERO_TERMINATED);

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "language");
		curl_mime_data(part, data.language.c_str(), CURL_ZERO_TERMINATED);

		// Attach mime structure to curl handle
		curl_easy_setopt(httpsClient, CURLOPT_MIMEPOST, mime);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					// todo: there is a decoding problem when coping with Chinese audio
					text = json["text"].asString();
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_mime_free(mime);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return text;
}

string OpenAIService::CreateTranslation(TranslationData data)
{
	string textInEnglish = "";
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_TRANSLATION;

		char auth[128];
		sprintf(auth, OpenAIConfig::AUTHENTICATION_FORMAT.c_str(), OpenAIConfig::api_key.c_str());
		string orgPair = OpenAIConstant::ORGANIZATION + ":" + OpenAIConfig::organization_id;
		char contentType[] = "Content-Type: multipart/form-data";
		struct curl_slist* customHeader{};
		customHeader = curl_slist_append(customHeader, auth);
		customHeader = curl_slist_append(customHeader, orgPair.c_str());
		customHeader = curl_slist_append(customHeader, contentType);

		CommonConfig(customHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		// Set the form data
		curl_mime* mime;
		curl_mimepart* part;

		// Initialize mime structure
		mime = curl_mime_init(httpsClient);

		// Add "image" to mime structure
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "file");
		curl_mime_filedata(part, data.filePath.c_str());
		curl_mime_type(part, "application/octet-stream");

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "model");
		curl_mime_data(part, data.model.c_str(), CURL_ZERO_TERMINATED);

		if (!data.prompt.empty()) {
			part = curl_mime_addpart(mime);
			curl_mime_name(part, "prompt");
			curl_mime_data(part, data.prompt.c_str(), CURL_ZERO_TERMINATED);
		}

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "response_format");
		curl_mime_data(part, data.response_format.c_str(), CURL_ZERO_TERMINATED);

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "temperature");
		curl_mime_data(part, to_string(data.temperature).c_str(), CURL_ZERO_TERMINATED);

		// Attach mime structure to curl handle
		curl_easy_setopt(httpsClient, CURLOPT_MIMEPOST, mime);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					// todo: there is a decoding problem when coping with Chinese audio
					textInEnglish = json["text"].asString();
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_mime_free(mime);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return textInEnglish;
}

vector<OpenAIFile> OpenAIService::ListFiles()
{
	/* to be returned result */
	vector<OpenAIFile> files{};
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::LIST_UPLOAD_FILES;

		CommonConfig(defaultHeader, requestUrl, 10L, &chunk);
		/* use a GET to fetch this */
		curl_easy_setopt(httpsClient, CURLOPT_HTTPGET, 1L);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			if (chunk.response != NULL) {
				Json::Value json;
				Json::CharReaderBuilder builder;
				std::string errors;
				std::istringstream iss(chunk.response);
				bool success = Json::parseFromStream(builder, iss, &json, &errors);
				if (success) {
					Json::Value fineTuneEventArr = json["data"];
					for (unsigned int i = 0; i < fineTuneEventArr.size(); i++) {
						OpenAIFile file{ fineTuneEventArr[i] };
						files.push_back(file);
					}
				}
				else {
					std::cerr << "Error parsing JSON: " << errors << std::endl;
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return files;
}

OpenAIFile* OpenAIService::UploadFile(UploadFileData data)
{
	/* to be returned result */
	OpenAIFile* filePtr = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::LIST_UPLOAD_FILES;

		CommonConfig(defaultHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a GET to fetch this */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);
		
		// Set the form data
		curl_mime* mime;
		curl_mimepart* part;

		// Initialize mime structure
		mime = curl_mime_init(httpsClient);

		// Add "image" to mime structure
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "file");
		curl_mime_filedata(part, data.filePath.c_str());
		curl_mime_type(part, "application/octet-stream");

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "purpose");
		curl_mime_data(part, data.purpose.c_str(), CURL_ZERO_TERMINATED);

		// Attach mime structure to curl handle
		curl_easy_setopt(httpsClient, CURLOPT_MIMEPOST, mime);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			if (chunk.response != NULL) {
				Json::Value json;
				Json::CharReaderBuilder builder;
				std::string errors;
				std::istringstream iss(chunk.response);
				bool success = Json::parseFromStream(builder, iss, &json, &errors);
				if (success) {
					filePtr = new OpenAIFile(json);
				}
				else {
					std::cerr << "Error parsing JSON: " << errors << std::endl;
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_mime_free(mime);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return filePtr;
}

GeneralDeleteResult* OpenAIService::DeleteFile(GeneralFileData data)
{
	/* to be returned result */
	GeneralDeleteResult* resultPtr = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		char route[512];
		sprintf(route, OpenAIApi::DELETE_RETRIEVE_FILE.c_str(), data.file_id.c_str());
		string requestUrl = OpenAIConfig::BASE_URL + route;

		CommonConfig(defaultHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a DELETE method to request this */
		curl_easy_setopt(httpsClient, CURLOPT_CUSTOMREQUEST, "DELETE");

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			if (chunk.response != NULL) {
				Json::Value json;
				Json::CharReaderBuilder builder;
				std::string errors;
				std::istringstream iss(chunk.response);
				bool success = Json::parseFromStream(builder, iss, &json, &errors);
				if (success) {
					resultPtr = new GeneralDeleteResult(json);
				}
				else {
					std::cerr << "Error parsing JSON: " << errors << std::endl;
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return resultPtr;
}

OpenAIFile* OpenAIService::RetrieveFile(GeneralFileData data)
{
	/* to be returned result */
	OpenAIFile* filePtr = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		char route[512];
		sprintf(route, OpenAIApi::DELETE_RETRIEVE_FILE.c_str(), data.file_id.c_str());
		string requestUrl = OpenAIConfig::BASE_URL + route;

		CommonConfig(defaultHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a GET to fetch this */
		curl_easy_setopt(httpsClient, CURLOPT_HTTPGET, 1L);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			if (chunk.response != NULL) {
				Json::Value json;
				Json::CharReaderBuilder builder;
				std::string errors;
				std::istringstream iss(chunk.response);
				bool success = Json::parseFromStream(builder, iss, &json, &errors);
				if (success) {
					filePtr = new OpenAIFile(json);
				}
				else {
					std::cerr << "Error parsing JSON: " << errors << std::endl;
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return filePtr;
}

void OpenAIService::RetrieveFileContent(RetrieveFileContentData data)
{
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		char route[512];
		sprintf(route, OpenAIApi::RETRIEVE_FILE_CONTENT.c_str(), data.file_id.c_str());
		string requestUrl = OpenAIConfig::BASE_URL + route;

		CommonConfig(defaultHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a GET to fetch this */
		curl_easy_setopt(httpsClient, CURLOPT_HTTPGET, 1L);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			if (chunk.response != NULL) {
				ofstream outfile(data.savedFilePath);

				if (outfile.is_open()) {
					outfile << chunk.response;
					outfile.close();
					cout << "Successfully wrote data into file." << endl;
				}
				else {
					cout << "Unable to open file." << endl;
				}
			}
		}
		else {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
}

/*------------------------------------------------fine-tune start----------------------------------------------------------*/

FineTune* OpenAIService::CreateFineTune(CreateFineTuneData data)
{
	FineTune* fineTunePtr = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_LIST_FINE_TUNE;

		char auth[128];
		sprintf(auth, OpenAIConfig::AUTHENTICATION_FORMAT.c_str(), OpenAIConfig::api_key.c_str());
		string orgPair = OpenAIConstant::ORGANIZATION + ":" + OpenAIConfig::organization_id;
		char contentType[] = "Content-Type: application/json";
		struct curl_slist* customHeader{};
		customHeader = curl_slist_append(customHeader, auth);
		customHeader = curl_slist_append(customHeader, orgPair.c_str());
		customHeader = curl_slist_append(customHeader, contentType);

		CommonConfig(customHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		// write post data
		Json::Value postData{};
		postData["training_file"] = data.training_file;
		if (!data.validation_file.empty()) {
			postData["validation_file"] = data.validation_file;
		}
		postData["model"] = data.model;
		postData["n_epochs"] = data.n_epochs;
		if (data.batch_size != 0) {
			postData["batch_size"] = data.batch_size;
		}
		else if (data.learning_rate_multiplier != 0.0) {
			postData["learning_rate_multiplier"] = data.learning_rate_multiplier;
		}
		postData["prompt_loss_weight"] = data.prompt_loss_weight;
		if (data.compute_classification_metrics) {
			postData["compute_classification_metrics"] = data.compute_classification_metrics;
		}
		if (data.classification_n_classes != 0) {
			postData["classification_n_classes"] = data.classification_n_classes;
		}
		if (!data.classification_positive_class.empty()) {
			postData["classification_positive_class"] = data.classification_positive_class;
		}
		if (!data.classification_betas.empty()) {
			Json::Value betaArr{};
			for (int i = 0; i < data.classification_betas.size(); i++) {
				betaArr.append(data.classification_betas[i]);
			}
			postData["classification_betas"] = betaArr;
		}
		if (!data.suffix.empty()) {
			postData["suffix"] = data.suffix;
		}
		string output = JsonUtil::toUnstyledString(postData);
		curl_easy_setopt(httpsClient, CURLOPT_POSTFIELDS, output.c_str());

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					fineTunePtr = new FineTune(json);
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return fineTunePtr;
}

std::vector<FineTune> OpenAIService::ListFineTunes()
{
	/* to be returned result */
	vector<FineTune> fineTunes{};
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_LIST_FINE_TUNE;

		CommonConfig(defaultHeader, requestUrl, 10L, &chunk);
		/* use a GET to fetch this */
		curl_easy_setopt(httpsClient, CURLOPT_HTTPGET, 1L);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			if (chunk.response != NULL) {
				Json::Value json;
				Json::CharReaderBuilder builder;
				std::string errors;
				std::istringstream iss(chunk.response);
				bool success = Json::parseFromStream(builder, iss, &json, &errors);
				if (success) {
					Json::Value fineTuneEventArr = json["data"];
					for (unsigned int i = 0; i < fineTuneEventArr.size(); i++) {
						FineTune fineTune{ fineTuneEventArr[i] };
						fineTunes.push_back(fineTune);
					}
				}
				else {
					std::cerr << "Error parsing JSON: " << errors << std::endl;
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return fineTunes;
}

FineTune* OpenAIService::RetrieveFineTune(GeneralFineTuneData data)
{
	/* to be returned result */
	FineTune* fineTunePtr = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		char route[512];
		sprintf(route, OpenAIApi::RETRIEVE_FINE_TUNE.c_str(), data.fine_tune_id.c_str());
		string requestUrl = OpenAIConfig::BASE_URL + route;

		CommonConfig(defaultHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a GET to fetch this */
		curl_easy_setopt(httpsClient, CURLOPT_HTTPGET, 1L);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			if (chunk.response != NULL) {
				Json::Value json;
				Json::CharReaderBuilder builder;
				std::string errors;
				std::istringstream iss(chunk.response);
				bool success = Json::parseFromStream(builder, iss, &json, &errors);
				if (success) {
					fineTunePtr = new FineTune(json);
				}
				else {
					std::cerr << "Error parsing JSON: " << errors << std::endl;
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return fineTunePtr;
}

FineTune* OpenAIService::CancelFineTune(GeneralFineTuneData data)
{
	/* to be returned result */
	FineTune* fineTunePtr = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		char route[512];
		sprintf(route, OpenAIApi::CANCEL_FINE_TUNE.c_str(), data.fine_tune_id.c_str());
		string requestUrl = OpenAIConfig::BASE_URL + route;

		CommonConfig(defaultHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a GET to fetch this */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			if (chunk.response != NULL) {
				Json::Value json;
				Json::CharReaderBuilder builder;
				std::string errors;
				std::istringstream iss(chunk.response);
				bool success = Json::parseFromStream(builder, iss, &json, &errors);
				if (success) {
					fineTunePtr = new FineTune(json);
				}
				else {
					std::cerr << "Error parsing JSON: " << errors << std::endl;
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return fineTunePtr;
}

std::vector<FineTuneEvent> OpenAIService::ListFineTuneEvents(ListFineTuneEventsData data)
{
	/* to be returned result */
	vector<FineTuneEvent> fineTuneEvents{};
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		char route[256];
		sprintf(route, OpenAIApi::LIST_FINE_TUNE_EVENTS.c_str(), data.fine_tune_id.c_str());
		string requestUrl = OpenAIConfig::BASE_URL + route;
		if (data.stream) {
			std::string query_param = "?stream=true";
			requestUrl += query_param;
		}

		CommonConfig(defaultHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a GET to fetch this */
		curl_easy_setopt(httpsClient, CURLOPT_HTTPGET, 1L);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;  // 

			if (chunk.response != NULL) {
				Json::Value json;
				Json::CharReaderBuilder builder;
				std::string errors;
				std::istringstream iss(chunk.response);
				bool success = Json::parseFromStream(builder, iss, &json, &errors);
				if (success) {
					Json::Value fineTuneEventArr = json["data"];
					for (unsigned int i = 0; i < fineTuneEventArr.size(); i++) {
						FineTuneEvent e{ fineTuneEventArr[i] };
						fineTuneEvents.push_back(e);
					}
				}
				else {
					std::cerr << "Error parsing JSON: " << errors << std::endl;
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return fineTuneEvents;
}

GeneralDeleteResult* OpenAIService::DeleteFineTuneModel(DeleteFineTuneModelData data)
{
	/* to be returned result */
	GeneralDeleteResult* resultPtr = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		char route[512];
		sprintf(route, OpenAIApi::DELETE_FINE_TUNE_MODEL.c_str(), data.model.c_str());
		string requestUrl = OpenAIConfig::BASE_URL + route;

		CommonConfig(defaultHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a DELETE method to request this */
		curl_easy_setopt(httpsClient, CURLOPT_CUSTOMREQUEST, "DELETE");

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;

			if (chunk.response != NULL) {
				Json::Value json;
				Json::CharReaderBuilder builder;
				std::string errors;
				std::istringstream iss(chunk.response);
				bool success = Json::parseFromStream(builder, iss, &json, &errors);
				if (success) {
					resultPtr = new GeneralDeleteResult(json);
				}
				else {
					std::cerr << "Error parsing JSON: " << errors << std::endl;
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return resultPtr;
}

/*------------------------------------------------fine-tune end------------------------------------------------------------*/

Moderation* OpenAIService::CreateModeration(CreateModerationData data)
{
	Moderation* moderationPtr = nullptr;
	/* get a curl handle */
	httpsClient = curl_easy_init();
	if (httpsClient)
	{
		memory chunk = { 0 };
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		string requestUrl = OpenAIConfig::BASE_URL + OpenAIApi::CREATE_MODERATION;

		char auth[128];
		sprintf(auth, OpenAIConfig::AUTHENTICATION_FORMAT.c_str(), OpenAIConfig::api_key.c_str());
		string orgPair = OpenAIConstant::ORGANIZATION + ":" + OpenAIConfig::organization_id;
		char contentType[] = "Content-Type: application/json";
		struct curl_slist* customHeader{};
		customHeader = curl_slist_append(customHeader, auth);
		customHeader = curl_slist_append(customHeader, orgPair.c_str());
		customHeader = curl_slist_append(customHeader, contentType);

		CommonConfig(customHeader, requestUrl, data.requestTimeOutInSecond, &chunk);
		/* use a POST to create completion */
		curl_easy_setopt(httpsClient, CURLOPT_POST, 1L);

		// write post data
		Json::Value postData{};
		// specify model
		postData["model"] = data.model;

		// add input(string or array of string)
		if (data.input.size() == 1) {
			postData["input"] = data.input[0];
		}
		else if (data.input.size() > 1) {
			Json::Value inputArr{};
			for (int i = 0; i < data.input.size(); i++) {
				inputArr.append(data.input[i]);
			}
			postData["input"] = inputArr;
		}
		else {
			return moderationPtr;
		}

		string output = JsonUtil::toUnstyledString(postData);
		curl_easy_setopt(httpsClient, CURLOPT_POSTFIELDS, output.c_str());

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(httpsClient);
		/* Check for errors */
		if (res == CURLE_OK) {
			long response_code;
			curl_easy_getinfo(httpsClient, CURLINFO_RESPONSE_CODE, &response_code);
			std::cout << "Response code: " << response_code << std::endl;
			std::cout << "Response body: " << std::endl;
			std::cout << chunk.response << std::endl;

			// Parse JSON data from a string
			if (chunk.response != NULL) {
				Json::Value json = JsonUtil::fromCStringToJson(chunk.response);
				if (json != NULL) {
					moderationPtr = new Moderation(json);
				}
			}
		}
		else
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		/* always cleanup */
		free(chunk.response);
		curl_easy_cleanup(httpsClient);
		httpsClient = nullptr;
	}
	return moderationPtr;
}

/*-----------------------------------------------------ONLY FOR INTERNAL USAGE OR TEST----------------------------------------------------------*/
size_t OpenAIService::WriteResponse(void* data, size_t size, size_t nmemb, void* clientp)
{
	size_t realsize = size * nmemb;
	struct memory* mem = (struct memory*)clientp;

	char* ptr = (char*)realloc(mem->response, mem->size + realsize + 1);
	if (ptr == NULL)
		return 0;  /* out of memory! */

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;

	return realsize;
}

void OpenAIService::CommonConfig(struct curl_slist* headerPtr, std::string requestUrl, long timeOutInSecond, memory* chunkPtr)
{
	curl_easy_setopt(httpsClient, CURLOPT_HTTPHEADER, headerPtr);
	curl_easy_setopt(httpsClient, CURLOPT_URL, requestUrl.c_str());
	if (!OpenAIConfig::proxy.empty() && OpenAIConfig::enableProxy) {
		curl_easy_setopt(httpsClient, CURLOPT_PROXY, OpenAIConfig::proxy.c_str());
	}
	curl_easy_setopt(httpsClient, CURLOPT_WRITEFUNCTION, WriteResponse);
	curl_easy_setopt(httpsClient, CURLOPT_WRITEDATA, (void*)chunkPtr);
	if (!OpenAIConfig::ca_file_path.empty() && OpenAIConfig::enableCustomCA) {
		curl_easy_setopt(httpsClient, CURLOPT_CAINFO, OpenAIConfig::ca_file_path.c_str());
	}
	// Set the timeout to 10 seconds
	curl_easy_setopt(httpsClient, CURLOPT_TIMEOUT, timeOutInSecond);
}

