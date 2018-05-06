#pragma once
#include <string>
#include <windows.h>
enum DllInjectionResult
{
	NOPE = 0,
	OK = 1,
	NOT_RUNNING = 2
};

class DllInjector
{
public:
	DllInjector();
	~DllInjector();
	DWORD InjectDLL(std::wstring processName, std::string path);
	DWORD GetProcessID(std::wstring processName);
	DWORD IsBakkesModDllInjected(std::wstring processName);
};

