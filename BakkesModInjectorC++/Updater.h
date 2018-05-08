#pragma once
#include <string>
#include <QtNetwork>
#include "BakkesModInstallation.h"

enum NetworkRequestStatus
{
	IDLE = 0,
	REQUESTED = 1,
	FINISHED_SUCCESS = 2,
	FINISHED_ERROR = 3
};

struct UpdateStatus
{
	NetworkRequestStatus networkRequestStatus = IDLE;
	bool requestFinished = false;
	bool requiresUpdate = false;

	std::string injectorUrl = "";
	std::string injectorVersion = "0";
	std::string buildID = "";
	
	int newestTrainerVersion = 0;
	std::string rocketLeagueVersion = "";
	std::string publicationDate = "";
	std::string updateMessage = "";
	std::string downloadUrl = "";
	bool updateRequest = false;
};


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

