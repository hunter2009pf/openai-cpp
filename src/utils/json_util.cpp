#include "json_util.h"
#include <iostream>

std::string JsonUtil::toUnstyledString(Json::Value json)
{
	Json::FastWriter fastWriter;
	std::string output = fastWriter.write(json);
	return output;
}

Json::Value JsonUtil::fromCStringToJson(char* response)
{
	Json::Value json;
	Json::CharReaderBuilder builder;
	std::string errors;
	std::istringstream iss(response);
	bool success = Json::parseFromStream(builder, iss, &json, &errors);
	if (success) {
		return json;
	}
	else {
		std::cerr << "Error parsing JSON: " << errors << std::endl;
		return NULL;
	}
}
