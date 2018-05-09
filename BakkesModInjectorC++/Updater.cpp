#include "Updater.h"
#include "qnetworkrequest.h"
#include <QObject>
#include "BakkesModInjectorCpp.h"

const std::string Updater::UPDATE_SERVER_URL = "http://127.0.0.1:8000/";//"http://149.210.150.107/updater/";


Updater::Updater()
{
	networkManager = new QNetworkAccessManager(this);
	connect(networkManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(OnUpdateInfoReceived(QNetworkReply*)));
	connect(networkManager, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(networkError(QNetworkReply::NetworkError)));
}


Updater::~Updater()
{
}

void Updater::CheckForUpdates(int version)
{
	std::string fullRequestUrl = UPDATE_SERVER_URL + std::to_string(version);// +"/";
	QNetworkRequest networkRequest(QUrl(fullRequestUrl.c_str()));
	networkRequest.setHeader(QNetworkRequest::UserAgentHeader, std::string("BakkesMod Updater CPP (" + std::to_string(BAKKESMODINJECTOR_VERSION) + ")").c_str());
	networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	

	auto reply = networkManager->get(networkRequest);

	reply->setProperty("version", version);
	latestUpdateInfo.networkRequestStatus = REQUESTED;
}

void Updater::replyFinished(QNetworkReply * result)
{
	
}

void Updater::networkError(QNetworkReply::NetworkError code)
{
	qDebug() << Q_FUNC_INFO << "Error" << code;
	latestUpdateInfo.requestFinished = true;
	latestUpdateInfo.networkRequestStatus = FINISHED_ERROR;
	
}

void Updater::OnUpdateInfoReceived(QNetworkReply* result)
{
	QJsonDocument json = QJsonDocument::fromJson(result->readAll());
	QJsonObject rootObj = json.object();
	if (!rootObj.empty())
	{
		int version = result->property("version").toInt();
		auto injectorInfo = rootObj.find("injector");
		if (injectorInfo != rootObj.end())
		{
			auto test = injectorInfo.value().toObject();
			latestUpdateInfo.injectorUrl = test["injectorurl"].toString().toStdString();
			latestUpdateInfo.injectorVersion = test["injectorversion"].toString().toStdString();
		}

		auto gameInfo = rootObj.find("gameinfo");
		if (gameInfo != rootObj.end())
		{
			auto test = gameInfo.value().toObject();
			latestUpdateInfo.buildID = test["buildid"].toString().toStdString();
		}

		auto updateInfo = rootObj.find("update_info");
		if (updateInfo != rootObj.end())
		{
			auto test = updateInfo.value().toObject();
			//std::string ow = test["trainer_version"].toString().toStdString();
			latestUpdateInfo.newestTrainerVersion = test["trainer_version"].toInt();
			latestUpdateInfo.rocketLeagueVersion = test["rocketleague_version"].toString().toStdString();
			latestUpdateInfo.publicationDate = test["pub_date"].toString().toStdString();
			latestUpdateInfo.updateMessage = test["message"].toString().toStdString();
			latestUpdateInfo.downloadUrl = test["download_url"].toString().toStdString();
		}
		latestUpdateInfo.requiresUpdate = version < latestUpdateInfo.newestTrainerVersion;
		latestUpdateInfo.networkRequestStatus = FINISHED_SUCCESS;
	}
	else {
		latestUpdateInfo.networkRequestStatus = FINISHED_ERROR;
	}

	latestUpdateInfo.requestFinished = true;
	result->deleteLater();
}
