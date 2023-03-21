#pragma once

#ifndef __OPEN_AI_EDIT_DATA__
#define __OPEN_AI_EDIT_DATA__

#include <string>

class EditData {
public:
	std::string model; // required
	std::string input;
	std::string instruction; // required
	int n = 1;
	double temperature = 1.0; // [0.0, 2.0]
	double top_p = 1.0;
};

#endif