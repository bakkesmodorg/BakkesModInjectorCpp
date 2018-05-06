#pragma once
#include <string>
#include <QtNetwork>
#include "BakkesModInstallation.h"
class Updater
{
private:
	static const std::string UPDATE_SERVER_URL;
	QNetworkAccessManager networkManager;
public:
	Updater();
	~Updater();
	void CheckForUpdates(int version);
public slots:
	void OnUpdateInfoReceived(QNetworkReply* result);
};

