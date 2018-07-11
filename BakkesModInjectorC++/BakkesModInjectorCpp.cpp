#include "stdafx.h"
#include "BakkesModInjectorCpp.h"
#include "qmessagebox.h"
#include <sstream>
#include "updatedownloader.h"
#include <tlhelp32.h>
#include "Installer.h"
#include <direct.h>
#include <qshortcut.h>
#include <fstream>
#include "logger.h"
#define INJECTION_TIMEOUT_DEFAULT 2500



BakkesModInjectorCpp::BakkesModInjectorCpp(QWidget *parent)
	: QMainWindow(parent)
{
	std::wstring TempPath;
	wchar_t wcharPath[MAX_PATH];
	if (GetTempPathW(MAX_PATH, wcharPath))
		TempPath = wcharPath;
	AixLog::Log::init<AixLog::SinkFile>(AixLog::Severity::trace, AixLog::Type::all, WindowsUtils::WStringToString(TempPath) + "\\injectorlog.log");
	LOG_LINE(INFO, "Initialized logger")

	ui.setupUi(this);
	LOG_LINE(INFO, "Set up UI")
	connect(&timer, SIGNAL(timeout()), this, SLOT(TimerTimeout()));
	QIcon icon;
	if(qApp->arguments().contains("icon"))
	{
		icon = QIcon(qApp->arguments().at(qApp->arguments().indexOf("icon")));
		LOG(INFO, "Custom icon " << qApp->arguments().at(qApp->arguments().indexOf("icon")));
	}
	else {
		icon = QIcon(":/BakkesModInjectorCpp/mainicon");
	}
	this->setWindowIcon(icon);
	LOG_LINE(INFO, "Set window icon")
	//Create tray icon
	trayIcon = new QSystemTrayIcon(icon, this);
	
	QMenu* menu = new QMenu();
	QAction* openAction = new QAction("Open", menu);
	QAction* closeAction = new QAction("Exit", menu);

	connect(openAction, SIGNAL(triggered()), this, SLOT(TrayOpenAction()));
	connect(closeAction, SIGNAL(triggered()), this, SLOT(TrayCloseAction()));
	//qApp->arguments().at(qApp->arguments().indexOf("icon"))
	menu->addAction(openAction);
	menu->addAction(closeAction);
	trayIcon->setContextMenu(menu);

	trayIcon->show();
	LOG_LINE(INFO, "Initialized tray icon")
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));

	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this, SLOT(ReleaseDLL()));
	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_T), this, SLOT(DebugDLL()));

}

void BakkesModInjectorCpp::initialize()
{
	timer.start(500);
	QApplication::setQuitOnLastWindowClosed(false);
	ui.progressBar->hide();
	LOG_LINE(INFO, "Checking if hideonboot is on")
	bool hideOnBoot = settingsManager.GetIntSetting(L"HideOnBoot");
	ui.actionMinimize_on_start->setChecked(hideOnBoot);
	if (hideOnBoot)
	{
		LOG_LINE(INFO, "Hiding to tray")
		this->showMinimized();
	}

	//Just had an update, remove the old file
	LOG_LINE(INFO, "Removing old exe")
	if (WindowsUtils::FileExists("bakkesmod_old.exe"))
	{
		remove("bakkesmod_old.exe");
	}

	int timeout = settingsManager.GetIntSetting(L"InjectionTimeout");
	if (timeout == 0)
	{
		timeout = INJECTION_TIMEOUT_DEFAULT;
	}
	LOG_LINE(INFO, "Current injection timeout is " << timeout)
	ui.actionSet_injection_timeout->setText(QString(std::string("Set injection timeout (" + std::to_string(timeout) + ")").c_str()));
	//settingsManager.SaveSetting(L"EnableSafeMode", (int)newStatus);
}

void BakkesModInjectorCpp::changeEvent(QEvent* e)
{
	QApplication::setQuitOnLastWindowClosed(true);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		//this->ui->retranslateUi(this);
		break;
	case QEvent::WindowStateChange:
	{
		if (this->windowState() & Qt::WindowMinimized)
		{
			if (settingsManager.GetIntSetting(L"HideOnMinimize")) 
			{
				QApplication::setQuitOnLastWindowClosed(false);
				QTimer::singleShot(50, this, SLOT(hide()));
			}
		}
		break;
	}
	default:
		break;
	}

	QMainWindow::changeEvent(e);
}

