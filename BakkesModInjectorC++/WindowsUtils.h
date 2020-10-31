#pragma once
#include <string>
#include <windows.h>
#include <filesystem>
class WindowsUtils
{
public:
	WindowsUtils();
	~WindowsUtils();
	static std::string GetMyDocumentsFolder();
	std::filesystem::path GetSteamRocketLeagueDirFromLog();
	std::string GetRocketLeagueBuildID();
	//static bool FileExists(std::string path);
	static bool FileExists(std::filesystem::path path);
	static void OpenFolder(std::filesystem::path path);
	static void CreateFolder(std::filesystem::path path);
	static DWORD IsProcessRunning(std::wstring processName);
	static std::wstring StringToWString(const std::string& s);
	static std::string WStringToString(const std::wstring& s);
	static std::wstring GetCurrentExecutablePath();
	static uintmax_t DeleteDirectory(std::filesystem::path loc);
};

template<typename charT>
struct my_equal
{
	my_equal(const std::locale& loc) : loc_(loc) {}
	bool operator()(charT ch1, charT ch2)
	{
		return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
	}
private:
	const std::locale& loc_;
};

// find substring (case insensitive)
template<typename T>
static int ci_find_substr(const T& str1, const T& str2, const std::locale& loc = std::locale())
{
	typename T::const_iterator it = std::search(str1.begin(), str1.end(),
		str2.begin(), str2.end(), my_equal<typename T::value_type>(loc));
	if (it != str1.end()) return it - str1.begin();
	else return -1; // not found
}