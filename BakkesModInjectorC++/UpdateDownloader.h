#pragma once
#include <QtCore/qobject.h>


#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "libssl_static.lib")
#pragma comment(lib, "libcrypto_static.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "curlpp.lib")
//#include "curlpp/curlpp.cpp"
#include "curlpp/cURLpp.hpp"
#include "curlpp/Easy.hpp"
#include "curlpp/Options.hpp"
#include "curlpp/Exception.hpp"


class UpdateDownloader : public QObject
{
	Q_OBJECT

	//	private slots:
	//void updateProgress(qint64 read, qint64 total);
public:
	static float percentComplete;
	//QNetworkAccessManager* networkManager;
	std::string updateUrl;
	std::wstring packageUrl;
	//float percentComplete = 0;
	bool completed = false;
	UpdateDownloader(std::string downloadUrl, std::string packageUrl);
	~UpdateDownloader();
	void StartDownload();
	/*public slots:
	void finishedSlot(QNetworkReply *reply);
	void redirected(const QUrl& url);*/
};
