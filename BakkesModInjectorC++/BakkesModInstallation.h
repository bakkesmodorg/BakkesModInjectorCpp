#pragma once
#include "windowsutils.h"
#include "SettingsManager.h"
#include <qwidget.h>
class BakkesModInstallation : public QWidget
{
private:
	std::string bakkesModFolder = "";
	WindowsUtils windowsUtils;
	RegisterySettingsManager settings;
public:
	static std::string overrideBakkesModFolder;
	BakkesModInstallation();
	~BakkesModInstallation();
	std::string GetBakkesModFolder();
	bool IsInstalled();
	unsigned int GetVersion();
	bool IsSafeToInject(std::string currentVersion);
};

