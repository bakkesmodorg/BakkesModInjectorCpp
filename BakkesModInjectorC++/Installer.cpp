#include "stdafx.h"
#undef max
#undef min
#include "Installer.h"
#include "zip_file.hpp"
#include "WindowsUtils.h"
#include "logger.h"

Installer::Installer(std::string updatePackageLocation, std::string destination)
{
	updatePackage = updatePackageLocation;
	extractDir = destination;
	LOG_LINE(INFO, "Initialized installer, location=" << updatePackage << ", destination=" << extractDir)
}

Installer::~Installer()
{
}

void Installer::Install()
{
	LOG_LINE(INFO, "Installing update")
	miniz_cpp::zip_file file(updatePackage);
	WindowsUtils::CreateFolder(extractDir);
	for (auto &member : file.infolist())
	{
		std::string fullPath = extractDir + member.filename;
		LOG_LINE(INFO, "Extracting file " << member.filename)
		if (fullPath.substr(fullPath.size() - std::string(".cfg").size()).compare(".cfg") == 0)
		{
			
			if (WindowsUtils::FileExists(fullPath)) {//Don't overwrite default cfg files
				LOG_LINE(INFO, "File (" << member.filename << ") is cfg file and it already exists, not overwriting!")
				continue;
			}
		}
		if (member.filename.find(".") == std::string::npos) //It's a folder
		{
			LOG_LINE(INFO, "File " << member.filename << ") is folder, creating if not exists yet")
			WindowsUtils::CreateFolder(fullPath);
		} else if (WindowsUtils::FileExists(fullPath))
		{
			int success = remove(fullPath.c_str());
			LOG_LINE(INFO, "File " << member.filename << " exists, deleting file result: " << success)
		}
		
		file.extract(member, extractDir);
		LOG_LINE(INFO, "File extraction result: " << WindowsUtils::FileExists(fullPath))
		//file.extract(member, "");
	}
	file.printdir(LOG(INFO));
}
