#include "StatusLogger.h"
#include "ErrorCodes.h"//todo: rename to statuscodes
#if defined(__APPLE__)
#include <boost/date_time/local_time/local_time.hpp>
#else
#include </usr/local/include/boost/date_time/local_time/local_time.hpp>
#endif
#include <sstream>
#include <algorithm>
#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"
#include "boost/filesystem.hpp"
#include <sstream>
//#include "ShlObj.h"
//#include "direct.h"
#include <time.h>
#include <sys/stat.h>

int gStatusCode = 0x00;
int settingsFileErrorCode = 0x00;
bool reportedAppFolderError = false;
bool appFolderErrorTriggered = false;
bool settingsFileErrorTriggered = false;
bool reportedSettingsFileError = false;
std::string GetCurrentTimeStamp()
{
	std::stringstream ss;
#if defined(_WIN32)

	SYSTEMTIME st;
	GetLocalTime(&st);
	if(st.wMilliseconds > 99)
	{
		ss<<std::setw(3)<<st.wMilliseconds;
	}
	else if(st.wMilliseconds>9)
	{
		ss<<"0"<<std::setw(2)<<st.wMilliseconds;
	}
	else
	{
		ss<<"00"<<std::setw(1)<<st.wMilliseconds;
	}
#endif
	//std::ostringstream ss;
	std::time_t seconds;
	struct tm  timeinfo;
	time(&seconds);
#if defined(_WIN32)
	localtime_s(&timeinfo, &seconds);
#endif

    ////////////test code DGD
    /// timeval curTime;
//    gettimeofday(&curTime, NULL);
//    int milli = curTime.tv_usec / 1000;

//    char buffer [80];
//    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));

//    char currentTime[84] = "";
//    sprintf(currentTime, "%s:%d", buffer, milli);
//    printf("current time: %s \n", currentTime);

    long            ms; // Milliseconds
        time_t          s;  // Seconds
        struct timespec spec;

        clock_gettime(CLOCK_REALTIME, &spec);
        char buffer2[80];
        s  = spec.tv_sec;
        int milli = spec.tv_nsec / 1000000;
        strftime(buffer2, 80, "%Y-%m-%d %H:%M:%S", localtime(&spec.tv_sec));
        char currentTime[84] = "";
        sprintf(currentTime, "%s:%03d", buffer2, milli);
        std::string timestamp(currentTime);
        //printf("current time: %s \n", currentTime);

        //ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
        //if (ms > 999) {
        //    s++;
        //    ms = 0;
       // }

        //printf("Current time: %"PRIdMAX".%03ld seconds since the Epoch\n",
          //     (intmax_t)s, ms);
    /// /////////////////
//	std::time_t now = std::time(NULL);
//	std::tm* ptm = std::localtime(&now);
//	char buffer[80];
//	strftime(buffer, 80, "%H:%M:%S:", &timeinfo);
//	size_t sz = strlen(buffer);
//	std::string timestamp(buffer, sz);
//	timestamp.append(ss.str());
	return timestamp;
}
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#if defined(__APPLE__)
#include "objcUtilityWrapper.h"
std::string GetDataDirectory()
{
    //LFObjCFunctionsCPPWrapper wrapper;
    //GetDataFolderPath();
    return GetDataFolderPath();
}
#else
std::string GetHomePath(){
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    std::string strHomeDir(homedir);
    return strHomeDir;
}

std::string GetAppDataPath(){
    std::string scfolder(GetHomePath());
    scfolder.append("/.smartcamjet");
    return scfolder;
}

std::string GetIconFolderPath()
{
    std::string iconFolder(GetAppDataPath());
    iconFolder.append("/icons");
    return iconFolder;
}
#endif

struct stat sb;

