#pragma once

#ifndef __OPENAI_CONFIG__
#define __OPENAI_CONFIG__

#include <string>

class OpenAIConfig {
private:
	OpenAIConfig() {}

public:
	static const std::string SDK_VERSION;
	
	static const std::string BASE_URL;
	
	static const std::string AUTHENTICATION_FORMAT;
	
	static std::string api_key;
	
	static std::string organization_id;

	static std::string proxy; // format: protocol://ip:port, e.g. http://127.0.0.1:7890

	static bool enableProxy;

	static std::string ca_file_path; // absolute path to a CA file, e.g. "E:/openAI_cpp_sdk/openai-cpp/ssl_certificates/cacert.pem"

	static bool enableCustomCA;
};

#endif // !__OPENAI_CONFIG__
