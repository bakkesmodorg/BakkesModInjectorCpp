#pragma once
#include "stdafx.h"
#include <string>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/QNetworkReply>
#include "BakkesModInstallation.h"
#include <vector>

class Updater : public QObject
{
	Q_OBJECT
private:
	static const std::vector<std::string> UPDATE_SERVER_URLS;
	int currentServerUrlIdx = 0;
	//bool useHostname = false;
	bool enableBeta = false;
	bool accessibilityChanged = false;
	bool networkAccessible = false;
	int lastVersionChecked = -1;
	QNetworkAccessManager* networkManager;
public:
	UpdateStatus latestUpdateInfo;
	Updater();
	~Updater();
	std::string GetUpdaterUrl();
	void OnUpdateInfoReceived(std::string result);
	bool checkForNetworkChange();
	void accessibleChanged(QNetworkAccessManager::NetworkAccessibility* access);
	void CheckForUpdates(int version);
	void SetEnableBeta(bool enable);
	private slots:
	/*void OnUpdateInfoReceived(QNetworkReply* result);
	void replyFinished(QNetworkReply* result);
	void networkError(QNetworkReply::NetworkError code);*/


};
