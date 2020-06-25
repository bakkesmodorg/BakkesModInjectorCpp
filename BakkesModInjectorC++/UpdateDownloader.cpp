#include "stdafx.h"
#include "UpdateDownloader.h"
#include <QtCore/qfile.h>
#include <QtNetwork/qnetworkreply.h>
#include "windowsutils.h"
#include <cstdio>
#include <QtWidgets/qmessagebox.h>
#include <windows.h>

UpdateDownloader::UpdateDownloader(std::string downloadUrl, std::string _packageUrl)
{
	LOG_LINE(INFO, "Initiating download of " << _packageUrl);
	std::wstring TempPath;
	wchar_t wcharPath[MAX_PATH];
	if (GetTempPathW(MAX_PATH, wcharPath))
		TempPath = wcharPath;

	this->updateUrl = downloadUrl;
	this->packageUrl = TempPath + WindowsUtils::StringToWString(_packageUrl.c_str());
	if (WindowsUtils::FileExists(packageUrl))
	{
		if (DeleteFile((packageUrl).c_str()) == 0)
		{
			if (GetLastError() != ERROR_FILE_NOT_FOUND)
			{
				QMessageBox msgBox2;
				LOG_LINE(INFO, "Could not remove " << WindowsUtils::WStringToString(packageUrl) << ". ERR: " << GetLastError())
					std::stringstream ss;
				ss << "Unable to remove bmupdate.zip!! Try running as administrator if you aren't already" << std::endl;
				msgBox2.setText(ss.str().c_str());
				msgBox2.setStandardButtons(QMessageBox::Ok);
				msgBox2.setDefaultButton(QMessageBox::Ok);
				int ret = msgBox2.exec();
			}
			else
			{
				LOG_LINE(INFO, "Tried to remove non-existing file " << WindowsUtils::WStringToString(packageUrl) << ". ERR: " << GetLastError())
			}
		}
	}
	networkManager = new QNetworkAccessManager(this);
	connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));

	LOG_LINE(INFO, "UpdateDownloader constructed ");
}


UpdateDownloader::~UpdateDownloader()
{
}

void UpdateDownloader::StartDownload()
{
	LOG_LINE(INFO, "Starting download of " << updateUrl);
	QUrl url(QString(updateUrl.c_str()));
	auto qnr = QNetworkRequest(url);

	qnr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

	auto reply = networkManager->get(qnr);
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(updateProgress(qint64, qint64)));
	
}


void UpdateDownloader::updateProgress(qint64 read, qint64 total)
{
	percentComplete = (float)read / (float)total * 100.f;
	LOG_LINE(INFO, "[UpdateDownloader] Update progress " << percentComplete)
}

void UpdateDownloader::finishedSlot(QNetworkReply *reply)
{
	LOG_LINE(INFO, "Update download completed")
	if (reply->error() == QNetworkReply::NoError) {
		if (WindowsUtils::FileExists(packageUrl))
		{
			if (DeleteFile(packageUrl.c_str()) != 0)
			{
				LOG(INFO, "Unable to delete " << packageUrl);
				QMessageBox msgBox;
				msgBox.setText("Could not delete update package, probably need admin privileges. I might crash now.");
				msgBox.show();
			}
		}
		QFile other(QString::fromStdWString(packageUrl.c_str()));
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
	disconnect(reply, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(updateProgress(qint64, qint64)));
	reply->deleteLater();
}