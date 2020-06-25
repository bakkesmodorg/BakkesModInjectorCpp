#include "DllInjector.h"
#include "stdafx.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include "windowsutils.h"

DllInjector::DllInjector()
{
	injectionParameters = new InjectionParameters();


}


DllInjector::~DllInjector()
{
}

InjectionParameters * DllInjector::GetInjectionParameters()
{
	return injectionParameters;
}
static HANDLE hMapObject;
static void* vMapData;
void DllInjector::SetInjectionParameters(InjectionParameters ip)
{
	if (!hMapObject) {
		hMapObject = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(InjectionParameters), L"BakkesModInjectionParameters");
		if (hMapObject == NULL) {
			return;
		}

		vMapData = MapViewOfFile(hMapObject, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(InjectionParameters));
		if (vMapData == NULL) {
			CloseHandle(hMapObject);
			return;
		}
		//vMapData = injectionParameters;
		//UnmapViewOfFile(vMapData);
	}
	memcpy(vMapData, &ip, sizeof(InjectionParameters));
}


DWORD DllInjector::InjectDLL(std::wstring processName, std::string path)
{
	DWORD processID = GetProcessID64(processName);
	if (processID == 0)
		return NOT_RUNNING;

	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
	if (h)
	{
		LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
		LPVOID dereercomp = VirtualAllocEx(h, NULL, path.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(h, dereercomp, path.c_str(), path.size(), NULL);
		HANDLE asdc = CreateRemoteThread(h, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, dereercomp, 0, NULL);
		WaitForSingleObject(asdc, INFINITE);
		VirtualFreeEx(h, dereercomp, path.size(), MEM_RELEASE);
		CloseHandle(asdc);
		CloseHandle(h);
		return OK;
	}
	return NOPE;
}

DWORD DllInjector::GetProcessID64(std::wstring processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (_wcsicmp(processName.c_str(), processInfo.szExeFile) == 0)
	{
		
		BOOL iswow64 = FALSE;
		//https://stackoverflow.com/questions/14184137/how-can-i-determine-whether-a-process-is-32-or-64-bit
		//If IsWow64Process() reports true, the process is 32-bit running on a 64-bit OS
		//So we want it to return false (32 bit on 32 bit os, or 64 bit on 64 bit OS, since we build x64 the first condition will never satisfy since they can't run this exe)
		
		auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processInfo.th32ProcessID);
		if (hProcess == NULL)
		{
			LOG_LINE(INFO, "Error on OpenProcess to check bitness");
		}
		else
		{

			if (IsWow64Process(hProcess, &iswow64))
			{
				//LOG_LINE(INFO, "Rocket league process ID is " << processInfo.th32ProcessID << " | " << " has the WOW factor: " << iswow64);
				if (!iswow64)
				{
					CloseHandle(processesSnapshot);
					return processInfo.th32ProcessID;
				}
			}
			else
			{
				LOG_LINE(INFO, "IsWow64Process failed bruv " << GetLastError());
			}
			CloseHandle(hProcess);
		}
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (_wcsicmp(processName.c_str(), processInfo.szExeFile) == 0)
		{
			BOOL iswow64 = FALSE;
			auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processInfo.th32ProcessID);
			if (hProcess == NULL)
			{
				LOG_LINE(INFO, "Error on OpenProcess to check bitness");
			}
			else
			{

				if (IsWow64Process(hProcess, &iswow64))
				{
					//LOG_LINE(INFO, "Rocket league process ID is " << processInfo.th32ProcessID << " | " << " has the WOW factor: " << iswow64);
					if (!iswow64)
					{
						CloseHandle(processesSnapshot);
						return processInfo.th32ProcessID;
					}
				}
				else
				{
					LOG_LINE(INFO, "IsWow64Process failed bruv " << GetLastError());
				}
				CloseHandle(hProcess);
			}
		}
		//CloseHandle(processesSnapshot);
	}

	CloseHandle(processesSnapshot);
	return 0;

}

DWORD DllInjector::IsBakkesModDllInjected(std::wstring processName)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;
	DWORD processID = GetProcessID64(processName);
	if (processID == 0)
		return NOT_RUNNING;

	printf("\nProcess ID: %u\n", processID);

	// Get a handle to the process.

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return 1;

	// Get a list of all the modules in this process.

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.

			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				std::wstring dllName = std::wstring(szModName);
				if (dllName.find(L"bakkesmod.dll") != std::string::npos)
				{
					return OK;
				}
			}
		}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);
	return NOPE;
}
