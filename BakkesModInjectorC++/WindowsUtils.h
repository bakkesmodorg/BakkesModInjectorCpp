#pragma once
#include <string>

class WindowsUtils
{
public:
	WindowsUtils();
	~WindowsUtils();
	static std::string GetMyDocumentsFolder();
	std::string GetRocketLeagueDirFromLog();
	std::string GetRocketLeagueBuildID();
	static bool FileExists(std::string path);
	static void OpenFolder(std::string path);
	static bool IsProcessRunning(std::wstring processName);
	static std::wstring StringToWString(const std::string& s);
	static std::string WStringToString(const std::wstring& s);
	static std::wstring GetCurrentExecutablePath();
};

