#include "BakkesModInstallation.h"
#include <fstream>
#include <string>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>
#include <fstream>
#include <sstream>
#include "logger.h"
#include "vdf_parser.h"
#include <fstream>
#include <shlobj_core.h>
std::string BakkesModInstallation::overrideBakkesModFolder = "";

void BakkesModInstallation::resetBMFolder()
{
	bakkesModFolder = "";
}

BakkesModInstallation::BakkesModInstallation()
{
}


BakkesModInstallation::~BakkesModInstallation()
{
}

std::filesystem::path BakkesModInstallation::GetBakkesModFolder()
{
	if (!overrideBakkesModFolder.empty())
		return overrideBakkesModFolder;

	if (bakkesModFolder.empty())
	{
		PWSTR path_tmp;
		auto get_folder_path_ret = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path_tmp);
		/* Error check */
		if (get_folder_path_ret != S_OK)
		{
			CoTaskMemFree(path_tmp);
			//SHOULD NEVER REACH

		}
		else
		{

			/* Convert the Windows path type to a C++ path */
			std::filesystem::path bmPath = path_tmp;
			bmPath = bmPath / "bakkesmod" / "bakkesmod";
			bakkesModFolder = bmPath;
			CoTaskMemFree(path_tmp);
			return bakkesModFolder;
		}
		
		/*std::wstring registryString = settings.GetStringSetting(L"BakkesModPath", RegisterySettingsManager::REGISTRY_DIR_APPPATH);
		if (!registryString.empty() && WindowsUtils::FileExists(windowsUtils.WStringToString(registryString) + "../RocketLeague.exe"))
		{
			bakkesModFolder = windowsUtils.WStringToString(registryString);
			return bakkesModFolder;
		}

		std::string newDetectedFolder = DetectRocketLeagueFolder();
		std::string rlPath = windowsUtils.GetRocketLeagueDirFromLog();
		if (!rlPath.empty() && WindowsUtils::FileExists(rlPath + "RocketLeague.exe"))
		{
			bakkesModFolder = rlPath + "bakkesmod\\";
			LOG_LINE(INFO, "Automatically detected Rocket League path: " << rlPath)
		} else if (!newDetectedFolder.empty()) {
			LOG_LINE(INFO, "Advanced detection found installation path " << newDetectedFolder)
			rlPath = newDetectedFolder;
			bakkesModFolder = rlPath + "bakkesmod\\";

		}
		else
		{
			LOG_LINE(INFO, "Could not find Rocket League path, asking user.")
			{
				QMessageBox msgBox2;
				std::stringstream ss;
				ss << "Could not find the Rocket League folder, please select it in the next window" << std::endl;
				ss << "The executable can be found in SteamLibrary\\steamapps\\common\\rocketleague\\Binaries\\Win64\\RocketLeague.exe";
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
		auto converted = bakkesModFolder.wstring();
		settings.SaveSetting(L"BakkesModPath", converted, RegisterySettingsManager::REGISTRY_DIR_APPPATH);

	}

	return bakkesModFolder;*/
	}
	return std::filesystem::path("");
}



std::filesystem::path BakkesModInstallation::GetOldBakkesModFolder()
{
	//auto test = GetBakkesModFolder();
	auto oldPath = settings.GetStringSetting(L"BakkesModPath", RegisterySettingsManager::REGISTRY_DIR_APPPATH);
	if (ci_find_substr(oldPath, std::wstring(L"appdata")))
	{
		return std::filesystem::path(oldPath);
	}
	return std::filesystem::path("");
}

bool BakkesModInstallation::IsInstalled()
{
	return GetVersion() > 0;
}