//bool FileExists (const std::string& name) {
//  struct stat buffer;
//  return (stat (name.c_str(), &buffer) == 0);
//}
std::string AppDataDir()
{
#if defined(__APPLE__)
    
    std::string scfolder(GetBundleFolderPath());
#else
    std::string scfolder(GetAppDataPath());
#endif
    const char* appDataPath = scfolder.c_str();
    if (stat(appDataPath, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        //...it is a directory...
        std::cout<<"";
    }
    else{
        if (boost::filesystem::create_directory(appDataPath))
        {
            //attempt to create the directory
        }
        else{
            //problems creating the appdata path
            gStatusCode = FAILEDCREATEHOMEFOLDER;
#if defined(__APPLE__)
#else
            std::cout<<"Error: could not create the folder"<<scfolder<<std::endl;
            std::cout<<"  verify that the folder "<<GetHomePath()<<" exists and that you have write permissions"<<std::endl;
#endif
            appFolderErrorTriggered = true;
        }
    }
    std::string strPath(appDataPath);
    return strPath;
}
void StatusLogger::CloseCurrentLogFile()
{
    if(_doLogging && _logType == LTSIXDOFIOS)//only create 1 log file on startup, if ios, let the user control the open/close by start/stop
    {
        if(_logFile.is_open())
        {
            _logFile.close();
        }
    }
}
void StatusLogger::OpenLogFile()//LET THE USER CONTROL OPEN/CLOSE OF LOGFILE WITH THE START/STOP LOGGING
{
    if(_doLogging && _logType == LTSIXDOFIOS)//only create 1 log file on startup, if ios, let the user control the open/close by start/stop
    {
        _logFile.open(GetLogFilename().c_str());
        if(_logFile.is_open())
        {
            _logFile<<"opened log file successfully"<<std::endl;
            if(_logType == LTSIXDOFIOS){
                _logFile<<"vehlat, vehlon, vehaltitude, vehpitch, vehheading, vehroll, campitch, camheading, camroll, fov, dFovVerticalAngle, dFovHorizontalAngle, dTargetLat, dTargetLon, dTargetAlt, dSlantDistance"
                <<std::endl;
            }
        }
    }
}
std::string StatusLogger::GetLogFilename()
{
    //
#if defined(__APPLE__)
    sprintf(_fullpath, GetDataDirectory().c_str());
#else
    sprintf(_fullpath, AppDataDir().c_str());
#endif
	//SHFILEINFO shFileInfo;

	//boost::filesystem::path dir(fullpath);
	boost::filesystem::path dir(AppDataDir().c_str());
	//if(boost::filesystem::is_directory(dir))
	//if (SHGetFileInfo((LPCSTR)fullpath, 0, &shFileInfo, sizeof(SHFILEINFO), SHGFI_TYPENAME) != 0)
	//{
		// Then file or folder exists so we can do something here
	///}
	//else
	//{
		// File or folder does not exist
		//create the directory
		//char filen[256];
		//sprintf(filen, AppDataDir().c_str());
		//boost::filesystem::create_directory(filen);
		//CreateDirectory(filen, NULL);
	//}
	std::string filename("/LFClient-log");
	filename.append(GetTimestampFilename());
	filename.append(".txt");
	strcat(_fullpath, filename.c_str());
	return _fullpath;
}
int StatusLogger::Init(boost::shared_ptr<UserSettingsData> usettingsdata)
{
	//check to see if there is an entry in usettingsdata for logging off/on

	if(usettingsdata->_stringSettings.find("LOGGING") == usettingsdata->_stringSettings.end() )
	{
		//didn't find logging setting, lets assume not to do logging then
		_doLogging = false;
	}
	else
	{
		//lets see if its set to ON
		std::string islogging(usettingsdata->_stringSettings["LOGGING"]);
		if(islogging.compare("ON") == 0)
		{
			_doLogging = true;
		}
		//true can also work
		if(islogging.compare("TRUE") == 0)
		{
			_doLogging = true;
		}
        std::string logType(usettingsdata->_stringSettings["LOGTYPE"]);
        if(logType.compare("SIXDOF")==0)
        {
            _logType = LTSIXDOF;
        }
        if(logType.compare("SIXDOFIOS")==0)
        {
            _logType = LTSIXDOFIOS;
        }
        else if(logType.compare("VERBOSE")==0)
        {
            _logType = LTVERBOSE;
        }
        else
        {
            _logType = LTSTANDARD;
        }
	}

	if(_doLogging && _logType != LTSIXDOFIOS)//only create 1 log file on startup, if ios, let the user control the open/close by start/stop
	{
		_logFile.open(GetLogFilename().c_str());
		if(_logFile.is_open())
		{
			_logFile<<"opened log file successfully"<<std::endl;
            if(_logType == LTSIXDOF){
                _logFile<<"vehlat, vehlon, vehaltitude, vehpitch, vehheading, vehroll, campitch, camheading, camroll, fov, dFovVerticalAngle, dFovHorizontalAngle, dTargetLat, dTargetLon, dTargetAlt, dSlantDistance"
                       <<std::endl;
            }
		}
	}
	return 1;
}//auto creates name with timestamp

int StatusLogger::Shutdown()
{
    //cout<<"statuslogger shutdown called"<<endl;
	if(_logFile.is_open())
	{
		_logFile.close();
	}
	return 0;
}
std::string StatusLogger::GetTimestampFilename()
{
	boost::posix_time::ptime  now = boost::posix_time::second_clock::local_time();
	boost::posix_time::ptime  utc = boost::posix_time::second_clock::universal_time();
	boost::posix_time::time_duration tz_offset = (now - utc);
	std::stringstream   ss;
	ss << tz_offset;
	boost::local_time::time_zone_ptr    zone(new boost::local_time::posix_time_zone(ss.str().c_str()));
	boost::local_time::local_date_time  ldt = boost::local_time::local_sec_clock::local_time(zone);
	ss.str("");
	ss << ldt;
	std::size_t location = 0;
	std::string format(ss.str());
	std::replace( format.begin(), format.end(), ':', '-'); // replace all 'x' to 'y'
	std::replace( format.begin(), format.end(), ' ', '-'); // replace all 'x' to 'y'
	return format;
}

std::string StatusLogger::ConvertCodeToMessage(int statusCode, bool& writeToConsole)
{
    writeToConsole = false;
	switch(statusCode)
	{
        case FAILEDCREATEHOMEFOLDER :
            writeToConsole = true;
            return "Failed to create home folder";
            break;
		case SUCCESS:
			return "Success";
			break;
		case SERIALCONNECTIONSUCCEEDED:
			return "serial connection succeeded";
			break;
		case SERIALCONNECTIONFAILED:
			return "serial connection failed";
			break;
		case SERIALOPENERROR:
			return "serial open failed";
			break;
		case SERIALBAUDERROR:
			return "serial baud setting error";
			break;
		case SERIALFLOWERROR:
			return "serial flow control setting error";
			break;
		case SERIALPARITYERROR:
			return "serial parity setting error";
			break;
		case SERIALSTOPBITERROR:
			return "serial stop bit setting error";
			break;
		case SERIALCHARSIZERROR:
			return "serial char size setting error";
			break;
		case VIDEOINITSUCCEEDED:
			return "video interface init succeeded";
			break;
		case VIDEONODECKLINKFOUND:
			return "video interface no decklink card found";
			break;
		case VIDEOSETCALLBACKFAILED:
			return "video interface video frame callback register failed";
			break;
		case VIDEOSTARTSTREAMSFAILED:
			return "video interface start video stream failed";
			break;
		case VIDEOENABLEINPUTFAILED:
			return "video interface enable video input failed";
			break;
		case VIDEOINPUTCAUGHTEXCEPTION:
			return "exception caught while trying to init video input";
			break;
		case HUDSHUTDOWNSUCCEED:
			return "shut down hud";
			break;
		case VIDEOINTERFACESTOPPED:
			return "stopped video interface";
			break;
		case SERIALTIMEOUT:
			return "timed out while waiting for serial message";
			break;
		case FAILOPENSETTINGSFILE:
			return "failed to open settings.ini file";
			break;
		case SERIALNOCOMINSETTINGS :
			return "no comport settings found in settings.ini file";
			break;
		case SERIALNOBAUDINSETTINGS :
			return "no baud rate settings found in settings.ini file";
			break;
		case SERIALNOBSIZEINSETTINGS :
			return "no byte size settings found in settings.ini file";
			break;
		case SERIALNOPARITYINSETTINGS  :
			return "no parity settings found in settings.ini file";
			break;
		case SERIALNOSTBITSINSETTINGS :
			return "no stopbits settings found in settings.ini file";
			break;
		case VIDEONOCOCREATE :
			return "couldn't cocreate instance of video player";
			break;
		case VIDEOCOMINITFAILED :
			return "video com initialization failed";
			break;
		case OPENSETTINGSFILESUCCEEDED :
			return "successfully opened settings file";
			break;
		case DECKLINKDISCOVERYFAIL:
			return "no decklink devices discovered";
			break;
		case HUDINITIALIZED:
			return "HUD initialized";
			break;
		case SERIALCLOSED:
			return "Serial connection closed";
			break;
		default:
			return "undefined status code";
	}
}
std::string StatusLogger::CreateSixdofString(boost::shared_ptr<SViewData> viewData){
    std::string msg;
    {
        std::stringstream ss;
        ss<<viewData->dVehicleLat;
        msg.append(ss.str());
    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dVehicleLon;
        msg.append(ss.str());
    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dVehicleAltitude;
        msg.append(ss.str());
    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dVehiclePitch;
        msg.append(ss.str());
    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dVehicleHeading;
        msg.append(ss.str());
    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dVehicleRoll;
        msg.append(ss.str());
    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dCameraPitch;
        msg.append(ss.str());
    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dCameraHeading;
        msg.append(ss.str());
    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dCameraRoll;
        msg.append(ss.str());
    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dfov;
        msg.append(ss.str());

    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dFovVerticalAngle;
        msg.append(ss.str());

    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dFovHorizontalAngle;
        msg.append(ss.str());

    }
    //stare point data
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dTargetLat;
        msg.append(ss.str());

    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dTargetLon;
        msg.append(ss.str());

    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dTargetAlt;
        msg.append(ss.str());

    }
    msg.append(",");
    {
        std::stringstream ss;
        ss<<viewData->dSlantDistance;
        msg.append(ss.str());

    }
    return msg;

}

