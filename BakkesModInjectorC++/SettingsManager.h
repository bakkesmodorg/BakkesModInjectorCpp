#pragma once
#include <map>

class SettingsManager
{
private:
	std::map<std::string, std::string> options;
public:
	SettingsManager();
	~SettingsManager();
};

