#include "BakkesModInstallation.h"
#include <fstream>
#include <string>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <fstream>
#include <sstream>
#include "logger.h"
std::string BakkesModInstallation::overrideBakkesModFolder = "";

BakkesModInstallation::BakkesModInstallation()
{
}


BakkesModInstallation::~BakkesModInstallation()
{
}

std::string BakkesModInstallation::GetBakkesModFolder()
{
	if (!overrideBakkesModFolder.empty())
		return overrideBakkesModFolder;
	if (bakkesModFolder.empty())
	{
		std::wstring registryString = settings.GetStringSetting(L"BakkesModPath", RegisterySettingsManager::REGISTRY_DIR_APPPATH);
		if (!registryString.empty() && WindowsUtils::FileExists(windowsUtils.WStringToString(registryString) + "../RocketLeague.exe"))
		{
			bakkesModFolder = windowsUtils.WStringToString(registryString);
			return bakkesModFolder;
		}

		std::string rlPath = windowsUtils.GetRocketLeagueDirFromLog();
		if (!rlPath.empty() && WindowsUtils::FileExists(rlPath + "RocketLeague.exe"))
		{
			bakkesModFolder = rlPath + "bakkesmod\\";
			LOG_LINE(INFO, "Automatically detected Rocket League path: " << rlPath)
		}
		else
		{
			LOG_LINE(INFO, "Could not find Rocket League path, asking user.")
			{
				QMessageBox msgBox2;
				std::stringstream ss;
				ss << "Could not find the Rocket League folder, please select it in the next window" << std::endl;
				ss << "The executable can be found in SteamLibrary\\steamapps\\common\\rocketleague\\Binaries\\Win32\\RocketLeague.exe";
				msgBox2.setText(ss.str().c_str());
				msgBox2.setStandardButtons(QMessageBox::Ok);
				msgBox2.setDefaultButton(QMessageBox::Ok);
				int ret = msgBox2.exec();
			}
			QString path = QFileDialog::getOpenFileName(this,
				QString("Select the Rocket League executable"), QString(QDir::currentPath()), QString("RocketLeague.exe (RocketLeague.exe)"));
			std::string rlPath = path.toStdString().substr(0, path.size() - std::string("RocketLeague.exe").size());
			bakkesModFolder = rlPath +"bakkesmod\\";
			LOG_LINE(INFO, "User selected executable: " << path.toStdString())
			if (!WindowsUtils::FileExists(rlPath))
			{

				LOG_LINE(INFO, "Folder does not exist: " << rlPath)
				QMessageBox msgBox2;
				msgBox2.setText("Did not select bakkesmod folder?");
				msgBox2.setStandardButtons(QMessageBox::Ok);
				msgBox2.setDefaultButton(QMessageBox::Ok);
				int ret = msgBox2.exec();
				bakkesModFolder = "";
				return "";
			}
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
	int version = 0;
	versionFile >> version;
	versionFile.close();
	return version;
}

bool BakkesModInstallation::IsSafeToInject(UpdateStatus currentVersion)
{
	std::string manifest = GetBakkesModFolder() + "..\\..\\..\\..\\..\\appmanifest_252950.acf";
	if (ManifestFileExists())
	{
		std::ifstream myfile(manifest);
		std::string test;
		while (myfile >> test)
		{
			if (test.compare("\"buildid\"") == 0)
			{
				std::string buildid = "";
				myfile >> buildid;
				std::string buildidwithoutquotes = buildid.substr(1, buildid.size() - 2);
				if (currentVersion.buildIds.size() > 0)
				{
					for (std::string buildidz : currentVersion.buildIds)
					{
						LOG_LINE(INFO, "Comparing buildid " << buildidwithoutquotes << " to " << buildidz)
						if (buildidwithoutquotes.compare(buildidz) == 0)
							return true;
					}
				}
				LOG_LINE(INFO, "Comparing buildid " << buildidwithoutquotes << " to " << currentVersion.buildID)
				return buildidwithoutquotes.compare(currentVersion.buildID) == 0;
			}
		}
		LOG_LINE(INFO, "buildid line not found")
	}
	else {
		LOG_LINE(INFO, "Could not find manifest file at " << manifest << " guess I'll disable safe mode manually?")
		return true;
	}
	return false;
}

bool BakkesModInstallation::ManifestFileExists()
{
	std::string manifest = GetBakkesModFolder() + "..\\..\\..\\..\\..\\appmanifest_252950.acf";
	return (WindowsUtils::FileExists(manifest));
}
