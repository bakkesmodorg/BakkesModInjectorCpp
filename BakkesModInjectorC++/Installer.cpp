#include "Installer.h"
#include "zip_file.hpp"
#include "WindowsUtils.h"


Installer::Installer(std::string updatePackageLocation, std::string destination)
{
	updatePackage = updatePackageLocation;
	extractDir = destination;
}

Installer::~Installer()
{
}

void Installer::Install()
{
	miniz_cpp::zip_file file(updatePackage);
	WindowsUtils::CreateFolder(extractDir);
	for (auto &member : file.infolist())
	{
		std::string fullPath = extractDir + member.filename;
		if (fullPath.substr(fullPath.size() - std::string(".cfg").size()).compare(".cfg") == 0)
		{
			if (WindowsUtils::FileExists(fullPath)) //Don't overwrite default cfg files
				continue;
		}
		if (member.filename.find(".") == std::string::npos)
		{
			WindowsUtils::CreateFolder(fullPath);
		}
		file.extract(member, extractDir);

		//file.extract(member, "");
	}
	file.printdir();
}
