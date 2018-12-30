#pragma once
#include <string>
class Installer
{
private:
	std::wstring updatePackage = L"";
	std::string extractDir = "";
public:
	Installer(std::wstring updatePackageLocation, std::string destination);
	~Installer();
	void Install();
};
