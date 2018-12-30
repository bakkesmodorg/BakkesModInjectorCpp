#pragma once
#include <string>
#include <windows.h>
class WindowsUtils
{
public:
	WindowsUtils();
	~WindowsUtils();
	static std::string GetMyDocumentsFolder();
	std::string GetRocketLeagueDirFromLog();
	std::string GetRocketLeagueBuildID();
	static bool FileExists(std::string path);
	static bool FileExists(std::wstring path);
	static void OpenFolder(std::string path);
	static void CreateFolder(std::string path);
	static DWORD IsProcessRunning(std::wstring processName);
	static std::wstring StringToWString(const std::string& s);
	static std::string WStringToString(const std::wstring& s);
	static std::wstring GetCurrentExecutablePath();
	static std::wstring GetExecutablePath(std::string process);
	static int DeleteDirectory(std::wstring &refcstrRootDirectory, bool bDeleteSubdirectories = true);
};
