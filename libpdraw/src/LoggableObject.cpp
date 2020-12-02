#include "LoggableObject.h"
//#include "Windows.h"
#include <sstream>
int LoggableConfigurableObject::Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID)
{
    _classID.assign(classID);
    _userSettingsData = data;
    _statusLogger = logger;
    //SetUserSettingsReference(_userSettingsData);//user will define this, it grabs settings from settings list, which is different for each class
    SetUserSettings();
    return 0;//success
}

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

void LoggableConfigurableObject::WriteToStatusLog(boost::shared_ptr<SViewData> sixdof){
    if(_statusLogger)
    {
        std::thread::id this_id = std::this_thread::get_id();
        std::stringstream ss;
        ss<<this_id;
        std::string msg("");
        msg.append("THREADID:");
        msg.append(ss.str());
        msg.append(" Class:");
        msg.append(_classID);
        msg.append(": ");
        _statusLogger->AddStatus(sixdof);
    }
}

void LoggableConfigurableObject::WriteToStatusLog(std::string message, bool error, bool toConsoleAlso)
{
    if(_statusLogger)
    {
        std::thread::id this_id = std::this_thread::get_id();
        std::stringstream ss;
        ss<<this_id;
        std::string msg("");
        if(error){
            msg.append("ERROR:  ");
        }
        msg.append("THREADID:");
        msg.append(ss.str());
        msg.append(" Class:");
        msg.append(_classID);
        msg.append(": ");
        msg.append(message);
        _statusLogger->AddStatus(msg, toConsoleAlso);
    }
}


//#include "LoggableObject.h"
////#include "Windows.h"
//#include <sstream>
//int LoggableConfigurableObject::Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID)
//{
//	_classID.assign(classID);
//	_userSettingsData = data;
//	_statusLogger = logger;
//    //SetUserSettingsReference(_userSettingsData);//user will define this, it grabs settings from settings list, which is different for each class
//    SetUserSettings();
//    return 0;//success
//}

//#include <iostream>
//#include <thread>
//#include <chrono>
//#include <mutex>

//void LoggableConfigurableObject::WriteToStatusLog(boost::shared_ptr<SViewData> sixdof){
//    if(_statusLogger)
//    {
//         std::thread::id this_id = std::this_thread::get_id();
//        std::stringstream ss;
//        ss<<this_id;
//        std::string msg("");
//        msg.append("THREADID:");
//        msg.append(ss.str());
//        msg.append(" Class:");
//        msg.append(_classID);
//        msg.append(": ");
//        _statusLogger->AddStatus(sixdof);
//    }
//}

//void LoggableConfigurableObject::WriteToStatusLog(std::string message, bool error, bool toConsoleAlso)
//{
//	if(_statusLogger)
//	{
//        std::thread::id this_id = std::this_thread::get_id();
//		std::stringstream ss;
//        ss<<this_id;
//        std::string msg("");
//        if(error){
//            msg.append("ERROR:  ");
//        }
//        msg.append("THREADID:");
//		msg.append(ss.str());
//		msg.append(" Class:");
//		msg.append(_classID);
//		msg.append(": ");
//		msg.append(message);
//        _statusLogger->AddStatus(msg, toConsoleAlso);
//	}
//}
