#include "BakkesModInjectorCpp.h"
#include "qmessagebox.h"
#include <sstream>
#include "updatedownloader.h"
#include <tlhelp32.h>
#include "Installer.h"
BakkesModInjectorCpp::BakkesModInjectorCpp(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(&timer, SIGNAL(timeout()), this, SLOT(TimerTimeout()));

	QIcon icon(":/BakkesModInjectorCpp/mainicon");
	this->setWindowIcon(icon);

	//Create tray icon
	trayIcon = new QSystemTrayIcon(QIcon(":/BakkesModInjectorCpp/mainicon"), this);
	trayIcon->show();

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));


}

void BakkesModInjectorCpp::initialize()
{
	timer.start(500);
	ui.progressBar->hide();
	
	bool hideOnBoot = settingsManager.GetIntSetting(L"HideOnBoot");
	ui.actionMinimize_on_start->setChecked(hideOnBoot);
	if (hideOnBoot)
	{
		this->showMinimized();
	}
	//settingsManager.SaveSetting(L"EnableSafeMode", (int)newStatus);
}

void BakkesModInjectorCpp::changeEvent(QEvent* e)
{
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
				QTimer::singleShot(250, this, SLOT(hide()));
			}
		}
		break;
	}
	default:
		break;
	}

	QMainWindow::changeEvent(e);
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
	static UpdateDownloader* updateDownloader;
	switch (bakkesModState)
	{
	case BOOTING:
	{
		if (!installation.IsInstalled())
		{
			settingsManager.SaveSetting(L"EnableSafeMode", 1);
			settingsManager.SaveSetting(L"HideOnMinimize", 1);
			settingsManager.SaveSetting(L"HideOnBoot", 0);
			settingsManager.SaveSetting(L"BakkesMod", L"-", RegisterySettingsManager::REGISTRY_DIR_RUN);
		}

		ui.actionEnable_safe_mode->setChecked(settingsManager.GetIntSetting(L"EnableSafeMode"));
		safeModeEnabled = ui.actionEnable_safe_mode->isChecked();
		ui.actionHide_when_minimized->setChecked(settingsManager.GetIntSetting(L"HideOnMinimize"));
		ui.actionRun_on_startup->setChecked(!settingsManager.GetStringSetting(L"BakkesMod", RegisterySettingsManager::REGISTRY_DIR_RUN).empty());
		OnRunOnStartup();




		int version = installation.GetVersion();
		updater.CheckForUpdates(version);
		bakkesModState = CHECKING_FOR_UPDATES;
		timer.setInterval(50);
	}
		break;
	case CHECKING_FOR_UPDATES:
	{
		if (updater.latestUpdateInfo.requestFinished)
		{
			if (updater.latestUpdateInfo.networkRequestStatus == FINISHED_ERROR)
			{
				bakkesModState = BAKKESMOD_IDLE;
				QMessageBox msgBox2;
				std::stringstream ss;
				msgBox2.setText("Could not connect to update server, running in offline mode");
				msgBox2.setStandardButtons(QMessageBox::Ok);
				msgBox2.setDefaultButton(QMessageBox::Ok);
				int ret = msgBox2.exec();
			}
			else
			{
				if (std::stoi(updater.latestUpdateInfo.injectorVersion) > BAKKESMODINJECTOR_VERSION)
				{
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
						bakkesModState = UPDATING_INJECTOR;
						updateDownloader = new UpdateDownloader(updater.latestUpdateInfo.injectorUrl, "newinjector.exe");
						updateDownloader->StartDownload();
						ui.progressBar->setMaximum(100);
						return;
					}
				}
				if (updater.latestUpdateInfo.requiresUpdate)
				{
					QMessageBox msgBox;

					std::stringstream ss;
					ss << "An update is available: " << std::endl;
					ss << updater.latestUpdateInfo.updateMessage;
					msgBox.setText(ss.str().c_str());
					msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
					msgBox.setDefaultButton(QMessageBox::Yes);
					int ret = msgBox.exec();
					if (ret == QMessageBox::Yes)
					{
						bakkesModState = UPDATING_BAKKESMOD;
						updateDownloader = new UpdateDownloader(updater.latestUpdateInfo.downloadUrl, "bmupdate.zip");
						updateDownloader->StartDownload();
						ui.progressBar->setMaximum(100);
					}
					else
					{
						bakkesModState = BAKKESMOD_IDLE;
						QMessageBox msgBox2;
						std::stringstream ss;
						msgBox2.setText("Update cancelled, mod might not work now though");
						msgBox2.setStandardButtons( QMessageBox::Ok);
						msgBox2.setDefaultButton(QMessageBox::Ok);
						int ret = msgBox2.exec();

					}
				}
				else
				{
					bakkesModState = BAKKESMOD_IDLE;
				}
			}
			if (safeModeEnabled && !installation.IsSafeToInject(updater.latestUpdateInfo.buildID)) //Check if out of date
			{
				bakkesModState = OUT_OF_DATE_SAFEMODE_ENABLED;
			}
		}
		else {

		}
	}
		break;
	case OUT_OF_DATE_SAFEMODE_ENABLED:
	{
		static bool toggle = false;
		if (!toggle) {
			timer.setInterval(30000); //Check for updates every 60 seconds
			toggle = true;
		}
		else
		{
			updater.latestUpdateInfo = UpdateStatus();
			bakkesModState = BOOTING;
			toggle = false;
		}

	}
		break;
	case UPDATING_INJECTOR:
	{
		if (updateDownloader->completed)
		{
			ui.progressBar->hide();
			auto currentName = windowsUtils.GetCurrentExecutablePath();
			if (rename(WindowsUtils::WStringToString(currentName).c_str(), "bakkesmod_old.exe") == 0)
			{
				rename(updateDownloader->packageUrl.c_str(), WindowsUtils::WStringToString(currentName).c_str());
				system(WindowsUtils::WStringToString(currentName).c_str());
				QCoreApplication::quit();
			}
			else
			{
				QMessageBox msgBox;
				msgBox.setText("Could not update BakkesMod injector, no permissions to rename file, sorry!");
				msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				msgBox.setDefaultButton(QMessageBox::Yes);
				int ret = msgBox.exec();
				if (ret == QMessageBox::Yes)
				{

				}
				bakkesModState = BAKKESMOD_IDLE;
			}
		}
		else {
			ui.progressBar->show();
			ui.progressBar->setValue(updateDownloader->percentComplete);

		}
	}
	break;
	case UPDATING_BAKKESMOD:
		if (updateDownloader->completed)
		{
			ui.progressBar->hide();
			bakkesModState = BAKKESMOD_INSTALLING;
		}
		else {
			ui.progressBar->show();
			ui.progressBar->setValue(updateDownloader->percentComplete);

		}
	break;
	case BAKKESMOD_INSTALLING:
	{
		Installer i(updateDownloader->packageUrl, installation.GetBakkesModFolder());
		i.Install();
		bakkesModState = BAKKESMOD_IDLE;
	}
		break;
	case BAKKESMOD_IDLE:
	{
		timer.setInterval(200);
		static int intervalLoops = 0;
		intervalLoops++;
		if (intervalLoops > 50 && safeModeEnabled && !installation.IsSafeToInject(updater.latestUpdateInfo.buildID)) //Check if out of date every X sec
		{
			bakkesModState = OUT_OF_DATE_SAFEMODE_ENABLED;
			intervalLoops = 0;
		}
		else if (dllInjector.GetProcessID(L"RocketLeague.exe"))
		{
			if (safeModeEnabled && !installation.IsSafeToInject(updater.latestUpdateInfo.buildID)) //Check if safe to inject right before injecting
			{
				bakkesModState = OUT_OF_DATE_SAFEMODE_ENABLED;
			}
			else
			{
				bakkesModState = INJECT_DLL;
			}

		}
	}
		break;
	case INJECT_DLL:
	{
		std::string rlPath = installation.GetBakkesModFolder();
		std::string path = rlPath + "BakkesMod/bakkesmod.dll";
		dllInjector.InjectDLL(L"RocketLeague.exe", path);

		DWORD injectionResult = dllInjector.IsBakkesModDllInjected(L"RocketLeague.exe");
		if (injectionResult == NOPE)
		{
			bakkesModState = INJECTION_FAILED;
			QMessageBox msgBox;
			msgBox.setText("It looks like injection was unsuccessful, this probably means you're missing a dependency. Please take a look at http://bakkesmod.wikia.com/wiki/Troubleshooting");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::Yes);
			int ret = msgBox.exec();
			if (ret == QMessageBox::Yes)
			{

			}
		}
		else
		{
			bakkesModState = INJECTED;
		}
	}
		break;
	case INJECTED:
		timer.setInterval(1000);
		if (!dllInjector.GetProcessID(L"RocketLeague.exe"))
		{
			bakkesModState = BAKKESMOD_IDLE;
		}
		break;
	}
	ui.label->setText(QString(GetStatusString().c_str()));
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


void BakkesModInjectorCpp::trayClicked(QSystemTrayIcon::ActivationReason e)
{
	if (e == QSystemTrayIcon::Trigger) {
		if (this->isVisible()) 
			this->hide();
		else {
			this->show();
		}
	}
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
	

}
