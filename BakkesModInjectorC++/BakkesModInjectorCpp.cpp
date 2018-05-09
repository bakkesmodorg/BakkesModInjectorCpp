#include "BakkesModInjectorCpp.h"
#include "qmessagebox.h"
#include <sstream>
#include "updatedownloader.h"
#include <tlhelp32.h>

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
	ui.actionEnable_safe_mode->setChecked(settingsManager.GetIntSetting(L"EnableSafeMode"));
	ui.actionHide_when_minimized->setChecked(settingsManager.GetIntSetting(L"HideOnMinimize"));

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
	case OUT_OF_DATE_SAFEMODE_DISABLED:
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
						updateDownloader = new UpdateDownloader(updater.latestUpdateInfo.downloadUrl, "update.zip");
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

		}
		else {

		}
	}
		break;
	case UPDATING_BAKKESMOD:
		if (updateDownloader->completed)
		{
			ui.progressBar->hide();
			bakkesModState = BAKKESMOD_IDLE;
		}
		else {
			ui.progressBar->show();
			ui.progressBar->setValue(updateDownloader->percentComplete);

		}
	break;
	case BAKKESMOD_IDLE:
		timer.setInterval(200);
		if (dllInjector.GetProcessID(L"RocketLeague.exe"))
		{
			bakkesModState = INJECT_DLL;
		}
		break;
	case INJECT_DLL:
	{
		std::string rlPath = windowsUtils.GetRocketLeagueDirFromLog();
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
	std::string rlPath = windowsUtils.GetRocketLeagueDirFromLog();
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
