#pragma once
#include <qobject.h>
#include <qnetworkaccessmanager.h>
class UpdateDownloader : public QObject
{
	Q_OBJECT

private slots:
	void updateProgress(qint64 read, qint64 total);
public:
	QNetworkAccessManager* networkManager;
	std::string updateUrl;
	std::string packageUrl;
	float percentComplete = 0;
	bool completed = false;
	UpdateDownloader(std::string downloadUrl, std::string packageUrl);
	~UpdateDownloader();
	void StartDownload();
public slots:
	void finishedSlot(QNetworkReply *reply);
};

