#pragma once
#include "stdafx.h"
#include "windowsutils.h"
#include "SettingsManager.h"
#include <QTWidgets/qwidget.h>
#include <filesystem>
class BakkesModInstallation : public QWidget
{
private:
	std::filesystem::path bakkesModFolder = "";
	WindowsUtils windowsUtils;
	RegisterySettingsManager settings;
public:
	void resetBMFolder();
	static std::string overrideBakkesModFolder;
	BakkesModInstallation();
	~BakkesModInstallation();
	void CreateAppDataFolderIfDoesntExist();
	std::filesystem::path GetBakkesModFolder();
	std::filesystem::path GetOldBakkesModFolder();
	bool IsInstalled();
	unsigned int GetVersion();
	bool IsSafeToInject(UpdateStatus currentVersion);

	//Steam
	bool ManifestFileExists();
	std::filesystem::path DetectRocketLeagueFolder();
	int GetBuildIDFromManifest(std::filesystem::path manifestPath);

	std::filesystem::path GetSteamInstallLocation();
	std::string GetSteamVersion();
	std::string GetHighestEpicVersion();
	std::vector<std::string> GetEpicVersion();

	bool IsSteamVersionInstalled();
	bool IsEpicVersionInstalled();
	bool IsSteamVersionReady(UpdateStatus currentVersion);
	bool IsEpicVersionReady(UpdateStatus currentVersion);
};
