#include "openai_config.h"

const std::string OpenAIConfig::SDK_VERSION = "V1.0";

const std::string OpenAIConfig::BASE_URL = "https://api.openai.com";

const std::string OpenAIConfig::AUTHENTICATION_FORMAT = "Authorization: Bearer %s";

std::string OpenAIConfig::api_key = "";

std::string OpenAIConfig::organization_id = "";

std::string OpenAIConfig::proxy = "";

bool OpenAIConfig::enableProxy = false;

std::string OpenAIConfig::ca_file_path = "";

bool OpenAIConfig::enableCustomCA = false;