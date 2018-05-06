#pragma once
#include "windowsutils.h"
class BakkesModInstallation
{
private:
	WindowsUtils windowsUtils;
public:
	BakkesModInstallation();
	~BakkesModInstallation();
	bool IsInstalled();
	unsigned int GetVersion();
	
};

