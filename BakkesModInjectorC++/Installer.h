#pragma once
#include <string>
#include <filesystem>
class Installer
{
private:
	std::wstring updatePackage = L"";
	std::filesystem::path extractDir = "";
public:
	Installer(std::wstring updatePackageLocation, std::filesystem::path destination);
	~Installer();
	void Install();
};
