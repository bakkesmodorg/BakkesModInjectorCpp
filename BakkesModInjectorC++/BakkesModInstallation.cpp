#include "BakkesModInstallation.h"
#include <fstream>


BakkesModInstallation::BakkesModInstallation()
{
}


BakkesModInstallation::~BakkesModInstallation()
{
}

std::string BakkesModInstallation::GetBakkesModFolder()
{
	if (bakkesModFolder.empty())
	{
		std::string registryString = settings.GetStringSetting(L"BakkesModPath", RegisterySettingsManager::REGISTRY_DIR_APPPATH);
		if (!registryString.empty())
		{
			bakkesModFolder = registryString;
			return bakkesModFolder;
		}

		std::string rlPath = windowsUtils.GetRocketLeagueDirFromLog();
		if (!rlPath.empty())
		{
			bakkesModFolder = rlPath + "bakkesmod/";
			
		}
		else
		{
			std::string path = QFileDialog::getOpenFileName(this,
				tr("Select the Rocket League executable"), "C:/", tr("RocketLeague.exe (RocketLeague.exe)"));
			bakkesModFolder = path.substr(0, path.size() - "RocketLeague.exe".size());
		}
		settings.SaveSetting(L"BakkesModPath", windowsUtils.StringToWString(bakkesModFolder), RegisterySettingsManager::REGISTRY_DIR_APPPATH);
		
	}
	return bakkesModFolder;
}

bool BakkesModInstallation::IsInstalled()
{
	return GetVersion() > 0;
}

unsigned int BakkesModInstallation::GetVersion()
{
	std::string versionFilePath = windowsUtils.GetRocketLeagueDirFromLog() + "bakkesmod\\version.txt";
	if (!windowsUtils.FileExists(versionFilePath))
		return 0;
	std::ifstream versionFile;
	versionFile.open(versionFilePath);
	int version = -1;
	versionFile >> version;
	versionFile.close();
	return version;
}
