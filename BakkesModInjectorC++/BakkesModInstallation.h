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
	std::filesystem::path GetBakkesModFolder();
	std::filesystem::path GetOldBakkesModFolder();
	bool IsInstalled();
	unsigned int GetVersion();
	bool IsSafeToInject(UpdateStatus currentVersion);
	bool ManifestFileExists();
	std::string DetectRocketLeagueFolder();
};
