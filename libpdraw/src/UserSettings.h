//#include <map>
#include "boost/shared_ptr.hpp"
#include "UserSettingsData.h"


class UserSettings{
 public: 
	  virtual int GetSettings(boost::shared_ptr<UserSettingsData> ptrSettingsData) = 0;
};

class TextFileSettings : public UserSettings
{
private:
	void Tokenize(std::string& str, boost::shared_ptr<UserSettingsData> data);
	int ParseData(boost::shared_ptr<UserSettingsData> data);
    int WriteInitialSettingsFile(std::string filePath);
 public:
	 virtual int GetSettings(boost::shared_ptr<UserSettingsData> ptrSettingsData);
};

class HardCodedSettings : public UserSettings
{
private:
	//void Tokenize(std::string& str, boost::shared_ptr<UserSettingsData> data);
	//int ParseData(boost::shared_ptr<UserSettingsData> data);
 public:
	 virtual int GetSettings(boost::shared_ptr<UserSettingsData> ptrSettingsData);
};

class XMLFileSettings : public UserSettings
{
 public:
	 virtual int GetSettings(boost::shared_ptr<UserSettingsData> ptrSettingsData);
};
