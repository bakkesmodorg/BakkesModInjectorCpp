#include "stdafx.h"
#include "UpdateDownloader.h"
#include <QtCore/qfile.h>
#include "windowsutils.h"
#include <cstdio>
#include <QtWidgets/qmessagebox.h>
#include <windows.h>
#include <future>
#include <thread>

float UpdateDownloader::percentComplete = 0.f;
int ProgressCallback(double dltotal, double dlnow, double ultotal, double ulnow)
{
	UpdateDownloader::percentComplete = (float)dlnow / (float)dltotal * 100.f;
	//LOG_LINE(INFO, "[UpdateDownloader] Update progress " << UpdateDownloader::percentComplete)
	return CURLE_OK;
}

size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void* f)
{
	FILE* file = (FILE*)f;
	return fwrite(ptr, size, nmemb, file);
};

std::future<std::wstring> downloadZip(std::string url, std::filesystem::path download_folder, std::wstring outUrl)
{
	return std::async(std::launch::async, [](std::string url, std::filesystem::path download_folder, std::wstring outUrl) mutable
		{
			try
			{

				curlpp::Cleanup cleaner;
				curlpp::Easy request;

				/// Set the writer callback to enable cURL to write result in a memory area
				curlpp::options::WriteFunctionCurlFunction
					myFunction(WriteCallback);

				curlpp::options::ProgressFunction
					myProgFunction(ProgressCallback);
				
				const std::filesystem::path filename = download_folder / outUrl;// download_folder / (random_string(12) + ".zip");
				LOG_LINE(INFO, "Downloading to " << filename.string())
				if (CreateDirectoryW(download_folder.c_str(), NULL) ||
					ERROR_ALREADY_EXISTS == GetLastError())
				{


					FILE* file = stdout;

					file = _wfopen(filename.c_str(), L"wb");
					if (file == NULL)
					{
						fprintf(stderr, "%s/n", strerror(errno));
						return std::wstring(L"ERROR: ") + WindowsUtils::StringToWString(strerror(errno));
					}


					curlpp::OptionTrait<void*, CURLOPT_WRITEDATA>
						myData(file);


					std::list<std::string> header;
					header.push_back(std::string("BakkesMod Updater CPP "));
					request.setOpt(new curlpp::options::HttpHeader(header));
					request.setOpt(new curlpp::options::FollowLocation(true));
					request.setOpt(new curlpp::options::SslVerifyHost(false));
					request.setOpt(new curlpp::options::SslVerifyPeer(false));
					request.setOpt(new curlpp::options::NoProgress(false));
					request.setOpt(myFunction);
					request.setOpt(myProgFunction);
					request.setOpt(myData);


					request.setOpt(new curlpp::options::Url(url));
					//request.setOpt(new curlpp::options::Verbose(true));
					request.perform();
					fclose(file);
					return filename.wstring();
				}
				else
				{
					return std::wstring(L"ERROR: Dont have access to ") + download_folder.wstring();
				}
			}

			catch (curlpp::LogicError& e)
			{
				std::cout << e.what() << std::endl;
				return std::wstring(L"ERROR1: ") + WindowsUtils::StringToWString((std::string(e.what())));
			}

			catch (curlpp::RuntimeError& e)
			{
				std::cout << e.what() << std::endl;
				std::string err = std::string(e.what());
				return std::wstring(L"ERROR2: ") + WindowsUtils::StringToWString(err);
			}
			catch (...) {}
			return std::wstring(L"ERROR: idk");
		}, url, download_folder, outUrl);
}


UpdateDownloader::UpdateDownloader(std::string downloadUrl, std::string _packageUrl)
{
	LOG_LINE(INFO, "Initiating download of " << _packageUrl);
	std::wstring TempPath;
	wchar_t wcharPath[MAX_PATH];
	if (GetTempPathW(MAX_PATH, wcharPath))
		TempPath = wcharPath;

	//downloadUrl =  "https://github.com/bakkesmodorg/BakkesModInjectorCpp/releases/download/1.3.1/bakkesmod_131.zip";

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
	//networkManager = new QNetworkAccessManager(this);
	//connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
	percentComplete = 0.f;
	LOG_LINE(INFO, "UpdateDownloader constructed ");
}


