#include "BakkesModInstallation.h"
#include <fstream>


BakkesModInstallation::BakkesModInstallation()
{
}


BakkesModInstallation::~BakkesModInstallation()
{
}

bool BakkesModInstallation::IsInstalled()
{
	return GetVersion() > 0;
}

unsigned int BakkesModInstallation::GetVersion()
{
	std::string versionFilePath = windowsUtils.GetRocketLeagueDirFromLog() + "version.txt";
	if (!windowsUtils.FileExists(versionFilePath))
		return 0;
	std::ifstream versionFile(versionFilePath);
	unsigned int version = 0;
	versionFile >> version;
	return version;
}