void BakkesModInjectorCpp::OpenWebsite(std::string url)
{
	ShellExecute(0, 0, WindowsUtils::StringToWString(url).c_str(), 0, 0, SW_SHOW);
}

void BakkesModInjectorCpp::SetState(BakkesModStatus newState)
{
	LOG_LINE(INFO, "Switching from " << GetStateName(bakkesModState) << " (" << bakkesModState << ")" << " to " << GetStateName(newState) << " (" << newState << ")")
	bakkesModState = newState;
}

std::string BakkesModInjectorCpp::GetStatusString()
{
	std::string status = "";
	switch (bakkesModState)
	{
	case BOOTING:
		status = "Starting...";
		break;
	case BAKKESMOD_IDLE:
		status = "Uninjected";
		break;
	case WAITING_FOR_RL:
		break;
	case OUT_OF_DATE:
		status = "Out of date";
		break;
	case OUT_OF_DATE_SAFEMODE_ENABLED:
		status = "Mod is out of date, waiting for an update";
		break;
	case CHECKING_FOR_UPDATES:
		status = "Checking for updates";
		break;
	case START_UPDATING:
		status = "Starting update process";
		break;
	case UPDATING_BAKKESMOD:
		status = "Updating BakkesMod";
		break;
	case UPDATING_INJECTOR:
		status = "Updating Injector";
		break;
	case INJECT_DLL:
		status = "Injecting DLL";
		break;
	case INJECTED:
		status = "Injected";
		break;
	case INJECTION_FAILED:
		status = "Injection failed, please download vc_redist.x86.exe and restart your PC";
		break;
	case BAKKESMOD_INSTALLING:
		status = "Extracting archive";
		break;
	case REINSTALL:
		status = "Reinstalling BakkesMod";
		break;
	case INSTALLATION_CORRUPT:
		status = "Installation corrupted, please do a reinstall (File -> reinstall)";
		break;
	}
	return status;
}

void BakkesModInjectorCpp::OnCheckInjection() 
{
	DWORD injectionResult = dllInjector.IsBakkesModDllInjected(L"RocketLeague.exe");
	QMessageBox msgBox;
	
	if (injectionResult == NOT_RUNNING)
	{
		msgBox.setText("Rocket League is not running");
	}
	else if (injectionResult == NOPE)
	{
		msgBox.setText("BakkesMod DLL not found in Rocket League process");
	}
	else if (injectionResult == OK)
	{
		msgBox.setText("Injection successful");
	}
	else
	{
		msgBox.setText("??");
	}
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	int ret = msgBox.exec();
}