void StatusLogger::AddStatus(boost::shared_ptr<SViewData> viewData){
    //only output to file with sixdof
    if(_doLogging && (_logType == LTSIXDOF || _logType == LTVERBOSE))
    {
        if(_logFile.is_open())
        {
            std::string msg(GetCurrentTimeStamp());
            msg.append(" - ");
            msg.append(CreateSixdofString(viewData));
            _logFile<<msg<<std::endl;
        }
    }
#if defined(__APPLE__)
    else if(_doLogging && _logType == LTSIXDOFIOS)
    {
        if(_logFile.is_open())
        {
            std::string timestamp = GetSixdofTimeStamp();
            //atti = [NSString stringWithFormat:@"%@ %.11f, %.14f, %f, %f, %f, %f, %f, %f, %f, %llu \r\n",timeStamp, _lat, _lon, (_altAGL+_HomeAlt), //_camPitch, _camYaw, _camRoll, _hFOV, _vFOV, _altAGL, _dts];
            std::stringstream strStream;
            strStream<<timestamp<<", "<<viewData->dVehicleLat<<", "<<viewData->dVehicleLon<<", "<<viewData->dVehicleAltitude<<", "<<viewData->dCameraPitch<<", "<<viewData->dCameraHeading<<", "<<viewData->dCameraRoll<<", "<<viewData->dFovHorizontalAngle<<", "<<viewData->dFovVerticalAngle<<", "<<viewData->dVehicldAltitudeAGL<<", "<<viewData->dts;
            //std::string msg(GetCurrentTimeStamp());
            //msg.append(" - ");
            //msg.append(CreateSixdofString(viewData));
            //TODO: CREATE AN IOS FLIGHT DATA LOGGING MESSAGE SAME AS SMARTCAM VIEW, LOOK IN DJICAMERAVIEWCONTROLLER
            _logFile<<strStream.str()<<std::endl;
        }
    }
#endif
}

