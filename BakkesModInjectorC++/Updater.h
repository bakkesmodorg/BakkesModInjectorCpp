#pragma once
#include "stdafx.h"
#include <string>
#include <QtNetwork>
#include "BakkesModInstallation.h"
#include <vector>



class Updater : public QObject
{
	Q_OBJECT
private:
	static const std::string UPDATE_SERVER_URL;
	QNetworkAccessManager* networkManager;
public:
	UpdateStatus latestUpdateInfo;
	Updater();
	~Updater();
	void CheckForUpdates(int version);
private slots:
	void OnUpdateInfoReceived(QNetworkReply* result);
	void replyFinished(QNetworkReply* result);
	void networkError(QNetworkReply::NetworkError code);
};

