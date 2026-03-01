#include "Updater.h"
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QObject>
#include "BakkesModInjectorCpp.h"
#include <future>
#include <thread>
#include "curlpp/cURLpp.hpp"
#include "curlpp/Easy.hpp"
#include "curlpp/Options.hpp"
#include "curlpp/Exception.hpp"

//http://updater.bakkesmod.com/static/versions/bakkesmod_57.zip
const std::vector<std::string> Updater::UPDATE_SERVER_URLS = {
	"https://updater.bakkesmod.com/updater/", 
	"https://updater2.bakkesmod.com/updater/",
	"http://149.210.150.107/updater/", //Unfortunately we need the IP fallback because some German ISPs don't like the BakkesMod domain
	"http://updater.bakkesmod.com/updater/" //Also a last resort incase ssl failure
};




std::future<std::string> downloadText(std::string url)
{
	return std::async(std::launch::async, [](std::string url) mutable
		{
			try
			{
				LOG_LINE(INFO, "Requesting " << url);
				curlpp::Cleanup cleaner;
				curlpp::Easy request;

				//std::list<std::string> header;
				request.setOpt(new curlpp::options::UserAgent("BakkesMod Updater CPP (" + std::to_string(BAKKESMODINJECTOR_VERSION) + ")"));
				//request.setOpt(new curlpp::options::HttpHeader(header));
				request.setOpt(new curlpp::options::FollowLocation(true));
				//request.setOpt(new curlpp::options::SslVerifyHost(false));
				//request.setOpt(new curlpp::options::SslVerifyPeer(false));
				//request.setOpt(new curlpp::options::NoProgress(false));


				request.setOpt(new curlpp::options::Url(url));
				//request.setOpt(new curlpp::options::Verbose(true));

				std::ostringstream os;
				curlpp::options::WriteStream ws(&os);
				request.setOpt(ws);
				request.perform();

				//os << request;
				LOG_LINE(INFO, "Got: " << os.str());
				return os.str();
			}
			catch (curlpp::LogicError& e)
			{
				std::cout << e.what() << std::endl;
				return std::string("ERROR1: ") + ((std::string(e.what())));
			}

			catch (curlpp::RuntimeError& e)
			{
				std::cout << e.what() << std::endl;
				std::string err = std::string(e.what());
				return std::string("ERROR2: ") + (err);
			}
			catch (...) {}

			return std::string("ERROR: idk");
		}, url);
}


Updater::Updater()
{
	networkManager = new QNetworkAccessManager(this);
	/*connect(networkManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(OnUpdateInfoReceived(QNetworkReply*)));
	connect(networkManager, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(networkError(QNetworkReply::NetworkError)));*/
	connect(networkManager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)), this, SLOT(accessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
	
}


Updater::~Updater()
{
}

std::string Updater::GetUpdaterUrl()
{
	return UPDATE_SERVER_URLS[currentServerUrlIdx % UPDATE_SERVER_URLS.size()];
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

	std::thread t([this, version]()
		{

			std::shared_ptr<std::future<std::string>> future = std::make_shared< std::future<std::string>>(downloadText(GetUpdaterUrl() + std::to_string(version)));
			std::future_status status;
			int timeouts = 0;
			do
			{
				status = future->wait_for(std::chrono::seconds(1));
				timeouts++;
			} while (status != std::future_status::ready && timeouts < 10);

			LOG_LINE(INFO, "Updater check request completed")
				//	std::string final_url = future->get();
				//LOG_LINE(INFO, "Final url" << (final_url));
				if (timeouts >= 9)
				{
					this->OnUpdateInfoReceived("{}");
				}
				else
				{
					this->OnUpdateInfoReceived(future->get());
				}
		});
	t.detach();

	//std::string fullRequestUrl = UPDATE_SERVER_URL + std::to_string(version);// +"/";
	//latestUpdateInfo.networkRequestStatus = REQUESTED;
	//latestUpdateInfo.requestFinished = false;
	//QNetworkRequest networkRequest(QUrl(fullRequestUrl.c_str()));
	//networkRequest.setHeader(QNetworkRequest::UserAgentHeader, std::string("BakkesMod Updater CPP (" + std::to_string(BAKKESMODINJECTOR_VERSION) + ")").c_str());
	//networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

	//auto reply = networkManager->get(networkRequest);
	//reply->setProperty("version", version);
	
}

void Updater::SetEnableBeta(bool enable)
{
	LOG_LINE(INFO, "Set beta status to: " << (int)enable);
	enableBeta = true;
}
//
//void Updater::replyFinished(QNetworkReply * result)
//{
//	
//}
//
//void Updater::networkError(QNetworkReply::NetworkError code)
//{
//	LOG_LINE(INFO, "Error" << (int)code);
//	
//	if (useHostname) {
//		latestUpdateInfo.requestFinished = true;
//		latestUpdateInfo.networkRequestStatus = FINISHED_ERROR;
//	}
//	UPDATE_SERVER_URL = "http://updater2.bakkesmod.com/updater/";
//	useHostname = true;
//	CheckForUpdates(lastVersionChecked);
//}

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
void Updater::OnUpdateInfoReceived(std::string result)
{
	//Kind of a roundabout way but works (for now...)
	auto qs = QString(result.c_str());
	QJsonDocument json = QJsonDocument::fromJson(qs.toUtf8());
	//QString strJson(json.toJson(QJsonDocument::Compact));
	latestUpdateInfo.jsonData = result;
	LOG_LINE(INFO, "Got data from server: " << std::string(latestUpdateInfo.jsonData));
	//LOG_LINE(INFO, "Test:" << qs.toStdString() << "\n\n" << json.toJson().toStdString());
	QJsonObject rootObj = json.object();

	if (!rootObj.empty())
	{
		int version = lastVersionChecked;
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
			
			latestUpdateInfo.downloadUrl = test["github_download_url"].toString().toStdString();

			//Version 21 and above the download_url and github_download_url will be the same (done serverside), which would make this obsolete(?)
			/*if (useHostname)
			{
				myReplace(latestUpdateInfo.downloadUrl, "149.210.150.107", "updater.bakkesmod.com");
				LOG_LINE(INFO, "Set update url to " << latestUpdateInfo.downloadUrl);
			}*/
		}
		latestUpdateInfo.requiresUpdate = version < latestUpdateInfo.newestTrainerVersion;
		latestUpdateInfo.networkRequestStatus = FINISHED_SUCCESS;
	}
	else {
		currentServerUrlIdx++;
		if (currentServerUrlIdx >= UPDATE_SERVER_URLS.size()) //Already using hostname
		{
			LOG_LINE(INFO, "Every update URL check failed");
			latestUpdateInfo.requestFinished = true;
			latestUpdateInfo.networkRequestStatus = FINISHED_ERROR;
			currentServerUrlIdx = 0;
			//result->deleteLater();
			return;
		}
		
		
		LOG_LINE(INFO, "Checking again on different url (" << GetUpdaterUrl() << ")");
		CheckForUpdates(lastVersionChecked);
		return;
	}

	latestUpdateInfo.requestFinished = true;
	LOG_LINE(INFO, "Allow network result to be freed");
	//result->deleteLater();
}
