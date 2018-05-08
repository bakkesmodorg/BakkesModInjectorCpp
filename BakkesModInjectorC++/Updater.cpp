#include "Updater.h"
#include "qnetworkrequest.h"
#include <QObject>

#define BAKKESMODINJECTOR_VERSION 5

const std::string Updater::UPDATE_SERVER_URL = "http://149.210.150.107/updater/";


Updater::Updater()
{
}


Updater::~Updater()
{
}

void Updater::CheckForUpdates(int version)
{
	std::string fullRequestUrl = UPDATE_SERVER_URL + std::to_string(version);
	QNetworkRequest networkRequest(QUrl(fullRequestUrl.c_str()));
	networkRequest.setHeader(QNetworkRequest::UserAgentHeader, std::string("BakkesMod Updater CPP (" + std::to_string(BAKKESMODINJECTOR_VERSION) + ")").c_str());
	
	
	QObject::connect(&networkManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(OnUpdateInfoReceived(QNetworkReply*)), Qt::QueuedConnection);

}

void Updater::OnUpdateInfoReceived(QNetworkReply* result)
{
	QJsonDocument json = QJsonDocument::fromJson(result->readAll());
	QJsonObject rootObj = json.object();
	if (rootObj.empty())
	{
		auto test = rootObj;
	}
}
