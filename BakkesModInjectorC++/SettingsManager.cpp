#include "SettingsManager.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

const std::wstring RegisterySettingsManager::REGISTRY_DIR = TEXT("SOFTWARE\\BakkesMod");
const std::wstring RegisterySettingsManager::REGISTRY_DIR_APPPATH = TEXT("SOFTWARE\\BakkesMod\\AppPath");

void RegisterySettingsManager::SaveSetting(std::wstring key, std::wstring setting, std::wstring subKey)
{
	HKEY hKey;
	LONG currentUserOpen = RegOpenKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	if (currentUserOpen != ERROR_SUCCESS)
		return;

	LONG setValueRes = RegSetValueEx(hKey, key.c_str(), 0, REG_SZ, (LPBYTE)setting.c_str(), (setting.size() + 1) * sizeof(wchar_t));
	if (setValueRes != ERROR_SUCCESS)
		return;
	RegCloseKey(hKey);
}

std::wstring RegisterySettingsManager::GetStringSetting(std::wstring key, std::wstring subKey)
{
	HKEY hKey;
	LONG currentUserOpen = RegOpenKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	if (currentUserOpen != ERROR_SUCCESS)
		return std::wstring();

	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	ULONG nError;
	nError = RegQueryValueExW(hKey, key.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	if (ERROR_SUCCESS == nError)
	{
		return szBuffer;
	}
	return std::wstring();
}

void RegisterySettingsManager::SaveSetting(std::wstring key, int setting, std::wstring subKey)
{
	HKEY hKey;
	LONG currentUserOpen = RegOpenKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	if (currentUserOpen != ERROR_SUCCESS)
		return;


	LONG setValueRes = RegSetValueEx(hKey, key.c_str(), 0, REG_DWORD, (const BYTE*)&setting, sizeof(setting));
	if (setValueRes != ERROR_SUCCESS)
		return;
	RegCloseKey(hKey);
}

int RegisterySettingsManager::GetIntSetting(std::wstring key, std::wstring subKey)
{
	HKEY hKey;
	LONG currentUserOpen = RegOpenKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	if (currentUserOpen != ERROR_SUCCESS)
		return 0x00;

	DWORD dwBufferSize(sizeof(DWORD));
	DWORD nResult(0);
	LONG nError = ::RegQueryValueExW(hKey,
		key.c_str(),
		0,
		NULL,
		reinterpret_cast<LPBYTE>(&nResult),
		&dwBufferSize);
	return nResult;
}
