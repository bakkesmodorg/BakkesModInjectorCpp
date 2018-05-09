#include "BakkesModInstallation.h"
#include <fstream>
#include <string>
#include <qfiledialog.h>

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
		std::wstring registryString = settings.GetStringSetting(L"BakkesModPath", RegisterySettingsManager::REGISTRY_DIR_APPPATH);
		if (!registryString.empty())
		{
			bakkesModFolder = windowsUtils.WStringToString(registryString);
			return bakkesModFolder;
		}

		std::string rlPath = windowsUtils.GetRocketLeagueDirFromLog();
		if (!rlPath.empty())
		{
			bakkesModFolder = rlPath + "bakkesmod\\";
		}
		else
		{
			QString path = QFileDialog::getOpenFileName(this,
				QString("Select the Rocket League executable"), QString("C:/"), QString("RocketLeague.exe (RocketLeague.exe)"));
			bakkesModFolder = path.toStdString().substr(0, path.size() - std::string("RocketLeague.exe").size()) + "bakkesmod\\";
		}
		auto converted = windowsUtils.StringToWString(bakkesModFolder);
		settings.SaveSetting(L"BakkesModPath", converted, RegisterySettingsManager::REGISTRY_DIR_APPPATH);
		
	}
	return bakkesModFolder;
}

bool BakkesModInstallation::IsInstalled()
{
	return GetVersion() > 0;
}

unsigned int BakkesModInstallation::GetVersion()
{
	std::string versionFilePath = GetBakkesModFolder() + "\\version.txt";
	if (!windowsUtils.FileExists(versionFilePath))
		return 0;
	std::ifstream versionFile;
	versionFile.open(versionFilePath);
	int version = -1;
	versionFile >> version;
	versionFile.close();
	return version;
}