void BakkesModInjectorCpp::TimerTimeout()
{
	static UpdateDownloader* updateDownloader = NULL;
	//LOG_LINE(INFO, GetStateName(bakkesModState))
	switch (bakkesModState)
	{
	case BOOTING:
	{
		LOG_LINE(INFO, "Booting")
		if (!installation.IsInstalled())
		{
			LOG_LINE(INFO, "Not installed, setting default values")
			settingsManager.SaveSetting(L"EnableSafeMode", 1);
			settingsManager.SaveSetting(L"HideOnMinimize", 1);
			settingsManager.SaveSetting(L"HideOnBoot", 0);
			settingsManager.SaveSetting(L"BakkesMod", L"-", RegisterySettingsManager::REGISTRY_DIR_RUN);
			LOG_LINE(INFO, "Default settings set")
			//settingsManager.SaveSetting(L"InjectionTimeout", 70);
		}

		ui.actionEnable_safe_mode->setChecked(settingsManager.GetIntSetting(L"EnableSafeMode"));
		safeModeEnabled = ui.actionEnable_safe_mode->isChecked();
		ui.actionHide_when_minimized->setChecked(settingsManager.GetIntSetting(L"HideOnMinimize"));
		ui.actionRun_on_startup->setChecked(!settingsManager.GetStringSetting(L"BakkesMod", RegisterySettingsManager::REGISTRY_DIR_RUN).empty());
		OnRunOnStartup();
		int version = installation.GetVersion();
		updater.CheckForUpdates(version);
		SetState(CHECKING_FOR_UPDATES);
		LOG_LINE(INFO, "Checking for updates, current version = " << version);
		timer.setInterval(50);
	}
		break;
	case CHECKING_FOR_UPDATES:
	{
		if (updater.latestUpdateInfo.requestFinished)
		{
			LOG_LINE(INFO, "Request finished")
			if (updater.latestUpdateInfo.networkRequestStatus == FINISHED_ERROR)
			{
				LOG_LINE(WARNING, "Error connecting to update server")
				SetState(BAKKESMOD_IDLE);
				QMessageBox msgBox2;
				std::stringstream ss;
				msgBox2.setText("Could not connect to update server, running in offline mode");
				msgBox2.setStandardButtons(QMessageBox::Ok);
				msgBox2.setDefaultButton(QMessageBox::Ok);
				int ret = msgBox2.exec();
			}
			else
			{
				LOG_LINE(INFO, "Successfully received response from BakkesMod update server")
				if (std::stoi(updater.latestUpdateInfo.injectorVersion) > BAKKESMODINJECTOR_VERSION)
				{
					LOG_LINE(INFO, "Injector out of date, current version = " << BAKKESMODINJECTOR_VERSION << " whilst " << updater.latestUpdateInfo.injectorVersion << " is available")
					QMessageBox msgBox;

					std::stringstream ss;
					ss << "An new version of the injector is available " << std::endl;
					ss << "Would you like to download?";
					msgBox.setText(ss.str().c_str());
					msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
					msgBox.setDefaultButton(QMessageBox::Yes);
					int ret = msgBox.exec();
					if (ret == QMessageBox::Yes)
					{
						LOG_LINE(INFO, "User accepted injector update")
						SetState(UPDATING_INJECTOR);
						updateDownloader = new UpdateDownloader(updater.latestUpdateInfo.injectorUrl, "newinjector.exe");
						updateDownloader->StartDownload();
						ui.progressBar->setMaximum(100);
						return;
					}
					LOG_LINE(INFO, "User denied injector update")
				}
				if (updater.latestUpdateInfo.requiresUpdate)
				{
					LOG_LINE(INFO, "BakkesMod update available, version " << updater.latestUpdateInfo.newestTrainerVersion)
					QMessageBox msgBox;

					std::stringstream ss;
					ss << "An update is available: " << std::endl;
					ss << updater.latestUpdateInfo.updateMessage << std::endl;
					ss << "Would you like to update?";
					msgBox.setText(ss.str().c_str());
					msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
					msgBox.setDefaultButton(QMessageBox::Yes);
					int ret = msgBox.exec();
					if (ret == QMessageBox::Yes)
					{
						LOG_LINE(INFO, "User accepted update, downloading now...")
						SetState(UPDATING_BAKKESMOD);
						LOG_LINE(INFO, "Constructing update downloader...")
						updateDownloader = new UpdateDownloader(updater.latestUpdateInfo.downloadUrl, "bmupdate.zip");
						LOG_LINE(INFO, "Telling downloader to download...")
						updateDownloader->StartDownload();
						LOG_LINE(INFO, "Told update downloader to download...")
						ui.progressBar->setMaximum(100);
						return;
					}
					else
					{
						LOG_LINE(INFO, "User cancelled update")
						SetState(CHECK_D3D9);
						QMessageBox msgBox2;
						std::stringstream ss;
						msgBox2.setText("Update cancelled, mod might not work now though");
						msgBox2.setStandardButtons( QMessageBox::Ok);
						msgBox2.setDefaultButton(QMessageBox::Ok);
						int ret = msgBox2.exec();
						LOG_LINE(INFO, "Shown message about mod not working")
					}
				}
				else
				{
					LOG_LINE(INFO, "No update available, switching to IDLE status")
					SetState(CHECK_D3D9);
				}
			}
			
			if (safeModeEnabled && !installation.IsSafeToInject(updater.latestUpdateInfo)) //Check if out of date
			{
				SetState(OUT_OF_DATE_SAFEMODE_ENABLED);
				LOG_LINE(INFO, "BakkesMod does not support newest update yet, still stuck on old version " << updater.latestUpdateInfo.buildID);
			}

		}
		else {

		}
	}
		break;
	case OUT_OF_DATE_SAFEMODE_ENABLED:
	{
		static int outOfDateCounter = 0;
		outOfDateCounter++;
		timer.setInterval(1000);
		if (outOfDateCounter > 120)
		{
			LOG_LINE(INFO, "Checking if BakkesMod has updated yet")
			updater.latestUpdateInfo = UpdateStatus();
			SetState(BOOTING);
			outOfDateCounter = 0;
		}
		if (!safeModeEnabled)
		{
			LOG_LINE(INFO, "User disabled safe mode whilst mod is out of date, going to IDLE state")
			SetState(BAKKESMOD_IDLE);
		}

	}
		break;
	case UPDATING_INJECTOR:
	{
		if (!updateDownloader)
			return;
		if (updateDownloader->completed)
		{
			LOG_LINE(INFO, "Injector update download complete")
			ui.progressBar->hide();
			auto currentName = windowsUtils.GetCurrentExecutablePath();
			if (rename(WindowsUtils::WStringToString(currentName).c_str(), "bakkesmod_old.exe") == 0)
			{
				LOG_LINE(INFO, "Successfully renamed current executable to bakkesmod_old.exe");
				rename(updateDownloader->packageUrl.c_str(), WindowsUtils::WStringToString(currentName).c_str());
				system(WindowsUtils::WStringToString(currentName).c_str());
				QCoreApplication::quit();
			}
			else
			{
				LOG_LINE(INFO, "Unable to rename current executable to bakkesmod_old.exe, displaying error to user");
				QMessageBox msgBox;
				msgBox.setText("Could not update BakkesMod injector, no permissions to rename file, sorry!");
				msgBox.setStandardButtons(QMessageBox::Ok);
				msgBox.setDefaultButton(QMessageBox::Ok);
				int ret = msgBox.exec();
				SetState(BAKKESMOD_IDLE);
			}
		}
		else {
			ui.progressBar->show();
			ui.progressBar->setValue(updateDownloader->percentComplete);

		}
	}
	break;
	case UPDATING_BAKKESMOD:
		if (!updateDownloader)
			return;
		if (updateDownloader->completed)
		{
			LOG_LINE(INFO, "Downloaded BakkesMod update file")
			ui.progressBar->hide();
			SetState(BAKKESMOD_INSTALLING);
		}
		else {
			ui.progressBar->show();
			LOG_LINE(INFO, "Update progress: " << updateDownloader->percentComplete << "%")
			ui.progressBar->setValue(updateDownloader->percentComplete);

		}
	break;
	case BAKKESMOD_INSTALLING:
	{
		Installer i(updateDownloader->packageUrl, installation.GetBakkesModFolder());
		i.Install();
		
		std::ofstream out(installation.GetBakkesModFolder() + "injectorversion.txt");
		out << BAKKESMODINJECTOR_VERSION;
		out.close();
		LOG_LINE(INFO, "Writing " << BAKKESMODINJECTOR_VERSION << " to injectorversion.txt")
		SetState(CHECK_D3D9);
	}
		break;
	case BAKKESMOD_IDLE:
	{
		timer.setInterval(70);
		static int intervalLoops = 0;
		intervalLoops++;
		if (intervalLoops > 200)
		{
			intervalLoops = 10;
			if (safeModeEnabled && !installation.IsSafeToInject(updater.latestUpdateInfo)) //Check if out of date every X sec
			{
				SetState(OUT_OF_DATE_SAFEMODE_ENABLED);
			}
		}
		else if (dllInjector.GetProcessID(L"RocketLeague.exe"))
		{
			if (safeModeEnabled && !installation.IsSafeToInject(updater.latestUpdateInfo)) //Check if safe to inject right before injecting
			{
				SetState(OUT_OF_DATE_SAFEMODE_ENABLED);
			}
			else
			{
				SetState(INJECT_DLL);
				int timeout = settingsManager.GetIntSetting(L"InjectionTimeout");
				if (timeout == 0)
					timeout = INJECTION_TIMEOUT_DEFAULT;
				timer.setInterval(timeout);
				if (intervalLoops < 10)
				{
					dllInjector.SetInjectionParameters({ BAKKESMODINJECTOR_VERSION, 0 });
				}
				else
				{
					dllInjector.SetInjectionParameters({ BAKKESMODINJECTOR_VERSION, 1 });
				}
			}

		}
	}
		break;
	case INJECT_DLL:
	{
		std::string bmPath = installation.GetBakkesModFolder();
		std::string path = bmPath + "bakkesmod.dll";
		if (!WindowsUtils::FileExists(path))
		{
			QMessageBox msgBox;
			msgBox.setText("Could not find BakkesMod DLL, please try reinstalling (File -> reinstall)");
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.setDefaultButton(QMessageBox::Ok);
			int ret = msgBox.exec();
			SetState(INSTALLATION_CORRUPT);
			return;
		}
		dllInjector.InjectDLL(L"RocketLeague.exe", path);

		DWORD injectionResult = dllInjector.IsBakkesModDllInjected(L"RocketLeague.exe");
		if (injectionResult == NOPE)
		{
			SetState(INJECTION_FAILED);
			QMessageBox msgBox;
			std::stringstream ss;
			ss << "It looks like injection was unsuccessful, this probably means you're missing a dependency. Please take a look at http://bakkesmod.wikia.com/wiki/Troubleshooting" << std::endl;
			ss << "Would you like to open it now?";
			msgBox.setText(ss.str().c_str());
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::Yes);
			int ret = msgBox.exec();
			if (ret == QMessageBox::Yes)
			{

			}
		}
		else
		{
			SetState(INJECTED);
		}
	}
		break;
	case INJECTED:
		timer.setInterval(1000);
		if (!dllInjector.GetProcessID(L"RocketLeague.exe"))
		{
			SetState(BAKKESMOD_IDLE);
		}
		break;
	case CHECK_D3D9:
	{
		if (WindowsUtils::FileExists(installation.GetBakkesModFolder() + "../d3d9.dll"))
		{
			QMessageBox msgBox;
			LOG_LINE(INFO, "found a d3d9.dll")
				std::stringstream ss;
			ss << "WARNING: d3d9.dll detected. This file is used in reshade and might prevent BakkesMod GUI/new console from working. Would you like BakkesMod to remove this file?" << std::endl;
			msgBox.setText(ss.str().c_str());
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::Yes);
			int ret = msgBox.exec();
			if (ret == QMessageBox::Yes)
			{
				if (remove((installation.GetBakkesModFolder() + "../d3d9.dll").c_str()) != 0) 
				{
					QMessageBox msgBox2;
					LOG_LINE(INFO, "Could not remove d3d9.dll")
						std::stringstream ss;
					ss << "Unable to remove file, file already in use? BakkesMod GUI will most likely not work" << std::endl;
					msgBox2.setText(ss.str().c_str());
					msgBox2.setStandardButtons(QMessageBox::Ok);
					msgBox2.setDefaultButton(QMessageBox::Ok);
					int ret = msgBox2.exec();
				}
				else
				{
					QMessageBox msgBox2;
					LOG_LINE(INFO, "d3d9.dll removed")
						std::stringstream ss;
					ss << "Removed d3d9.dll!" << std::endl;
					msgBox2.setText(ss.str().c_str());
					msgBox2.setStandardButtons(QMessageBox::Ok);
					msgBox2.setDefaultButton(QMessageBox::Ok);
					int ret = msgBox2.exec();
				}
			}
		}
		if (!installation.ManifestFileExists() && ui.actionEnable_safe_mode->isChecked())
		{
			QMessageBox msgBox;
			LOG_LINE(INFO, "Could not find manifest file")
				std::stringstream ss;
			ss << "Unable to find manifest file, disabling safe mode" << std::endl << "Warning: might result in instability after a RL update";
			msgBox.setText(ss.str().c_str());
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.setDefaultButton(QMessageBox::Ok);
			int ret = msgBox.exec();
			ui.actionEnable_safe_mode->setChecked(false);
			OnCheckSafeMode();
		}
		SetState(BAKKESMOD_IDLE);
	}
		break;
	}
	std::string sstr = GetStatusString().c_str();
	if(ui.label)
		ui.label->setText(QString(sstr.c_str()));
}

