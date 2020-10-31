#pragma once
#include "logger.h"
#define LOG_LINE(sev, msg) LOG(sev) << msg << std::endl;

enum NetworkRequestStatus
{
	IDLE = 0,
	REQUESTED = 1,
	FINISHED_SUCCESS = 2,
	FINISHED_ERROR = 3
};

struct UpdateStatus
{
	NetworkRequestStatus networkRequestStatus = IDLE;
	bool requestFinished = false;
	bool requiresUpdate = false;

	std::string jsonData = "";

	std::string injectorUrl = "";
	std::string injectorVersion = "0";
	std::string buildID = "";

	int newestTrainerVersion = 0;
	std::string rocketLeagueVersion = "";
	std::string publicationDate = "";
	std::string updateMessage = "";
	std::string downloadUrl = "";
	int backoff_seconds = 60 * 5;
	int backoff_seconds_outofdate = 60 * 2;
	std::vector<std::string> buildIds;
	std::vector<std::string> egsBuildIds;
	bool updateRequest = false;
};