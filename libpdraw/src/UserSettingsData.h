#pragma once
#include <map>
#include <iostream>
using namespace std;
/**
 * stores settings from settings file that each class can grab whatever contents it desires
 */
class UserSettingsData{
 public:
        std::map<std::string, std::string> _stringSettings;///< settings from setting.ini file that are strings
        std::map<std::string, int> _intSettings;///< settings from setting.ini file that are ints
        std::map<std::string, double> _doubleSettings;///< settings from setting.ini file that are doubles

};