void BakkesModInjectorCpp::OnCheckSafeMode()
{
	bool newStatus = ui.actionEnable_safe_mode->isChecked();
	//ui.actionEnable_safe_mode->setChecked(newStatus);
	settingsManager.SaveSetting(L"EnableSafeMode", (int)newStatus);
	safeModeEnabled = newStatus;
}

void BakkesModInjectorCpp::OnMinimizeOnStart()
{
	bool newStatus = ui.actionMinimize_on_start->isChecked();
	//ui.actionMinimize_on_start->setChecked(newStatus);
	settingsManager.SaveSetting(L"HideOnBoot", (int)newStatus);
}

void BakkesModInjectorCpp::OnHideOnMinimize()
{
	bool newStatus = ui.actionHide_when_minimized->isChecked();
	//ui.actionHide_when_minimized->setChecked(newStatus);
	settingsManager.SaveSetting(L"HideOnMinimize", (int)newStatus);
}

void BakkesModInjectorCpp::OnRunOnStartup()
{
	bool newStatus = ui.actionRun_on_startup->isChecked();
	
	if (newStatus)
	{
		std::wostringstream w;
		w << "\"" << windowsUtils.GetCurrentExecutablePath() << "\"";
		settingsManager.SaveSetting(L"BakkesMod", w.str(), RegisterySettingsManager::REGISTRY_DIR_RUN);
	}
	else
	{
		settingsManager.DeleteSetting(L"BakkesMod", RegisterySettingsManager::REGISTRY_DIR_RUN);
	}
}

