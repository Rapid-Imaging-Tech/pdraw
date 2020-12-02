#include <iostream>
#include <fstream>
#include <cctype>
#if defined(__APPLE__)
#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/foreach.hpp"
#include "boost/shared_ptr.hpp"
#include "objcUtilityWrapper.h"
#else
#include </usr/local/include/boost/algorithm/string/classification.hpp>
#include </usr/local/include/boost/algorithm/string/split.hpp>
#include </usr/local/include/boost/foreach.hpp>
#include </usr/local/include/boost/shared_ptr.hpp>
#endif
#include <vector>
#include "UserSettings.h"
#include "StringTrimmingRoutines.h"
#include "ErrorCodes.h"
#include "boost/filesystem.hpp"
#include "utilities.h"
extern std::string AppDataDir();
//extern bool FileExists (const std::string& name);
extern bool settingsFileErrorTriggered ;
extern int settingsFileErrorCode;
using namespace std;

void TextFileSettings::Tokenize(std::string& str, boost::shared_ptr<UserSettingsData> data)
{
    std::vector<std::string> strVec = TokenizeStringBySubstring(str, " ");
    
    if(strVec.size() != 2)
    {
        //we have a problem, we need a key and value
        //just ignore
    }
    else
    {
        char* pEnd = 0x00;
        int val = strtol(strVec[1].c_str(), &pEnd, 10);
        if((val == 0 && strVec[1].compare("0") != 0) ||  strVec[0].compare("KMLURL") == 0
           //||  strVec[0].compare("KMLPORT") == 0
           //||strVec[0].compare("ELEVMGRHOST")==0
           //|| strVec[0].compare("VECTILEURL") == 0
           //||  strVec[0].compare("VECTILEPORT") == 0
           || strVec[0].compare("TILESERVERIP") == 0
           ||  strVec[0].compare("TILESERVERPORT") == 0)
            ///TILESERVERIP 192.168.0.39
            //TILESERVERPORT 8080
        {
            data->_stringSettings[strVec[0]] = strVec[1];
            
        }
        else
        {
            data->_intSettings[strVec[0]] = val;
        }
    }
}
int TextFileSettings::WriteInitialSettingsFile(std::string filePath)
{
    //create settings file at filePath location
    //write out default values
    ofstream outputFile;
    outputFile.open(filePath.c_str());
    settingsFileErrorCode = 0x00;
    if(outputFile.fail()){
        settingsFileErrorTriggered = true;
        settingsFileErrorCode = FAILEDCREATESETTINGSFILE;
        std::cout<<"Error: could not create initial settings.ini file"<<std::endl;
        //std::string folder;
        std::cout<<"  verify that the folder "<<AppDataDir()<<" exists and that it has write permissions"<<std::endl;
        return settingsFileErrorCode;
    }
    else{
        std::cout<<"Msg:  created initial settings file at "<<filePath<<std::endl;
        
        //TODOSETTINGS: UPDATE THIS TO REFLECT MAPS.SMARTCAM3D.COM AS SERVER ETC.
        //when adding new settings, make sure 2 elements get added with a space between
        //ie, each non-# line gets placed into a map of either <string, string> <string, int> <string, bool>
        
        
        //////////////////
        
        
        
        outputFile<<"#if videofile in, sc expects to see a ts file with embedded klve\n";
        outputFile<<"#otherwise, we go to normal FLIR camera mode\n";
        outputFile<<"#VIDEOFILE /mapdata/DowntownABQ_2_2014_06_18_07_33_46.ts\n";
        outputFile<<"#TILESERVERSTYLE /styles/klokantech-basic/\n";
        outputFile<<"#TILESERVERSTYLE /styles/osm-bright/\n";
        outputFile<<"TILESERVERSTYLE /styles/rapid-gl-style/\n";
        outputFile<<"CDTFOLDER /mapdata/CDTfixed/\n";
        outputFile<<"#PNGFOLDER /mapdata/tilepngs/\n";
        outputFile<<"#TILESERVERIP 192.168.0.39\n";
        outputFile<<"#http://ec2-34-221-147-27.us-west-2.compute.amazonaws.com:8080\n";
        outputFile<<"#TILESERVERIP ec2-34-221-147-27.us-west-2.compute.amazonaws.com\n";
        outputFile<<"#TILESERVERIP maps.smartcam3d.com\n";
        outputFile<<"TILESERVERIP 35.202.49.60\n";
        outputFile<<"#TILESERVERIP 35.226.0.190\n";
        outputFile<<"TILESERVERPORT 8080\n";
        outputFile<<"RETINALEVEL 3\n";
        outputFile<<"TEXTFONT arial.png\n";
        outputFile<<"TEXTFONTJSON arial.json\n";
        outputFile<<"MAXRENDERTILES 8\n";
        outputFile<<"#textscale / 1000 move dec 3 places left\n";
        outputFile<<"TEXTSCALE 7000\n";
        outputFile<<"LOGGING ON\n";
        outputFile<<"LOGTYPE SIXDOFIOS\n";
        outputFile<<"STLABELNOSHOW 11000\n";
        outputFile<<"#STLABELCUTOFF1 4300\n";
        outputFile<<"STLABELCUTOFF1 120\n";
        outputFile<<"#STLABELCUTOFF2 2500\n";
        outputFile<<"STLABELCUTOFF2 60\n";
        outputFile<<"#STLABELCUTOFF3 2000\n";
        outputFile<<"STLABELCUTOFF3 30\n";
        outputFile<<"#STLABELCUTOFF4 1000\n";
        outputFile<<"STLABELCUTOFF4 15\n";
        outputFile<<"STLABELSCALE1 800\n";
        outputFile<<"STLABELSCALE2 400\n";
        outputFile<<"STLABELSCALE3 200\n";
        outputFile<<"STLABELSCALE4 100\n";
        outputFile<<"STLABELSCALE5 50\n";
        outputFile<<"STCOLORMINOR WHITE\n";
        outputFile<<"STCOLORSERVICE WHITE\n";
        outputFile<<"STCOLORSECONDARY YELLOW\n";
        outputFile<<"STCOLORTERTIARY WHITE\n";
        outputFile<<"STCOLORRAIL WHITE\n";
        outputFile<<"STCOLORTRUNK YELLOW\n";
        outputFile<<"STCOLORPRIMARY YELLOW\n";
        outputFile<<"STCOLORMOTORWAY ORANGE\n";
//        outputFile<<"STLABELCUTOFF1 4300\n";
//        outputFile<<"STLABELCUTOFF2 2500\n";
//        outputFile<<"STLABELCUTOFF3 2000\n";
//        outputFile<<"STLABELCUTOFF4 1000\n";
//        outputFile<<"STLABELSCALE1 1750\n";
//        outputFile<<"STLABELSCALE2 1500\n";
//        outputFile<<"STLABELSCALE3 1000\n";
//        outputFile<<"STLABELSCALE4 750\n";
//        outputFile<<"STLABELSCALE5 500\n";
        outputFile<<"#30.102434, -93.473751 Louisianna\n";
        outputFile<<"#STARTLOCATIONLAT 30102434\n";
        outputFile<<"#STARTLOCATIONLON -93473751\n";
        outputFile<<"#28.501893, -93.408859 gulf of Mexico\n";
        outputFile<<"#STARTLOCATIONLAT 28501893\n";
        outputFile<<"#STARTLOCATIONLON -93408859\n";
        outputFile<<"#44.244725, -93.467456Minneapolis\n";
        outputFile<<"#STARTLOCATIONLAT 44244725\n";
        outputFile<<"#STARTLOCATIONLON -93467456\n";
        outputFile<<"#DaveD house\n";
        outputFile<<"#STARTLOCATIONLAT 46685425\n";
        outputFile<<"#STARTLOCATIONLON -92360508\n";
        outputFile<<"#DuluthMN\n";
        outputFile<<"#STARTLOCATIONLAT 46705954\n";
        outputFile<<"#STARTLOCATIONLON -92112293\n";
        outputFile<<"#Willsonville\n";
        outputFile<<"#STARTLOCATIONLAT 45321012\n";
        outputFile<<"#STARTLOCATIONLON -122770417\n";
        outputFile<<"#Stafford OR\n";
        outputFile<<"#STARTLOCATIONLAT 45345000\n";
        outputFile<<"#STARTLOCATIONLON -122722700\n";
        outputFile<<"#DAVE G HOUSE\n";
        outputFile<<"#STARTLOCATIONLAT 43008851\n";
        outputFile<<"#STARTLOCATIONLON -89778113\n";
        outputFile<<"#STARTLOCATIONALT 1000\n";
        outputFile<<"#DELTALAT 3\n";
        outputFile<<"#DELTALAT 0\n";
        outputFile<<"#DELTALON 0\n";
        outputFile<<"PLACECITY 1\n";
        outputFile<<"PLACETOWN 1\n";
        outputFile<<"PLACEVILLAGE 1\n";
        outputFile<<"PLACEHAMLET 1\n";
        outputFile<<"PLACESUBURB 1\n";
        outputFile<<"PLACENEIGHBOURHOOD 1\n";
        outputFile<<"PLACEISLAND 1\n";
        outputFile<<"PLACEISLET 1\n";
        outputFile<<"PLACEARCHIPELAGO 1\n";
        outputFile<<"PLACERESIDENTIAL 1\n";
        outputFile<<"PLACEABORIGINAL_LANDS 1\n";
        outputFile<<"PMAIRFIELD 0\n";
        outputFile<<"PMALCOHOLSHOP 0\n";
        outputFile<<"PMAMUSEMENTPARK 0\n";
        outputFile<<"PMAQUARIUM 0\n";
        outputFile<<"PMARTGALLERY 0\n";
        outputFile<<"PMATTRACTION 0\n";
        outputFile<<"PMBAKERY 0\n";
        outputFile<<"PMBANK 0\n";
        outputFile<<"PMBAR 0\n";
        outputFile<<"PMBEER 0\n";
        outputFile<<"PMBICYCLE 0\n";
        outputFile<<"PMBICYCLESHARE 0\n";
        outputFile<<"PMBUS 0\n";
        outputFile<<"PMCAFE 0\n";
        outputFile<<"PMCAR 0\n";
        outputFile<<"PMCAMPSITE 0\n";
        outputFile<<"PMCASTLE 0\n";
        outputFile<<"PMCEMETERY 0\n";
        outputFile<<"PMCINEMA 0\n";
        outputFile<<"PMCLOTHINGSTORE 0\n";
        outputFile<<"PMCOLLEGE 0\n";
        outputFile<<"PMDENTIST 0\n";
        outputFile<<"PMDOGPARK 0\n";
        outputFile<<"PMDRINKINGWATER 0\n";
        outputFile<<"PMEMBASSY 0\n";
        outputFile<<"PMENTRANCE 0\n";
        outputFile<<"PMFASTFOOD 0\n";
        outputFile<<"PMFERRY 0\n";
        outputFile<<"PMFUEL 0\n";
        outputFile<<"PMGARDEN 0\n";
        outputFile<<"PMGROCERY 0\n";
        outputFile<<"PMHARBOR 0\n";
        outputFile<<"PMHELIPORT 0\n";
        outputFile<<"PMICECREAM 0\n";
        outputFile<<"PMINFORMATION 0\n";
        outputFile<<"PMLAUNDRY 0\n";
        outputFile<<"PMLODGING 0\n";
        outputFile<<"PMMONUMENT 0\n";
        outputFile<<"PMMUSEUM 0\n";
        outputFile<<"PMMUSIC 0\n";
        outputFile<<"PMPARK 0\n";
        outputFile<<"PMPHARMACY 0\n";
        outputFile<<"PMPICNICSITE 0\n";
        outputFile<<"PMPLACEOFWORSHIP 0\n";
        outputFile<<"PMPLAYGROUND 0\n";
        outputFile<<"PMPOLICE 0\n";
        outputFile<<"PMPOST 0\n";
        outputFile<<"PMPRISON 0\n";
        outputFile<<"PMRELIGIOUSCHRISTIAN 0\n";
        outputFile<<"PMRELIGIOUSJEWISH 0\n";
        outputFile<<"PMRELIGIOUSMUSLIM 0\n";
        outputFile<<"PMRESTAURANT 0\n";
        outputFile<<"PMROCKET 0\n";
        outputFile<<"PMSCHOOL 0\n";
        outputFile<<"PMSHOP 0\n";
        outputFile<<"PMSTADIUM 0\n";
        outputFile<<"PMSWIMMING 0\n";
        outputFile<<"PMSUITCASE 0\n";
        outputFile<<"PMTHEATRE 0\n";
        outputFile<<"PMTOILET 0\n";
        outputFile<<"PMTOWNHALL 0\n";
        outputFile<<"PMVETERINARY 0\n";
        outputFile<<"PMZOO 0\n";
        outputFile<<"PMDOCTOR 1\n";
        outputFile<<"PMHOSPITAL 1\n";
        outputFile<<"PMLIBRARY 0\n";
        outputFile<<"PMGOLF 0\n";
        outputFile<<"PMFIRE_STATION 1\n";
        outputFile<<"#LOGGING FALSE-NOTIMPLEMENTED\n";
        outputFile<<"#STANDARD|SIXDOF|VERBOSELOGTYPE STANDARD-NOTIMPLEMENTED\n";
        outputFile<<"#STREETALPHA 50-NOTIMPLEMENTED\n";
        outputFile<<"#TEXTSCALE 1000000-NOTIMPLEMENTED\n";
        //
        //        ///////////////////
        //        outputFile<<"TILESERVERSTYLE /styles/klokantech-basic/\n";
        //        outputFile<<"CDTFOLDER /mapdata/CDTfixed/\n";
        //        outputFile<<"PNGFOLDER /mapdata/tilepngs/\n";
        //        outputFile<<"TILESERVERIP 192.168.0.39\n";
        //        outputFile<<"TILESERVERPORT 8080\n";
        //        outputFile<<"RETINALEVEL 3\n";
        //        outputFile<<"TEXTFONT arial.png\n";
        //        outputFile<<"TEXTFONTJSON arial.json\n";
        //        outputFile<<"CLEARCACHEFOLDER TRUE\n";
        //        outputFile<<"MINRASTERZOOMLEVEL 10\n";
        //        outputFile<<"#alpha move decimal pt 3 places left to get actual value\n";
        //        outputFile<<"ALPHA 1000\n";
        //        //outputFile<<"#textscale / 1000 move dec 3 places left\n";
        //        //outputFile<<"TEXTSCALE 7000\n";
        //        outputFile<<"MAXRENDERTILES 9\n";
        //        outputFile<<"LOGGING TRUE\n";
        //        outputFile<<"#STANDARD|SIXDOF|VERBOSELOGTYPE STANDARD-NOTIMPLEMENTED\n";
        //        outputFile<<"LOGTYPE STANDARD\n";
        outputFile.close();
    }
    return SUCCESS;
}

