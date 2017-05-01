#pragma once
#include <string>
#include "modp_b64.h"
class MyBase64
{
public:
	MyBase64();
	//º”√‹
	static bool Base64Encode(const std::string& input, std::string* output);
	//Ω‚√‹
	static bool Base64Decode(const std::string& input, std::string* output);
	~MyBase64();
};