void BakkesModInjectorCpp::OnExitClick()
{
	QApplication::exit();
}

void BakkesModInjectorCpp::OnReinstallClick()
{
	QMessageBox msgBox;
	LOG_LINE(INFO, "User requested reinstall")
	std::stringstream ss;
	ss << "This will remove all existing BakkesMod files, are you sure you want to continue?" << std::endl;
	msgBox.setText(ss.str().c_str());
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::Yes);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes)
	{
		if (WindowsUtils::DeleteDirectory(WindowsUtils::StringToWString(installation.GetBakkesModFolder())))
		{
			LOG_LINE(INFO, "Could not remove BakkesMod folder")
			QMessageBox msgBox2;
			msgBox2.setText("Could not remove BakkesMod folder, no rights or a file is already in use.");
			msgBox2.setStandardButtons(QMessageBox::Ok);
			msgBox2.setDefaultButton(QMessageBox::Ok);
			int ret = msgBox2.exec();
		}
		else 
		{
			LOG_LINE(INFO, "Reinstalling BakkesMod")
			updater.latestUpdateInfo = UpdateStatus();
			settingsManager.DeleteSetting(L"BakkesModPath", RegisterySettingsManager::REGISTRY_DIR_APPPATH);

			SetState(BOOTING);
		}
	}
	else {
		LOG_LINE(INFO, "User cancelled reinstall")
	}
}

