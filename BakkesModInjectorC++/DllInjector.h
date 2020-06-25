#pragma once
#include <string>
#include <windows.h>

enum DllInjectionResult
{
	NOPE = 0,
	OK = 1,
	NOT_RUNNING = 2
};

struct InjectionParameters
{
	int version = 0;
	int injectionMode = 0; //0 is RL already running, 1 = RL just started
};

class DllInjector
{
private:
	InjectionParameters* injectionParameters;
public:
	DllInjector();
	~DllInjector();
	InjectionParameters* GetInjectionParameters();
	void SetInjectionParameters(InjectionParameters ip);
	DWORD InjectDLL(std::wstring processName, std::string path);
	DWORD GetProcessID64(std::wstring processName);
	DWORD IsBakkesModDllInjected(std::wstring processName);
};
