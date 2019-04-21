#pragma once
#include <map>
#include <memory>
#include <windows.h>

class RegisterySettingsManager
{
public:
	const static std::wstring REGISTRY_DIR;
	const static std::wstring REGISTRY_DIR_APPPATH;
	const static std::wstring REGISTRY_DIR_RUN;
private:
	HKEY GetKey(std::wstring subKey);
public:

	void SaveSetting(std::wstring key, std::wstring setting, std::wstring subKey = REGISTRY_DIR);
	std::wstring GetStringSetting(std::wstring key, std::wstring subKey = REGISTRY_DIR);
	void SaveSetting(std::wstring key, int setting, std::wstring subKey = REGISTRY_DIR);
	int GetIntSetting(std::wstring key, std::wstring subKey = REGISTRY_DIR);
	void DeleteSetting(std::wstring key, std::wstring subKey = REGISTRY_DIR);
};
