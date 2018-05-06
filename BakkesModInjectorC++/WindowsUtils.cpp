#include "WindowsUtils.h"
#include <shlobj.h>
#include <winerror.h> 
#include <comutil.h> 
#include <iostream>
#include <fstream>
#pragma comment(lib, "comsuppw")


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