unsigned int BakkesModInstallation::GetVersion()
{
	std::filesystem::path versionFilePath = GetBakkesModFolder() / "\\version.txt";
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
	std::filesystem::path manifest = GetBakkesModFolder() / "..\\..\\..\\..\\..\\appmanifest_252950.acf";
	if (WindowsUtils::FileExists(manifest))
	{
		LOG_LINE(INFO, "Path contains a manifest file");
		std::ifstream manifestStream(manifest);
		std::string str((std::istreambuf_iterator<char>(manifestStream)),
			std::istreambuf_iterator<char>());
		if (str.find("buildid") == std::string::npos) //broken
		{
			return false;
		}
		manifestStream = std::ifstream(manifest);
		tyti::vdf::object manifestRoot;
		std::string buildId = "";
		try {
			manifestRoot = tyti::vdf::read(manifestStream);
		}
		catch (...)
		{
			manifestStream.seekg(0, manifestStream.beg);
			std::string line = "";
			while (std::getline(manifestStream, line))
			{
				if (line.find("buildid") != std::string::npos)
				{
					std::string bid = "";
					for (auto c : line)
					{
						if (c >= '0' && c <= '9')
						{
							bid += c;
						}
					}
					buildId = bid;
					break;
				}
			}
			manifestRoot.name = "ERR";
		}
		if (buildId.size() == 0)
		{
			if (manifestRoot.name.compare("AppState") == std::string::npos)
			{
				LOG_LINE(INFO, "Manifest at " << manifest << " corrupt? Guess I'll disable safe mode manually?")
					return true;
			}

			if (manifestRoot.attribs.find("buildid") == manifestRoot.attribs.end())
			{
				LOG_LINE(INFO, "Manifest does not contain a buildid");
				return false;
			}

			buildId = manifestRoot.attribs["buildid"];
		}
		if (currentVersion.buildIds.size() > 0)
		{
			for (std::string buildidz : currentVersion.buildIds)
			{
				LOG_LINE(INFO, "Comparing buildid " << buildId << " to " << buildidz)
				if (buildId.compare(buildidz) == 0)
					return true;
			}
		}
	} else {
		LOG_LINE(INFO, "Could not find manifest file at " << manifest << " guess I'll disable safe mode manually?")
		return true;
	}
	return false;
}

bool BakkesModInstallation::ManifestFileExists()
{
	auto manifest = GetBakkesModFolder() / "..\\..\\..\\..\\..\\appmanifest_252950.acf";
	return (WindowsUtils::FileExists(manifest));
}

std::string BakkesModInstallation::DetectRocketLeagueFolder()
{
	std::wstring path = settings.GetStringSetting(L"SteamPath", L"SOFTWARE\\Valve\\Steam") + L"\\steamapps\\";
	std::wstring vdfFile = path + L"libraryfolders.vdf";
	if (!WindowsUtils::FileExists(WindowsUtils::WStringToString(vdfFile)))
	{
		LOG_LINE(INFO, "Could not find steamapps folder, detected " << vdfFile.c_str());
		return "";
	}
	std::ifstream vdfStream(WindowsUtils::WStringToString(vdfFile));
	tyti::vdf::object root = tyti::vdf::read(vdfStream);
	if (root.name.compare("LibraryFolders") == std::string::npos)
	{
		LOG_LINE(INFO, "File root is not LibraryFolders, but " << root.name.c_str());
		return "";
	}

	std::string installationPath = "";
	int lastBuildId = -1;
	for (auto child : root.attribs)
	{
		bool isPath = WindowsUtils::FileExists(child.second);
		LOG_LINE(INFO, "Key " << child.first << " = " << child.second << ". is path=" << (isPath ? "true" : "false"));
		if (isPath)
		{
			std::string newPath = child.second + "\\steamapps\\";
			std::string manifestPath = newPath + "appmanifest_252950.acf";
			std::string executablePath = newPath + "common\\rocketleague\\Binaries\\Win64\\";
			std::string executableLocation = executablePath + "RocketLeague.exe";
			if (WindowsUtils::FileExists(manifestPath))
			{
				try {
					LOG_LINE(INFO, "Path contains a manifest file");
					std::ifstream manifestStream(manifestPath);
					tyti::vdf::object manifestRoot = tyti::vdf::read(manifestStream);

					if (manifestRoot.name.compare("AppState") == std::string::npos)
					{
						LOG_LINE(INFO, "Manifest is invalid, skipping");
						continue;
					}

					if (manifestRoot.attribs.find("buildid") == manifestRoot.attribs.end())
					{
						LOG_LINE(INFO, "Manifest does not contain a buildid");
						continue;
					}
					std::string buildId = manifestRoot.attribs["buildid"];
					int buildIdInteger = stoi(buildId);
					if (buildIdInteger > lastBuildId)
					{
						LOG_LINE(INFO, "Found buildid " << buildIdInteger << " (newest high)");
						if (!WindowsUtils::FileExists(executableLocation))
						{
							LOG_LINE(INFO, "Could not find RocketLeague.exe at " << executableLocation);
							continue;
						}
						lastBuildId = buildIdInteger;
						installationPath = executablePath;
						LOG_LINE(INFO, "Found RocketLeague.exe at " << installationPath);
					}
				}
				catch (...)
				{
					LOG_LINE(INFO, "Error parsing manifest file, skipping this one")
				}
			}
		}
	}

	return installationPath;
}
