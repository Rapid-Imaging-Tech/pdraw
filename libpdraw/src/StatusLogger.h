#pragma once
#include <string>
#include <fstream>
#include "boost/shared_ptr.hpp"
#include "UserSettingsData.h"
#include "SViewData.h"
enum LOGTYPE{LTSTANDARD, LTSIXDOF, LTSIXDOFIOS, LTVERBOSE};
class StatusLogger
{
public:
    void CloseCurrentLogFile();
    void OpenLogFile();
	int Shutdown();
    StatusLogger():
        _doLogging(false),
        _logType(LTSTANDARD)
    {};
	~StatusLogger(){};
	//int Init(std::string filename){return 1;};
	int Init(boost::shared_ptr<UserSettingsData> usettingsdata);
	int AddStatus(int statusCode);
    int AddStatus(std::string statusMsg, bool writeToConsole);
    void AddStatus(boost::shared_ptr<SViewData> viewData);
    //void CloseCurrentLogFile();
private:
    std::string CreateSixdofString(boost::shared_ptr<SViewData> viewData);
    LOGTYPE _logType;
	bool _doLogging;
	std::string GetLogFilename();

	char _fullpath[256];
	std::string GetTimestampFilename();
	std::ofstream _logFile;
    std::string ConvertCodeToMessage(int statusCode, bool& writeToConsole);
};