int TextFileSettings::ParseData(boost::shared_ptr<UserSettingsData> data)
{
    using namespace std;
#if defined(__APPLE__)
    LFObjCFunctionsCPPWrapper wrapper;
    std::string settingsFileFolder = wrapper.GetSettingsFilePath();
#else
    std::string settingsFileFolder = AppDataDir();
    settingsFileFolder.append("/settings.ini");
#endif
    //TODOSETTINGS: CHECK IF SETTINGS FILE EXISTS IN THE DOCUMENT DIRECTORY
    // IF NOT THEN CREATE A DEFAULT THERE.
    
    std::string errorMsg;
    if(!FileExists(settingsFileFolder, errorMsg))
    {
        //settings file doesn't exist create it with initial values
        int error = WriteInitialSettingsFile(settingsFileFolder);
        if(error){
            return error;
        }
    }
    
    std::string line;
    std::ifstream configFile;
    configFile.open(settingsFileFolder.c_str());//todo: what if comsettings not available
    
    char outputComsettingsTag[100];
    char outputComsettingsValue[100];
    
    if(!configFile.is_open())
    {
        settingsFileErrorTriggered = true;
        settingsFileErrorCode = FAILOPENSETTINGSFILE;
        std::cout<<"Error: could not open settings.ini file"<<std::endl;
        return FAILOPENSETTINGSFILE;
    }
    else
    {
        while ( std::getline (configFile, line) )
        {
            trim(line);
            if(line.size() > 0)
            {
                if(!(line[0] == '#')) //skip any lines with # at beginning
                    Tokenize(line, data);
            }
        }
        configFile.close();
    }
    settingsFileErrorTriggered = true;
    settingsFileErrorCode = OPENSETTINGSFILESUCCEEDED;
    return SUCCESS;
}
int TextFileSettings::GetSettings(boost::shared_ptr<UserSettingsData> ptrSettingsData)
{
    return ParseData(ptrSettingsData);
}