UpdateDownloader::~UpdateDownloader()
{
}

void UpdateDownloader::StartDownload()
{
	LOG_LINE(INFO, "Starting download of " << updateUrl);

	std::thread t([this]()
		{
			wchar_t tmp_path[MAX_PATH];
			GetTempPathW(MAX_PATH, tmp_path);
			std::filesystem::path download_folder = std::filesystem::path(tmp_path);


			std::shared_ptr<std::future<std::wstring>> future = std::make_shared< std::future<std::wstring>>(downloadZip(updateUrl, download_folder, packageUrl));
			std::future_status status;
			do
			{
				
				status = future->wait_for(std::chrono::seconds(1));
				if (status == std::future_status::deferred)
				{
					std::cout << "deferred\n";
				}
				else if (status == std::future_status::timeout)
				{
					std::cout << "timeout\n";
				}
				else if (status == std::future_status::ready)
				{
					std::cout << "ready!\n";
				}
			} while (status != std::future_status::ready);
				
				//while (fut->wait_for(std::chrono::seconds(60)) == std::future_status::ready);
				LOG_LINE(INFO, "Update download completed")
					std::wstring final_url = future->get();
				LOG_LINE(INFO, "Final url" << WindowsUtils::WStringToString(final_url));
				if (!WindowsUtils::FileExists(final_url))
				{
					LOG_LINE(INFO, "Doesnt exist?")
					QMessageBox msgBox;
					msgBox.setText("Error downloading update package. The update server might be offline, please try again later!");
					msgBox.show();
					int ret = msgBox.exec();
				}

				this->completed = true;
			});
	t.detach();

	/*QUrl url(QString(updateUrl.c_str()));
	auto qnr = QNetworkRequest(url);

	qnr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

	auto reply = networkManager->get(qnr);
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(updateProgress(qint64, qint64)));
	connect(reply, SIGNAL(redirected(const QUrl&)),
		this, SLOT(redirected(const QUrl&)));*/
	
}
//
//void UpdateDownloader::redirected(const QUrl& url)
//{
//	//LOG_LINE(INFO, "Redirected to " << url.toString().toStdString());
//}


//void UpdateDownloader::updateProgress(qint64 read, qint64 total)
//{
//	percentComplete = (float)read / (float)total * 100.f;
//	LOG_LINE(INFO, "[UpdateDownloader] Update progress " << percentComplete)
//}
//
//void UpdateDownloader::finishedSlot(QNetworkReply *reply)
//{
//	
//	//LOG_LINE(INFO, "Supports SSL: " << QSslSocket::supportsSsl());
//	//LOG_LINE(INFO, reply->raw)
//	LOG_LINE(INFO, "Update download completed")
//	if (reply->error() == QNetworkReply::NoError) {
//		if (WindowsUtils::FileExists(packageUrl))
//		{
//			if (DeleteFile(packageUrl.c_str()) != 0)
//			{
//				LOG(INFO, "Unable to delete " << packageUrl);
//				QMessageBox msgBox;
//				msgBox.setText("Could not delete update package, probably need admin privileges. I might crash now.");
//				msgBox.show();
//			}
//		}
//		QFile other(QString::fromStdWString(packageUrl.c_str()));
//		other.open(QIODevice::WriteOnly);
//		other.write(reply->readAll());
//		other.flush();
//		other.close();
//	}
//	else
//	{
//		LOG(INFO, "Error downloading update! " << reply->error());
//	}
//	/*completed = true;
//	disconnect(reply, SIGNAL(downloadProgress(qint64, qint64)),
//		this, SLOT(updateProgress(qint64, qint64)));
//	reply->deleteLater();*/
//}