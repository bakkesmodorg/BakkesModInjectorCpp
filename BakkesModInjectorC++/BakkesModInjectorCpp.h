#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BakkesModInjectorCpp.h"
#include "WindowsUtils.h"
#include "DllInjector.h"
#include "qtimer.h"
#include "BakkesModInstallation.h"

enum BakkesModStatus
{
	BOOTING = 0,
	IDLE = 1, //NO RL RUNNING
	WAITING_FOR_RL = 2,
	OUT_OF_DATE = 3,
	OUT_OF_DATE_SAFEMODE_DISABLED = 4,
	CHECKING_FOR_UPDATES = 5,
	UPDATING = 6,
	
};

class BakkesModInjectorCpp : public QMainWindow
{
	Q_OBJECT
private:
	BakkesModStatus bakkesModState;
	BakkesModInstallation installation;
	WindowsUtils windowsUtils;
	DllInjector dllInjector;
	QTimer timer;
public:
	BakkesModInjectorCpp(QWidget *parent = Q_NULLPTR);
public slots:
	void OnOpenBakkesModFolderClicked();
	void OnCheckInjection();
	void TimerTimeout();
private:
	Ui::BakkesModInjectorCppClass ui;
};
