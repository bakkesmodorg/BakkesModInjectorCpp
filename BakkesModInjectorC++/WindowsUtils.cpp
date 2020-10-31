#include "WindowsUtils.h"
#include <shlobj.h>
#include <winerror.h> 
#include <comutil.h> 
#include <iostream>
#include <fstream>
#pragma comment(lib, "comsuppw")
#include <tlhelp32.h>
#include "stdafx.h"
#include <cctype>
#include <string>
#include <algorithm>
#include <filesystem>
WindowsUtils::WindowsUtils()
{
}


WindowsUtils::~WindowsUtils()
{
}

std::string WindowsUtils::GetMyDocumentsFolder()
{
	LPWSTR wszPath = NULL;
	HRESULT hr;

	hr = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, NULL, &wszPath);

	if (SUCCEEDED(hr)) {
		_bstr_t bstrPath(wszPath);
		std::string strPath((char*)bstrPath);
		return strPath;
	}
	return "";
}

inline std::string trim(const std::string &s)
{
	auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) {return std::isspace(c); });
	auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c) {return std::isspace(c); }).base();
	return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

std::filesystem::path WindowsUtils::GetSteamRocketLeagueDirFromLog()
{
	static std::string LOG_LINE_MATCH = "directory:";
	static std::string windowsPath = GetMyDocumentsFolder();
	static std::filesystem::path cachedPath = "";
	std::string logPath = windowsPath + "\\My Games\\Rocket League\\TAGame\\Logs\\";
	std::string logFile = logPath + "Launch.log";
	if (WindowsUtils::FileExists(logFile))
	{
		std::ifstream fs(logFile);
		std::string logLine;

		while (fs >> logLine)
		{
			if (logLine.substr(0, LOG_LINE_MATCH.size()) == LOG_LINE_MATCH)
			{
				std::getline(fs, logLine); //Actually read till end of line incase there are spaces in the path
				logLine = trim(logLine);
				LOG_LINE(INFO, "Found RL dir from log: " << logLine)

				//Make sure its steam i guess lol
				if (logLine.find("steamapps") != std::string::npos)
				{
					cachedPath = std::filesystem::path(logLine);
					return cachedPath;
				}
			}

		}
	}
	return cachedPath;
}

//bool WindowsUtils::FileExists(std::filesystem::path path)
//{
//	struct stat buffer;
//	return (stat(path.c_str(), &buffer) == 0);
//}

bool WindowsUtils::FileExists(std::filesystem::path path)
{
	return std::filesystem::exists(path);
	//return GetFileAttributesW(path.c_str()) != 0xFFFFFFFF;
}

void WindowsUtils::OpenFolder(std::filesystem::path path)
{
	//std::string openPath = "explorer \"" + path + "\"";
	//system(openPath.c_str());
	//openPath.replace(openPath.begin(), openPath.end(), '/', '\\\\')
	ShellExecute(NULL, L"open", path.wstring().c_str(), NULL, NULL, SW_SHOWDEFAULT);
}

void WindowsUtils::CreateFolder(std::filesystem::path path)
{
	try
	{
		if (std::filesystem::create_directory(path) ||
			ERROR_ALREADY_EXISTS == GetLastError())
		{
			// CopyFile(...)
		}
	} 
	
	catch(...) {}
}

DWORD WindowsUtils::IsProcessRunning(std::wstring processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

uintmax_t WindowsUtils::DeleteDirectory(std::filesystem::path loc)
{
	return std::filesystem::remove_all(loc);
}

std::wstring WindowsUtils::StringToWString(const std::string & s)
{
	std::wstring temp(s.length(), L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

std::string WindowsUtils::WStringToString(const std::wstring & s)
{
	std::string temp(s.length(), ' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

std::wstring WindowsUtils::GetCurrentExecutablePath()
{
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);
	return path;
}

