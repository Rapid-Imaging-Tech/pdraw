#include "UserSettings.h"
using namespace std;

 int HardCodedSettings::GetSettings(boost::shared_ptr<UserSettingsData> ptrSettingsData)
 {
	ptrSettingsData->_stringSettings["KMLURL"] = "maps.smartcam3d.com";
	ptrSettingsData->_stringSettings["KMLPREFIX"] = "/cgi-bin/mapserv?map=/home/map/LandFormKML.map";
	ptrSettingsData->_stringSettings["ELEVMGRHOST"] = "maps.smartcam3d.com";
	ptrSettingsData->_stringSettings["ELEVMGRPATH"] = "/cgi-bin/mapserv?map=/home/map/dae.map";
	//ptrSettingsData->_stringSettings["ICONFOLDER"] = "icons";
	ptrSettingsData->_stringSettings["TEMPFILEDIR"] = "C:\\Temp";
	ptrSettingsData->_stringSettings["LOGGING"] = "ON";

	return 1;
 }
