#include "WindowsUtils.h"
#include <shlobj.h>
#include <winerror.h> 
#include <comutil.h> 
#include <iostream>
#include <fstream>
#pragma comment(lib, "comsuppw")
#include <tlhelp32.h>
#include "stdafx.h"
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
				std::getline(fs, logLine); //Actually read till end of line incase there are spaces in the path
				LOG_LINE(INFO, "Found RL dir from log: " << logLine)
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
	//std::string openPath = "explorer \"" + path + "\"";
	//system(openPath.c_str());
	//openPath.replace(openPath.begin(), openPath.end(), '/', '\\\\')
	ShellExecute(NULL, L"open", StringToWString(path).c_str(), NULL, NULL, SW_SHOWDEFAULT);
}

void WindowsUtils::CreateFolder(std::string path)
{
	if (CreateDirectory(StringToWString(path).c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		// CopyFile(...)
	}
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

int WindowsUtils::DeleteDirectory(std::wstring &refcstrRootDirectory, bool bDeleteSubdirectories)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
												 // subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	std::wstring     strFilePath;                 // Filepath
	std::wstring     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information


	strPattern = refcstrRootDirectory + std::wstring(L"\\*.*");
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FileInformation.cFileName[0] != '.')
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + std::wstring(L"\\") + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if (iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if (::SetFileAttributes(strFilePath.c_str(),
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();

					// Delete file
					if (::DeleteFile(strFilePath.c_str()) == FALSE)
						return ::GetLastError();
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			if (!bSubdirectory)
			{
				// Set directory attributes
				if (::SetFileAttributes(refcstrRootDirectory.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

				// Delete directory
				if (::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
					return ::GetLastError();
			}
		}
	}

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

std::wstring WindowsUtils::GetCurrentExecutablePath()
{
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);
	return path;
}
