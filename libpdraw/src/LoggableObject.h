#pragma once
#include "boost/shared_ptr.hpp"
#include "UserSettingsData.h"
#include "StatusLogger.h"

#include "SViewData.h"
//#include "ROIMan.h"

class ROIMan;
class LoggableConfigurableObject
{
public:
    virtual int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID) ;
protected:
    bool _initComplete;
    //this is needed so that each class grabs it's settings from the usersettings
    //each derived class needs to define this themselves.
    virtual void SetUserSettings()=0;

    virtual void WriteToStatusLog(std::string message, bool error, bool toConsoleAlso);
    virtual void WriteToStatusLog(boost::shared_ptr<SViewData> sixdof);
    boost::shared_ptr<UserSettingsData> _userSettingsData;
    boost::shared_ptr<StatusLogger> _statusLogger;
private:
    std::string _classID;


};
