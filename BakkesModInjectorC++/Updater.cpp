#include "Updater.h"
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QObject>
#include "BakkesModInjectorCpp.h"

//http://updater.bakkesmod.com/static/versions/bakkesmod_57.zip
std::string Updater::UPDATE_SERVER_URL = "https://updater.bakkesmod.com/updater/";// "http://149.210.150.107/updater/"; //


Updater::Updater()
{
	networkManager = new QNetworkAccessManager(this);
	connect(networkManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(OnUpdateInfoReceived(QNetworkReply*)));
	connect(networkManager, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(networkError(QNetworkReply::NetworkError)));
	connect(networkManager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)), this, SLOT(accessibleChanged(QNetworkAccessManager::NetworkAccessibility)));

}


Updater::~Updater()
{
}

bool Updater::checkForNetworkChange()
{
	bool isAccessibleNow = networkManager->networkAccessible() == QNetworkAccessManager::NetworkAccessibility::Accessible;

	if (isAccessibleNow && !networkAccessible)
	{
		networkAccessible = true;
		return true;
	}
	bool result = accessibilityChanged;
	accessibilityChanged = false;
	return result;
}

void Updater::accessibleChanged(QNetworkAccessManager::NetworkAccessibility* access)
{
	LOG_LINE(INFO, "Network accessibility changed")
	accessibilityChanged = true;
}

void Updater::CheckForUpdates(int version)
{
	networkAccessible = networkManager->networkAccessible() == QNetworkAccessManager::NetworkAccessibility::Accessible;
	lastVersionChecked = version;

	std::string fullRequestUrl = UPDATE_SERVER_URL + std::to_string(version);// +"/";
	latestUpdateInfo.networkRequestStatus = REQUESTED;
	latestUpdateInfo.requestFinished = false;
	QNetworkRequest networkRequest(QUrl(fullRequestUrl.c_str()));
	networkRequest.setHeader(QNetworkRequest::UserAgentHeader, std::string("BakkesMod Updater CPP (" + std::to_string(BAKKESMODINJECTOR_VERSION) + ")").c_str());
	networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

	auto reply = networkManager->get(networkRequest);
	reply->setProperty("version", version);
	
}

void Updater::SetEnableBeta(bool enable)
{
	LOG_LINE(INFO, "Set beta status to: " << (int)enable);
	enableBeta = true;
}

void Updater::replyFinished(QNetworkReply * result)
{
	
}

void Updater::networkError(QNetworkReply::NetworkError code)
{
	LOG_LINE(INFO, "Error" << (int)code);
	
	if (useHostname) {
		latestUpdateInfo.requestFinished = true;
		latestUpdateInfo.networkRequestStatus = FINISHED_ERROR;
	}

	//Some (German?) ISPs don't like the bakkesmod dowmain for some reason so we'd have to use a http fallback with direct IP :/
	UPDATE_SERVER_URL = "http://149.210.150.107/updater/";
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
	LOG_LINE(INFO, "Got data from server: " << std::string(latestUpdateInfo.jsonData));
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
			latestUpdateInfo.injectorSetupUrl = test["injectorsetupurl"].toString().toStdString();
		}

		auto gameInfo = rootObj.find("gameinfo");
		if (gameInfo != rootObj.end())
		{
			auto test = gameInfo.value().toObject();
			latestUpdateInfo.buildID = test["buildid"].toString().toStdString();
			std::string buildids = test["buildids"].toString().toStdString();
			latestUpdateInfo.buildIds = split(buildids, ',');
			
			std::string egsbuildids = test["egsbuildids"].toString().toStdString();
			latestUpdateInfo.egsBuildIds = split(egsbuildids, ',');

		}
		if (auto backoff = rootObj.find("backoff_seconds"); backoff != rootObj.end())
		{
			latestUpdateInfo.backoff_seconds = backoff->toInt();
			LOG_LINE(INFO, "Set backoff to " << latestUpdateInfo.backoff_seconds << " seconds");
		}
		if (auto backoff = rootObj.find("backoff_seconds_outofdate"); backoff != rootObj.end())
		{
			latestUpdateInfo.backoff_seconds_outofdate = backoff->toInt();
			LOG_LINE(INFO, "Set backoff (out of date) to " << latestUpdateInfo.backoff_seconds << " seconds");
		}
		
		if (auto tmp = rootObj.find("allow_beta"); tmp != rootObj.end())
		{
			latestUpdateInfo.allowBetaAccess = tmp->toBool();
		}


		auto updateInfo = rootObj.find("update_info");
		if (enableBeta)
		{
			if (auto tmp = rootObj.find("update_info_beta"); tmp != rootObj.end())
			{
				updateInfo = tmp;
			}
		}
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
				LOG_LINE(INFO, "Set update url to " << latestUpdateInfo.downloadUrl);
			}
		}
		latestUpdateInfo.requiresUpdate = version < latestUpdateInfo.newestTrainerVersion;
		latestUpdateInfo.networkRequestStatus = FINISHED_SUCCESS;
	}
	else {
		if (useHostname) //Already using hostname
		{
			LOG_LINE(INFO, "Two fails");
			latestUpdateInfo.requestFinished = true;
			latestUpdateInfo.networkRequestStatus = FINISHED_ERROR;

			result->deleteLater();
			return;
		}
		LOG_LINE(INFO, "Checking again but with hostname this time");
		UPDATE_SERVER_URL = "http://updater.bakkesmod.com/updater/";
		useHostname = true;

		CheckForUpdates(lastVersionChecked);
		return;
	}

	latestUpdateInfo.requestFinished = true;
	LOG_LINE(INFO, "Allow network result to be freed");
	result->deleteLater();
}
