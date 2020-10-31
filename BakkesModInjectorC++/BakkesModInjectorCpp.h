#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BakkesModInjectorCpp.h"
#include "WindowsUtils.h"
#include "DllInjector.h"
#include <QtCore/qtimer.h>
#include "BakkesModInstallation.h"
#include "SettingsManager.h"
#include "Updater.h"
#include <QtWidgets/qsystemtrayicon.h>

#define RL_PROCESS_NAME L"RocketLeague.exe"
#define BAKKESMODINJECTOR_VERSION 16

#define BAKKESMOD_STATES \
	X(BOOTING, "BOOTING") \
	X(BAKKESMOD_IDLE, "BAKKESMOD_IDLE") \
	X(WAITING_FOR_RL, "WAITING_FOR_RL") \
	X(OUT_OF_DATE, "OUT_OF_DATE") \
	X(OUT_OF_DATE_SAFEMODE_ENABLED, "OUT_OF_DATE_SAFEMODE_ENABLED") \
	X(CHECKING_FOR_UPDATES, "CHECKING_FOR_UPDATES") \
	X(START_UPDATING, "START_UPDATING") \
	X(UPDATING_BAKKESMOD, "UPDATING_BAKKESMOD") \
	X(UPDATING_INJECTOR, "UPDATING_INJECTOR") \
	X(INJECT_DLL, "INJECT_DLL") \
	X(INJECTED, "INJECTED") \
	X(INJECTION_FAILED, "INJECTION_FAILED") \
	X(BAKKESMOD_INSTALLING, "BAKKESMOD_INSTALLING") \
	X(REINSTALL, "REINSTALL") \
	X(INSTALLATION_CORRUPT, "INSTALLATION_CORRUPT") \
	X(CHECK_D3D9, "CHECKING_D3D9") \
	X(WAIT_FOR_RL_CLOSE, "WAIT_FOR_RL_CLOSE")

#define X(state, name) state,
enum BakkesModStatus : size_t
{
	BAKKESMOD_STATES
};
#undef X

const std::string GetStateName(BakkesModStatus state);

class BakkesModInjectorCpp : public QMainWindow
{
	Q_OBJECT
private:
	bool safeModeEnabled = false;
	BakkesModStatus bakkesModState = BOOTING;
	BakkesModInstallation installation;
	Updater updater;
	RegisterySettingsManager settingsManager;
	WindowsUtils windowsUtils;
	DllInjector dllInjector;
	QTimer timer;
	QSystemTrayIcon* trayIcon;
	int outOfDateCounter = 0;
	std::string bakkesModIdleString = "Uninjected, waiting for user to start Rocket League";
public:
	BakkesModInjectorCpp(QWidget *parent = Q_NULLPTR);
	void initialize();
	std::string GetStatusString();
	void changeEvent(QEvent* e);
	void OpenWebsite(std::string url);
	void SetState(BakkesModStatus newState);

	bool PopupRLRunningTillClosed();

	public slots:
	void OnOpenBakkesModFolderClicked();
	void OnCheckInjection();
	void TimerTimeout();
	void OnCheckSafeMode();
	void OnMinimizeOnStart();
	void OnHideOnMinimize();
	void OnRunOnStartup();
	void OnDisableWarnings();
	void OnExitClick();
	void OnReinstallClick();
	void OnPythonInstallClick();
	void OnPatreonClick();
	void OnBetaChannelClick();

	void OpenTwitter();
	void OpenDiscord();
	void OpenWebsite();
	void OpenTroubleshootPage();
	void trayClicked(QSystemTrayIcon::ActivationReason e);

	void TrayOpenAction();
	void TrayCloseAction();

	void OnSelectBakkesModFolder();
	void OnCheckForUpdates();
	void OnSetInjectionTimeout();

	void ReleaseDLL();
	void DebugDLL();

private:
	Ui::BakkesModInjectorCppClass ui;
};
