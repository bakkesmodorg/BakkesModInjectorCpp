#include "WindowsUtils.h"
#include <shlobj.h>
#include <winerror.h> 
#include <comutil.h> 
#include <iostream>
#include <fstream>
#pragma comment(lib, "comsuppw")
#include <tlhelp32.h>

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

std::string WindowsUtils::GetRocketLeagueDirFromLog()
{
	static std::string LOG_LINE_MATCH = "directory:";
	static std::string windowsPath = GetMyDocumentsFolder();
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
				fs >> logLine;
				return logLine;
			}

		}
	}
	return std::string();
}

bool WindowsUtils::FileExists(std::string path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

void WindowsUtils::OpenFolder(std::string path)
{
	std::string openPath = "explorer " + path;
	system(openPath.c_str());
}

bool WindowsUtils::IsProcessRunning(std::wstring processName)
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
