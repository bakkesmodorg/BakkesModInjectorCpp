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
	std::string versionFilePath =windowsUtils.GetRocketLeagueDirFromLog() + "bakkesmod\\version.txt";
	if (!windowsUtils.FileExists(versionFilePath))
		return 0;
	std::ifstream versionFile;
	versionFile.open(versionFilePath);
	int version = 0;
	versionFile >> version;
	versionFile.close();
	return version;
}
