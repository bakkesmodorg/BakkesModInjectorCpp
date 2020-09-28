#include "Updater.h"
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QObject>
#include "BakkesModInjectorCpp.h"

//http://updater.bakkesmod.com/static/versions/bakkesmod_57.zip
std::string Updater::UPDATE_SERVER_URL = "http://149.210.150.107/updater/"; //


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
	lastVersionChecked = version;
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
	LOG(INFO, Q_FUNC_INFO << "Error" << code);
	
	if (useHostname) {
		latestUpdateInfo.requestFinished = true;
		latestUpdateInfo.networkRequestStatus = FINISHED_ERROR;
	}
	UPDATE_SERVER_URL = "http://updater.bakkesmod.com/updater/";
	useHostname = true;
	CheckForUpdates(lastVersionChecked);
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

void myReplace(std::string& str,
	const std::string& oldStr,
	const std::string& newStr)
{
	std::string::size_type pos = 0u;
	while ((pos = str.find(oldStr, pos)) != std::string::npos) {
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
void Updater::OnUpdateInfoReceived(QNetworkReply* result)
{
	QJsonDocument json = QJsonDocument::fromJson(result->readAll());
	QString strJson(json.toJson(QJsonDocument::Compact));
	latestUpdateInfo.jsonData = strJson.toStdString();
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
			std::string buildids = test["buildids"].toString().toStdString();
			latestUpdateInfo.buildIds = split(buildids, ',');
			
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
			if (useHostname)
			{
				//latestUpdateInfo.downloadUrl = latestUpdateInfo.downloadUrl
				myReplace(latestUpdateInfo.downloadUrl, "149.210.150.107", "updater.bakkesmod.com");
				LOG(INFO, "Set update url to " << latestUpdateInfo.downloadUrl);
			}
		}
		latestUpdateInfo.requiresUpdate = version < latestUpdateInfo.newestTrainerVersion;
		latestUpdateInfo.networkRequestStatus = FINISHED_SUCCESS;
	}
	else {
		if (useHostname) 
		{
			latestUpdateInfo.requestFinished = true;
			latestUpdateInfo.networkRequestStatus = FINISHED_ERROR;
		}
		UPDATE_SERVER_URL = "http://updater.bakkesmod.com/updater/";
		useHostname = true;
		CheckForUpdates(lastVersionChecked);
	}

	latestUpdateInfo.requestFinished = true;
	result->deleteLater();
}