void BakkesModInjectorCpp::OnPythonInstallClick()
{
	QMessageBox msgBox;

	std::stringstream ss;
	ss << "Not supported yet." << std::endl;
	msgBox.setText(ss.str().c_str());
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	int ret = msgBox.exec();
}

void BakkesModInjectorCpp::OpenTwitter()
{
	OpenWebsite("https://twitter.com/bakkesmod");
}

void BakkesModInjectorCpp::OpenDiscord()
{
	OpenWebsite("https://discord.gg/HsM6kAR");
}

void BakkesModInjectorCpp::OpenWebsite()
{
	OpenWebsite("http://bakkesmod.com");
}

void BakkesModInjectorCpp::OpenTroubleshootPage()
{
	
	OpenWebsite("http://bakkesmod.wikia.com/wiki/Troubleshooting");
}


void BakkesModInjectorCpp::trayClicked(QSystemTrayIcon::ActivationReason e)
{
	if (e == QSystemTrayIcon::Trigger) {
		if (this->isVisible()) 
			this->hide();
		else {
			setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
			raise();  // for MacOS
			activateWindow(); // for Windows
			this->show();
		}
	}
}

void BakkesModInjectorCpp::TrayOpenAction()
{
	setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
	raise();
	activateWindow();
	this->show();
}

