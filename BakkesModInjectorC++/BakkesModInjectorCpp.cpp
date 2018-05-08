#include "BakkesModInjectorCpp.h"
#include "qmessagebox.h"
BakkesModInjectorCpp::BakkesModInjectorCpp(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(&timer, SIGNAL(timeout()), this, SLOT(TimerTimeout()));

}

void BakkesModInjectorCpp::initialize()
{
	timer.start(500);
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

	switch (bakkesModState)
	{
	case BOOTING:
		if (!installation.IsInstalled())
		{
			
			bakkesModState = CHECKING_FOR_UPDATES;
		}
		break;
	case CHECKING_FOR_UPDATES:
		break;
	case IDLE:
		break;
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
	
	std::string path = rlPath + "BakkesMod/bakkesmod.dll";
	dllInjector.InjectDLL(L"RocketLeague.exe", path);
}
