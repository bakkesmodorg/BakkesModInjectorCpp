#pragma once
#include <string>
class Installer
{
private:
	std::string updatePackage = "";
	std::string extractDir = "";
public:
	Installer(std::string updatePackageLocation, std::string destination);
	~Installer();
	void Install();
};

