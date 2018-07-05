#include "stdafx.h"
#include "UpdateDownloader.h"
#include <qfile.h>
#include <qnetworkreply.h>
#include "windowsutils.h"
#include <cstdio>
#include <qmessagebox.h>
#include <windows.h>

UpdateDownloader::UpdateDownloader(std::string downloadUrl, std::string _packageUrl)
{
	LOG_LINE(INFO, "Initiating download of " << _packageUrl);
	std::wstring TempPath;
	wchar_t wcharPath[MAX_PATH];
	if (GetTempPathW(MAX_PATH, wcharPath))
		TempPath = wcharPath;

	this->updateUrl = downloadUrl;
	this->packageUrl = WindowsUtils::WStringToString(TempPath) + _packageUrl;
	if (WindowsUtils::FileExists(packageUrl))
	{
		if (remove((packageUrl).c_str()) != 0)
		{
			QMessageBox msgBox2;
			LOG_LINE(INFO, "Could not remove " << packageUrl)
				std::stringstream ss;
			ss << "Unable to remove bmupdate.zip!! Try running as administrator if you aren't already" << std::endl;
			msgBox2.setText(ss.str().c_str());
			msgBox2.setStandardButtons(QMessageBox::Ok);
			msgBox2.setDefaultButton(QMessageBox::Ok);
			int ret = msgBox2.exec();
		}
	}
	networkManager = new QNetworkAccessManager(this);
	connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
	connect(networkManager, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(updateProgress(qint64, qint64)));
	LOG_LINE(INFO, "UpdateDownloader constructed ");
}


UpdateDownloader::~UpdateDownloader()
{
}

void UpdateDownloader::StartDownload()
{
	LOG_LINE(INFO, "Starting download of " << updateUrl);
	QUrl url(QString(updateUrl.c_str()));
	networkManager->get(QNetworkRequest(url));
}


void UpdateDownloader::updateProgress(qint64 read, qint64 total)
{
	percentComplete = (float)read / (float)total * 100.f;
	LOG_LINE(INFO, "Update progress " << percentComplete)
}

void UpdateDownloader::finishedSlot(QNetworkReply *reply)
{
	LOG_LINE(INFO, "Update download completed")
	if (reply->error() == QNetworkReply::NoError) {
		if (WindowsUtils::FileExists(packageUrl))
		{
			if (remove(packageUrl.c_str()) != 0)
			{
				LOG(INFO, "Unable to delete " << packageUrl);
				QMessageBox msgBox;
				msgBox.setText("Could not delete update package, probably need admin privileges. I might crash now.");
				msgBox.show();
			}
		}
		QFile other(packageUrl.c_str());
		other.open(QIODevice::WriteOnly);
		other.write(reply->readAll());
		other.flush();
		other.close();
	}
	else
	{
		LOG(INFO, "Error downloading update! " << reply->error());
	}
	completed = true;
	reply->deleteLater();
}