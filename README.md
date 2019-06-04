# BakkesMod Injector

## Introduction
This is the entry point for the BakkesMod executable. It is primarily responsible for injecting the mod into the RocketLeague.exe process. This repository uses Visual Studio + Qt to produce a GUI for injection into RocketLeague. It may be possible to build this on UNIX devices; however this repository contains Windows only instruction sets.

*Note:* Building the injector is not a requirement to run the mod. Simply run an existing executable found on the main site at [bakkesmod.com](https://bakkesmod.com/).

## Contact Us
Join the [community discord](https://discord.gg/RqaZAbM) and direct issues to the #programming channel. Someone, perhaps Bakkes himself, should be able to help with inquiries.

## Building the Injector & Contributing
### External Dependencies
Should you be unable to find the dependencies listed below, please file an issue or reachout on the community discord:
* [miniz-master](https://drive.google.com/drive/folders/1dcPjUgWUgOntheDENWL4t-vcgGZSDbdb?usp=sharing) or [here](https://github.com/richgel999/miniz) - Download, or clone, these folders to a safe place in your drive (perhaps where you've cloned this very injector's repo).
* [Qt5.6.0-static](https://drive.google.com/file/d/1v61jCe3RSYFbNHTZWGJY5-RJNZBFMI2G/view?usp=sharing) - Extract this zip to a safe place on your drive. Qt is normally installed at the root of the disk (e.g. C:\Qt\Qt5.6.0-static)
* [OpenSSL-win32\lib](https://drive.google.com/file/d/14RsbTCeX1pE4PVCQZGymjKPM61NTxyzm/view?usp=sharing) - Extract this zip near this repo. In the future, this repo may include these.

### Install Visual Studio IDE
Download a version of Visual Studio later than 2017. Here's the free: [community version](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=16#)

You'll want the `Desktop development with C++` workload as well as the various C++ build tools, MSVC v142, and Windows 10 SDK 10.0.17763.0 (or higher) in your installation.

### Setting Up the IDE
1. You'll need to add the Qt VS Tools extensions within Visual Studio IDE. To do this, in the taskbar click Extensions -> Manage Extensions; and in the side-bar click Online and search for Qt. Qt Visual Studio Tools should be listed. Initial installation of extensions may require a restart of the Visual Studio IDE.
2. Add the version of Qt to your project. In Extensions -> Qt VS Tools -> Qt Options: click `Add` and select the path to the Qt5.6.0-static you extracted (e.g. C:\Qt\Qt5.6.0-static)
3. Link the other external dependencies with environment variables. With `Control Panel -> System -> Advanced system settings -> Advanced`, or alternatively using the super key, typing environment variable and selecting the control panel item, click the `Environment Variables...` button. Create two system variables:
	* Variable name: `MINIZ` ; Variable value: `path-to-your\miniz-master`
	* Variable name: `OpenSSL` ; Variable value: `path-to-your\openssl-win32\lib`
4. Select the main.cpp file and verify the include paths are all setup. If there are still issues in linking sources. Use vcpkg to install qt5-base. Follow this instruction set: [here](https://devblogs.microsoft.com/cppblog/vcpkg-a-tool-to-acquire-and-build-c-open-source-libraries-on-windows/)
	* `git clone https://github.com/Microsoft/vcpkg`
    * `cd vcpkg`
    * `.\bootstrap-vcpkg.bat`
    * `vcpkg install qt5-base`
5. Restart Visual Studio IDE, or reload the project, in order to update the paths for Visual Studio. Build the project and you'll find the executable under the source's `\Win32\Release` directory.