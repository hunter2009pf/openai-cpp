#pragma once

#ifndef __OPEN_AI_JSON_UTIL__
#define __OPEN_AI_JSON_UTIL__

#include <string>
#include <json/json.h>

class JsonUtil {
public:
	static std::string toUnstyledString(Json::Value json);
	static Json::Value fromCStringToJson(char* response);
};

#endif // !__OPEN_AI_JSON_UTIL__