int StatusLogger::AddStatus(std::string statusMsg, bool writeToConsole)
{
    if(writeToConsole){
        std::cout<<statusMsg<<std::endl;
    }
    if(_doLogging && (_logType == LTSTANDARD || _logType == LTVERBOSE))
	{
		if(_logFile.is_open())
		{
			std::string msg(GetCurrentTimeStamp());
			msg.append(" - ");
			msg.append(statusMsg);
			_logFile<<msg<<std::endl;
		}
	}
	return 0;
}

int StatusLogger::AddStatus(int statusCode)
{


//    int gStatusCode = 0x00;
//    int settingsFileErrorCode = 0x00;
//    bool reportedAppFolderError = false;
//    bool appFolderErrorTriggered = false;
//    bool settingsFileErrorTriggered = false;
//    bool reportedSettingsFileError = false;

    if(settingsFileErrorTriggered == true && reportedSettingsFileError == false)
    {
        bool testVal = false;//just a dummy variable
         std::string folderMessage(ConvertCodeToMessage(settingsFileErrorCode, testVal));
         std::cout<<folderMessage<<std::endl;
        reportedSettingsFileError = true;
    }
    if(appFolderErrorTriggered == true && reportedAppFolderError == false)
    {
        bool testVal = false;//just a dummy variable
         std::string folderMessage(ConvertCodeToMessage(gStatusCode, testVal));
         std::cout<<folderMessage<<std::endl;
        reportedAppFolderError = true;
    }
    bool writeToConsole = false;
    std::string message(ConvertCodeToMessage(statusCode, writeToConsole));
    if(writeToConsole){
        std::cout<<message<<std::endl;
    }
    if(_doLogging && _logType == LTSTANDARD)
    {
		if(_logFile.is_open())
		{
			_logFile<<message<<std::endl;
		}
	}
	return 0;
}