void BakkesModInjectorCpp::TrayCloseAction()
{
	QApplication::exit();
}
#include <qfiledialog.h>
void BakkesModInjectorCpp::OnSelectBakkesModFolder()
{
	QString path = QFileDialog::getOpenFileName(this,
		QString("Select the Rocket League executable"), QString(QDir::currentPath()), QString("RocketLeague.exe (RocketLeague.exe)"));
	std::string rlPath = path.toStdString().substr(0, path.size() - std::string("RocketLeague.exe").size());
	auto converted = windowsUtils.StringToWString(rlPath + "bakkesmod\\");
	settingsManager.SaveSetting(L"BakkesModPath", converted, RegisterySettingsManager::REGISTRY_DIR_APPPATH);
}
#include <qinputdialog.h>
void BakkesModInjectorCpp::OnSetInjectionTimeout()
{
	bool ok;
	int timeout = settingsManager.GetIntSetting(L"InjectionTimeout");
	if (timeout == 0)
	{
		timeout = INJECTION_TIMEOUT_DEFAULT;
	}
	QString text = QInputDialog::getText(this, "Set injection timeout",
		QString(std::string("Set the injection timeout (" + std::to_string(INJECTION_TIMEOUT_DEFAULT) + " = default). Set it to a higher value if you're experiencing crashes during launch.").c_str()), 
		QLineEdit::Normal, QString(std::to_string(timeout).c_str()), &ok);
	if (ok && !text.isEmpty()) {
		std::string test = text.toStdString();
		int intval = std::stoi(test);

		ui.actionSet_injection_timeout->setText(QString(std::string("Set injection timeout (" + std::to_string(intval) + ")").c_str()));
		settingsManager.SaveSetting(L"InjectionTimeout", intval);
	}
}

void BakkesModInjectorCpp::ReleaseDLL()
{
	BakkesModInstallation::overrideBakkesModFolder = "F:\\Bakkesmod\\development\\BakkesMod-rewrite\\Release\\";
	QMessageBox msgBox;
	msgBox.setText("Changed to release folder");
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	int ret = msgBox.exec();
}

void BakkesModInjectorCpp::DebugDLL()
{
	BakkesModInstallation::overrideBakkesModFolder = "F:\\Bakkesmod\\development\\BakkesMod-rewrite\\Debug\\";
	QMessageBox msgBox;
	msgBox.setText("Changed to debug folder");
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	int ret = msgBox.exec();
}

void BakkesModInjectorCpp::OnOpenBakkesModFolderClicked()
{
	std::string rlPath = installation.GetBakkesModFolder();
	if (rlPath.empty())
	{
		QMessageBox msgBox;
		msgBox.setText("Could not find the BakkesMod folder");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();
		return;
	}
	//else
	//{
	//	QMessageBox msgBox;
	//	msgBox.setText("Should open " + QString(rlPath.c_str()));
	//	msgBox.setStandardButtons(QMessageBox::Ok);
	//	msgBox.setDefaultButton(QMessageBox::Ok);
	//	int ret = msgBox.exec();
	//}
	windowsUtils.OpenFolder(rlPath);

}

const std::string GetStateName(BakkesModStatus state)
{
	 #define X(state, name) name,
	static std::string state_names[] =
	{
		BAKKESMOD_STATES
	};
	#undef X
	return state_names[state];
}
