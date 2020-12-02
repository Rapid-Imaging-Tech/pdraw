

#include "LFClientEngine.h"
#if defined(__APPLE__)
#define GRIDSIZEMILES 0.5
#else
#include <QtGui/QOpenGLShaderProgram>
#include <GL/gl.h>
#include <GL/glu.h>
#include <qmatrix4x4.h>
#endif


#include "coordinateconverter.h"
//#include "ErrorCodes.h"
//#include "kmlpathdetails.h"
//#include <boost/asio/io_service.hpp>
//#include <boost/bind.hpp>
//#include <boost/thread/thread.hpp>
#include <boost/algorithm/string/replace.hpp>
//#include <videoplayer.h>
#include "boost/shared_array.hpp"
#include "placemarkdatatypes.h"
#include "customkmlfunctions.h"
#include "symbol.h"
#define FADESPEED 1.0 //0.06
#include <math.h>
#include <time.h>
//#include </usr/local/include/boost/thread/thread.hpp>
#include "boost/thread/thread.hpp"
using namespace std;
#include "utilities.h"
#if defined(__APPLE__)
#include "objcUtilityWrapper.h"
extern std::string GetDataDirectory();
GLuint testtextureToBind;
//GLuint testVBO;
GLuint vao = 0;
GLuint gbl_u_SamplerLocation = 0;
GLuint gbl_u_mvpLocation = 0;
GLuint gbl_u_colorLocation = 0;

GLuint gbl_u_mvpLocationAutoScale = 0;
GLuint gbl_u_projLocationAutoScale = 0;
GLuint gbl_u_scaleLocationAutoScale = 0;
GLuint gbl_u_lblDensityLevelLocationAutoScale = 0;
GLuint gbl_u_vehLocationAutoScale = 0;
GLuint gbl_u_targetLocationAutoScale = 0;
GLuint gbl_u_stLabelColorLocationAutoScale = 0;
GLuint gbl_u_SamplerLocationAutoScale = 0;
#else
extern std::string GetIconFolderPath();
#if defined VIDEOPLAYER
Stanag4609SIL sil;
#endif
double RAextCamx = 0.00;
double RAextCamy = 0.00;
double RAextCamz = 0.00;
#endif

std::vector<std::string> _listOfKmlFilesToLoad;


boost::asio::io_service io_service;
boost::thread_group threadgroup;
std::vector<boost::shared_future<int> > pending_data;
boost::asio::io_service::work work(io_service);
#include "boost/thread/mutex.hpp"
boost::mutex glbPlacemarkMutex;
typedef boost::packaged_task<int> packed_task_t;
typedef boost::shared_ptr<packed_task_t> ptask_t;
extern std::vector<std::string> _missingIcons;
#if defined(__APPLE__)
GLuint LFClientEngine::findTextureBasedOnHref(std::string href)
#else
boost::shared_ptr<QOpenGLTexture> LFClientEngine::findTextureBasedOnHref(std::string href)
#endif
{
#if defined(__APPLE__)
    GLuint textureToBind;
#else
    boost::shared_ptr<QOpenGLTexture> textureToBind;
    textureToBind.reset();
#endif
    if ( _symbolTextures->find(href) == _symbolTextures->end() )
    {
        // not found desired symbol
        bool foundRepeat = false;
        for(int jj = 0 ; jj < _missingIcons.size() ; jj++)
        {
            if(_missingIcons[jj].compare(href) == 0)
            {
                foundRepeat = true;
            }
        }
        if(foundRepeat == false)
        {
            
            //show message once
            std::stringstream ss;
            ss<<"could not find icon: ";
            ss<<AppDataDir();
            ss<<"/icons/";
            ss<<href;
            _missingIcons.push_back(href);
            WriteToStatusLog(ss.str(), true, true);
        }
        
        if(_symbolTextures->find("pinred.png") == _symbolTextures->end() ) {
            //didn't find default symbol even
            bool foundRepeat2 = false;
            for(int jj = 0 ; jj < _missingIcons.size() ; jj++)
            {
                if(_missingIcons[jj].compare("pinred.png") == 0)
                {
                    foundRepeat2 = true;
                }
            }
            if(foundRepeat2 == false)
            {
                std::stringstream ss;
                ss<<"could not find icon: ";
                ss<<AppDataDir();
                ss<<"/icons/";
                ss<<"pinred.png";
                _missingIcons.push_back("pinred.png");
                WriteToStatusLog(ss.str(), true, true);
            }
        }
        else{
            textureToBind = (*(_symbolTextures.get()))["pinred.png"];
        }
        
    }
    else
    {
        // found the icon!!
        textureToBind = (*(_symbolTextures.get()))[href];
    }
    
    return textureToBind;
}
void LFClientEngine::SetBillboardStyle(bool val){
    _billboardStyle = val;
}
boost::shared_ptr<iconInfo> LFClientEngine::CreateIconInfo(POIENUM classification, std::string iconFilename)
{
    boost::shared_ptr<iconInfo> icInfo(new iconInfo(classification, iconFilename));
    return icInfo;
}
void LFClientEngine::FillIconMap()
{
    
    (*(_poiIconMap.get()))["highway"] = CreateIconInfo(SHIELD_HIGHWAY, "highway.png");
    (*(_poiIconMap.get()))["interstate"] = CreateIconInfo(SHIELD_INTERSTATE, "interstate.png");
    (*(_poiIconMap.get()))["whiterectangle"] = CreateIconInfo(SHIELD_WHITERECT, "whiterectangle.png");
    
    (*(_placeIconMap.get()))["city"] = CreateIconInfo(PLACE_CITY, "city.png");
    (*(_placeIconMap.get()))["town"] = CreateIconInfo(PLACE_TOWN, "city.png");
    (*(_placeIconMap.get()))["village"] = CreateIconInfo(PLACE_VILLAGE, "city.png");
    (*(_placeIconMap.get()))["hamlet"] = CreateIconInfo(PLACE_HAMLET, "city.png");
    (*(_placeIconMap.get()))["suburb"] = CreateIconInfo(PLACE_SUBURB, "city.png");
    (*(_placeIconMap.get()))["neighbourhood"] = CreateIconInfo(PLACE_NEIGHBOURHOOD, "city.png");
    (*(_placeIconMap.get()))["island"] = CreateIconInfo(PLACE_ISLAND, "city.png");
    (*(_placeIconMap.get()))["islet"] = CreateIconInfo(PLACE_ISLET, "city.png");
    (*(_placeIconMap.get()))["archipelago"] = CreateIconInfo(PLACE_ARCHIPELAGO, "city.png");
    (*(_placeIconMap.get()))["residential"] = CreateIconInfo(PLACE_RESIDENTIAL, "city.png");
    (*(_placeIconMap.get()))["aboriginal_lands"] = CreateIconInfo(PLACE_ABORIGINAL_LANDS, "city.png");
    (*(_placeIconMap.get()))["undefined"] = CreateIconInfo(PM_UNKNOWN, "city.png");
    
    //(*(_poiIconMap.get()))["interstate"] = CreateIconInfo(PM_CUSTOM, "interstate.png");
    //(*(_poiIconMap.get()))["whiterectangle"] = CreateIconInfo(PM_CUSTOM, "whiterectangle.png");
    (*(_poiIconMap.get()))["custom"] = CreateIconInfo(PM_CUSTOM, "pinred.png");
    (*(_poiIconMap.get()))["undefined"] = CreateIconInfo(PM_UNKNOWN, "pinred.png");
    (*(_poiIconMap.get()))["airfield"] = CreateIconInfo(PM_AIRFIELD, "airport.png");
    (*(_poiIconMap.get()))["alcohol-shop"] = CreateIconInfo(PM_ALCOHOL_SHOP, "bar.png");
    (*(_poiIconMap.get()))["amusement-park"] = CreateIconInfo(PM_AMUSEMENT_PARK, "park.png");
    (*(_poiIconMap.get()))["aquarium"] = CreateIconInfo(PM_AQUARIUM, "aquarium.png");
    (*(_poiIconMap.get()))["art-gallery"] = CreateIconInfo(PM_ART_GALLERY, "artgallery.png");
    (*(_poiIconMap.get()))["attraction"] = CreateIconInfo(PM_ATTRACTION, "attraction.png");
    (*(_poiIconMap.get()))["bakery"] = CreateIconInfo(PM_BAKERY, "cafe.png");
    (*(_poiIconMap.get()))["bank"] = CreateIconInfo(PM_BANK, "bank.png");
    (*(_poiIconMap.get()))["bar"] = CreateIconInfo(PM_BAR, "bar.png");
    (*(_poiIconMap.get()))["beer"] = CreateIconInfo(PM_BEER, "bar.png");
    (*(_poiIconMap.get()))["bicycle"] = CreateIconInfo(PM_BICYCLE, "bicycle.png");
    (*(_poiIconMap.get()))["bicycle-share"] =CreateIconInfo(PM_BICYCLE_SHARE, "bicycle .png");
    (*(_poiIconMap.get()))["bus"] = CreateIconInfo(PM_BUS, "bus.png");
    (*(_poiIconMap.get()))["cafe"] = CreateIconInfo(PM_CAFE, "cafe.png");
    (*(_poiIconMap.get()))["car"] = CreateIconInfo(PM_CAR, "car.png");
    (*(_poiIconMap.get()))["campsite"] = CreateIconInfo(PM_CAMPSITE, "camp.png");
    (*(_poiIconMap.get()))["castle"] = CreateIconInfo(PM_CASTLE, "castle.png");
    (*(_poiIconMap.get()))["cemetery"] = CreateIconInfo(PM_CEMETERY, "cemetery.png");
    (*(_poiIconMap.get()))["cinema"] = CreateIconInfo(PM_CINEMA, "cinema.png");
    (*(_poiIconMap.get()))["clothing-store"] = CreateIconInfo(PM_CLOTHING_STORE, "clothingstore.png");
    (*(_poiIconMap.get()))["college"] = CreateIconInfo(PM_COLLEGE, "university.png");
    (*(_poiIconMap.get()))["dentist"] = CreateIconInfo(PM_DENTIST, "hospital.png");
    (*(_poiIconMap.get()))["dog-park"] = CreateIconInfo(PM_DOG_PARK, "park.png");
    (*(_poiIconMap.get()))["drinking-water"] = CreateIconInfo(PM_DRINKING_WATER, "poi.png");
    (*(_poiIconMap.get()))["embassy"] = CreateIconInfo(PM_EMBASSY, "poi.png");
    (*(_poiIconMap.get()))["entrance"] = CreateIconInfo(PM_ENTRANCE, "poi.png");
    (*(_poiIconMap.get()))["fast-food"] = CreateIconInfo(PM_FAST_FOOD, "cafe.png");
    (*(_poiIconMap.get()))["ferry"] = CreateIconInfo(PM_FERRY, "ferry.png");
    (*(_poiIconMap.get()))["fuel"] = CreateIconInfo(PM_FUEL, "fuel.png");
    (*(_poiIconMap.get()))["garden"] = CreateIconInfo(PM_GARDEN, "trees.png");
    (*(_poiIconMap.get()))["grocery"] = CreateIconInfo(PM_GROCERY, "store.png");
    (*(_poiIconMap.get()))["harbor"] = CreateIconInfo(PM_HARBOR, "waves.png");
    (*(_poiIconMap.get()))["heliport"] = CreateIconInfo(PM_HELIPORT, "airport.png");
    (*(_poiIconMap.get()))["ice-cream"] = CreateIconInfo(PM_ICE_CREAM, "cafe.png");
    (*(_poiIconMap.get()))["information"] = CreateIconInfo(PM_INFORMATION, "information.png");
    (*(_poiIconMap.get()))["laundry"] = CreateIconInfo(PM_LAUNDRY, "store.png");
    (*(_poiIconMap.get()))["lodging"] = CreateIconInfo(PM_LODGING, "house.png");
    (*(_poiIconMap.get()))["monument"] = CreateIconInfo(PM_MONUMENT, "monument.png");
    (*(_poiIconMap.get()))["museum"] = CreateIconInfo(PM_MUSEUM, "museum.png");
    (*(_poiIconMap.get()))["music"] = CreateIconInfo(PM_MUSIC, "music.png");
    (*(_poiIconMap.get()))["park"] = CreateIconInfo(PM_PARK, "park.png");
    (*(_poiIconMap.get()))["pharmacy"] = CreateIconInfo(PM_PHARMACY, "hospital.png");
    (*(_poiIconMap.get()))["picnic-site"] = CreateIconInfo(PM_PICNIC_SITE, "park.png");
    (*(_poiIconMap.get()))["place-of-worship"] = CreateIconInfo(PM_PLACE_OF_WORSHIP, "church.png");
    (*(_poiIconMap.get()))["playground"] = CreateIconInfo(PM_PLAYGROUND, "park.png");
    (*(_poiIconMap.get()))["police"] = CreateIconInfo(PM_POLICE, "police.png");
    (*(_poiIconMap.get()))["post"] = CreateIconInfo(PM_POST, "postal.png");
    (*(_poiIconMap.get()))["prison"] = CreateIconInfo(PM_PRISON, "prison.png");
    (*(_poiIconMap.get()))["religious-christian"] = CreateIconInfo(PM_RELIGIOUS_CHRISTIAN, "church.png");
    (*(_poiIconMap.get()))["religious-jewish"] = CreateIconInfo(PM_RELIGIOUS_JEWISH, "church.png");
    (*(_poiIconMap.get()))["religious-muslim"] = CreateIconInfo(PM_RELIGIOUS_MUSLIM, "mosque.png");
    (*(_poiIconMap.get()))["restaurant"] = CreateIconInfo(PM_RESTAURANT, "cafe.png");
    (*(_poiIconMap.get()))["rocket"] = CreateIconInfo(PM_ROCKET, "poi.png");
    (*(_poiIconMap.get()))["school"] = CreateIconInfo(PM_SCHOOL, "school.png");
    (*(_poiIconMap.get()))["shop"] = CreateIconInfo(PM_SHOP, "store.png");
    (*(_poiIconMap.get()))["stadium"] = CreateIconInfo(PM_STADIUM, "poi.png");
    (*(_poiIconMap.get()))["swimming"] = CreateIconInfo(PM_SWIMMING, "river.png");
    (*(_poiIconMap.get()))["suitcase"] = CreateIconInfo(PM_SUITCASE, "suitcase.png");
    (*(_poiIconMap.get()))["theatre"] = CreateIconInfo(PM_THEATRE, "cinema.png");
    (*(_poiIconMap.get()))["toilet"] = CreateIconInfo(PM_TOILET, "toilet.png");
    (*(_poiIconMap.get()))["town-hall"] = CreateIconInfo(PM_TOWN_HALL, "townhall.png");
    (*(_poiIconMap.get()))["veterinary"] = CreateIconInfo(PM_VETERINARY, "hospital.png");
    (*(_poiIconMap.get()))["zoo"] = CreateIconInfo(PM_ZOO, "zoo.png");
    (*(_poiIconMap.get()))["doctor"] = CreateIconInfo(PM_DOCTOR, "hospital.png");
    (*(_poiIconMap.get()))["hospital"] = CreateIconInfo(PM_HOSPITAL, "hospital.png");
    (*(_poiIconMap.get()))["library"] = CreateIconInfo(PM_LIBRARY, "library.png");
    (*(_poiIconMap.get()))["golf"] = CreateIconInfo(PM_GOLF, "golf.png");
    (*(_poiIconMap.get()))["fire-station"] = CreateIconInfo(PM_FIRE_STATION, "firemen.png");
    
}
int symbolListUID = 0;

void LFClientEngine::NotifyCustomSymbolListeners()
{
    for(int i = 0 ; i < NUMOFZOOMLEVELSRASTER ; i++)
    {
        boost::shared_ptr<vector<boost::shared_ptr<tile> > > zlvecRasterTiles = _rasterTileCache[i];
        for(int j = 0 ; j < zlvecRasterTiles->size() ; j++)
        {
            boost::shared_ptr<tile> spRasterTile = (*(zlvecRasterTiles.get()))[j];
            spRasterTile->OnNotifyCustomSymbolChange();
        }
    }
    for(int i = 0 ; i < NUMOFZOOMLEVELSVECTILE ; i++)
    {
        boost::shared_ptr<vector<boost::shared_ptr<tile> > > zlvecVectorTiles = _vectorTileCache[i];
        for(int j = 0 ; j < zlvecVectorTiles->size() ; j++)
        {
            boost::shared_ptr<tile> spVectorTile = (*(zlvecVectorTiles.get()))[j];
            spVectorTile->OnNotifyCustomSymbolChange();
        }
    }
}
std::vector<KmlPathDetails> LFClientEngine::GetKMLEntitiesFromAlreadyLoadedFile(std::string kmlFileName)
{
    std::vector<KmlPathDetails> listOfEntities;
    //todo: search through list of kml entities that were added from file (see hide/show/delete functions to see where list is)
    //          if filename matches inside KMLPathdetails with the given kmlfilename, then push that kmlpathdetails onto the list
    //          to be returned;
    for(int i = 0 ; i < _customSymbolList->size() ; i++)
    {
        boost::shared_ptr<symbol> sym = (*(_customSymbolList.get()))[i];
        boost::shared_ptr<KmlPathDetails> symPathDetails = sym->GetKmlPathDetails();
        KmlPathDetails kpd = *(symPathDetails.get());
        //KmlPathDetails kpd2 = *(kmlDetails.get());
        //if(kpd2 == kpd)
        if(kpd._filename.compare(kmlFileName) == 0)
        {
            //we found a match
            //sym->SetVisible(false);
            listOfEntities.push_back(kpd);
        }
    }
    for(int i = 0 ; i < _pathways.size() ; i++)
    {
        boost::shared_ptr<pathway> pw = _pathways[i];
        boost::shared_ptr<KmlPathDetails> pathDetails = pw->GetPathDetails();
        KmlPathDetails kpd = *(pathDetails.get());
        //KmlPathDetails kpd2 = *(kmlDetails.get());
        //if(kpd2 == kpd)
        if(kpd._filename.compare(kmlFileName) == 0)
        {
            //we found a match
            //pw->SetVisible(false);
            listOfEntities.push_back(kpd);
        }
    }
    return listOfEntities;
}

#if defined(__APPLE__)
void LFClientEngine::HideKMLEntities(std::string filename, std::vector<std::string> path, std::string element)
#else
void LFClientEngine::HideKMLEntities(QString filename, QStringList path, QString element)
#endif
{
    boost::mutex::scoped_lock lock(_kmlFileReadMutex);
    boost::shared_ptr<KmlPathDetails> kmlDetails(new KmlPathDetails);
#if defined(__APPLE__)
    kmlDetails->_element = element;//  //std::string utf8_text = qs.toUtf8().constData();
    kmlDetails->_filename = filename;
    for (int i = 0; i < path.size(); ++i)
    {
        
        kmlDetails->_path.push_back(path[i]);
#else
    kmlDetails->_element = element.toUtf8().constData();//  //std::string utf8_text = qs.toUtf8().constData();
    kmlDetails->_filename = filename.toUtf8().constData();
    for (int i = 0; i < path.size(); ++i)
    {
            kmlDetails->_path.push_back(path.at(i).toLocal8Bit().constData());
#endif
    }
    for(int i = 0 ; i < _customSymbolList->size() ; i++)
    {
        boost::shared_ptr<symbol> sym = (*(_customSymbolList.get()))[i];
        boost::shared_ptr<KmlPathDetails> symPathDetails = sym->GetKmlPathDetails();
        KmlPathDetails kpd = *(symPathDetails.get());
        KmlPathDetails kpd2 = *(kmlDetails.get());
        if(kpd2 == kpd)
        {
            //we found a match
            sym->SetVisible(false);
        }
            
    }
    for(int i = 0 ; i < _pathways.size() ; i++)
    {
        boost::shared_ptr<pathway> pw = _pathways[i];
        boost::shared_ptr<KmlPathDetails> pathDetails = pw->GetPathDetails();
        KmlPathDetails kpd = *(pathDetails.get());
        KmlPathDetails kpd2 = *(kmlDetails.get());
        if(kpd2 == kpd)
        {
            //we found a match
            pw->SetVisible(false);
        }
    }
}
    
#if defined(__APPLE__)
void LFClientEngine::DeleteKMLEntities(std::string filename, std::vector<std::string> path, std::string element){
#else
void LFClientEngine::DeleteKMLEntities(QString filename, QStringList path, QString element){
#endif
    boost::mutex::scoped_lock lock(_kmlFileReadMutex);
    boost::shared_ptr<KmlPathDetails> kmlDetails(new KmlPathDetails);
#if defined(__APPLE__)
    kmlDetails->_element = element;
    kmlDetails->_filename = filename;
#else
    kmlDetails->_element = element.toUtf8().constData();//  //std::string utf8_text = qs.toUtf8().constData();
    kmlDetails->_filename = filename.toUtf8().constData();
#endif
    for (int i = 0; i < path.size(); ++i)
    {
#if defined(__APPLE__)
        kmlDetails->_path.push_back(path[i]);
#else
        kmlDetails->_path.push_back(path.at(i).toLocal8Bit().constData());
#endif
        }
        for(int i = 0 ; i < _customSymbolList->size() ; i++)
        {
            boost::shared_ptr<symbol> sym = (*(_customSymbolList.get()))[i];
            boost::shared_ptr<KmlPathDetails> symPathDetails = sym->GetKmlPathDetails();
            KmlPathDetails kpd = *(symPathDetails.get());
            KmlPathDetails kpd2 = *(kmlDetails.get());
            if(kpd2 == kpd)
            {
                //we found a match
                _customSymbolList->erase(_customSymbolList->begin() + i);
            }
        }
        
        for(int i = 0 ; i < _pathways.size() ; i++)
        {
            boost::shared_ptr<pathway> pw = _pathways[i];
            boost::shared_ptr<KmlPathDetails> pathDetails = pw->GetPathDetails();
            KmlPathDetails kpd = *(pathDetails.get());
            KmlPathDetails kpd2 = *(kmlDetails.get());
            if(kpd2 == kpd)
            {
                //we found a match
                _pathways.erase(_pathways.begin() + i);
            }
    }
    
    NotifyCustomSymbolListeners();
}
std::vector<std::string> LFClientEngine::GetLoadedKMZFilenames()
{
    return _kmzFilesLoaded;
}
#if defined(__APPLE__)
        void LFClientEngine::ShowKMLEntities(std::string filename, std::vector<std::string> path, std::string element){
#else
            void LFClientEngine::ShowKMLEntities(QString filename, QStringList path, QString element){
#endif
                boost::mutex::scoped_lock lock(_kmlFileReadMutex);
                boost::shared_ptr<KmlPathDetails> kmlDetails(new KmlPathDetails);
#if defined(__APPLE__)
                kmlDetails->_element = element;//  //std::string utf8_text = qs.toUtf8().constData();
                kmlDetails->_filename = filename;
                
                for (int i = 0; i < path.size(); ++i)
                {
                    kmlDetails->_path.push_back(path[i]);
                }
#else
                kmlDetails->_element = element.toUtf8().constData();//  //std::string utf8_text = qs.toUtf8().constData();
                kmlDetails->_filename = filename.toUtf8().constData();
                
                for (int i = 0; i < path.size(); ++i)
                {
                    kmlDetails->_path.push_back(path.at(i).toLocal8Bit().constData());
                }
#endif
                for(int i = 0 ; i < _customSymbolList->size() ; i++)
                {
                    boost::shared_ptr<symbol> sym = (*(_customSymbolList.get()))[i];
                    boost::shared_ptr<KmlPathDetails> symPathDetails = sym->GetKmlPathDetails();
                    KmlPathDetails kpd = *(symPathDetails.get());
                    KmlPathDetails kpd2 = *(kmlDetails.get());
                    if(kpd2 == kpd)
                    {
                        //we found a match
                        sym->SetVisible(true);
                    }
                    
                }
                for(int i = 0 ; i < _pathways.size() ; i++)
                {
                    boost::shared_ptr<pathway> pw = _pathways[i];
                    boost::shared_ptr<KmlPathDetails> pathDetails = pw->GetPathDetails();
                    KmlPathDetails kpd = *(pathDetails.get());
                    KmlPathDetails kpd2 = *(kmlDetails.get());
                    if(kpd2 == kpd)
                    {
                        //we found a match
                        pw->SetVisible(true);
                    }
                }
            }
            //CustomKmlFunctions _gEntities;
            //int numEnts = 0;
            std::vector<boost::shared_ptr<KmlPathDetails> > LFClientEngine::AddKMLEntitiesFromFile(std::string kmlFileName, bool listOnly)
            {
                
                //boost::mutex::scoped_lock lock(_kmlFileReadMutex);
                //PUT A LOCK HERE SO THAT CLIENT APP CALLING FROM DIFFERENT THREAD, AND MAIN RENDER THREAD DON'T COLLIDE!!
                //IF LIST ONLY THEN SAVE THE FILENAME TO GLOBAL NAME, THEN THE RENDER THREAD CAN COME BACK AND USE IT TO CREATE THE OPENGL
                //ENTITIES.  SET A FLAG TO TRUE FROM CLIENT, AND FALSE FROM RENDER THREAD.
                std::vector<boost::shared_ptr<KmlPathDetails> > vecKmlPathDetails;
                if(listOnly)
                    symbolListUID++;//each kml file we add will increment this by 1 so each set of icons is unique
                this->WriteToStatusLog("grabbing kml from file", false, false);
                
                using namespace Kml;
                CustomKmlFunctions _gEntities;
                std::vector<std::string> newIconFileNames;
                int numEnts = 0;
                
                //if(listOnly)
                    numEnts = _gEntities.ParseFile(kmlFileName.c_str(), symbolListUID, newIconFileNames);//todo: send uid for use in path for custom icons
                if(numEnts == -1){
                    std::stringstream ss;
                    ss<<"Parsing kml file: "<<kmlFileName<<" failed";
                    WriteToStatusLog(ss.str(), true, true);
                    return vecKmlPathDetails;
                }
                boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > points = _gEntities.GetPoints(kmlFileName);
                boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > j = _gEntities.GetStreetLabels();
                
                boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecs = _gEntities.GetStreets(kmlFileName);
                for(int i = 0 ; i < vecs->size() ; i++)
                {
                    std::vector<boost::shared_ptr<Kml::Vectors> > vec = *vecs.get();
                    boost::shared_ptr<Kml::Vectors> spVector = vec[i];
                    vecKmlPathDetails.push_back(spVector->_kmlPathDetails);
                    
                    //DO THIS WHEN CHECKING QUEUE ON RENDER SIDE ONLY!!
                    if(!listOnly)
                    {
#if defined(__APPLE__)
                        GLuint whiteTexture = findTextureBasedOnHref("white.png");
#else
                        boost::shared_ptr<QOpenGLTexture> whiteTexture = findTextureBasedOnHref("white.png");
#endif
                        boost::shared_ptr<pathway> pway(new pathway(_usettingsdata, _statusLogger, _coordConverter, spVector, whiteTexture, _openglFont, _viewData, _EML));
                        //pway->SetEML(_EML);
                        pway->SetupOpenglForEntities(_viewData);
                        _pathways.push_back(pway);
                    }
                }
                
                boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > polys = _gEntities.GetPolygons();
                for(int i = 0 ; i < polys->size() ; i++)
                {
                    std::vector<boost::shared_ptr<Kml::Vectors> > poly = *polys.get();
                    boost::shared_ptr<Kml::Vectors> spPoly = poly[i];
                    vecKmlPathDetails.push_back(spPoly->_kmlPathDetails);
                }
                
                //DO THIS WHEN CHECKING QUEUE ON RENDER SIDE ONLY!!
                if(!listOnly)
                {
                    for(int i = 0 ; i < newIconFileNames.size() ; i++)
                    {
#if defined(__APPLE__)
                        std::string iconFolderPath = GetDataDirectory();
                        iconFolderPath.append("/");
                        GLuint texture = loadSymbolTextureWithFilename(newIconFileNames[i], "png", iconFolderPath);
#else
                        std::string iconFolderPath = GetIconFolderPath();
                        iconFolderPath.append("/");
                        boost::shared_ptr<QOpenGLTexture> texture = loadSymbolTextureWithFilename(newIconFileNames[i], "png", iconFolderPath);
#endif
                        (*(_symbolTextures.get()))[newIconFileNames[i]] = texture;
                        std::string stdMsg("Adding custom icon: ");
                        iconFolderPath.append(newIconFileNames[i]);
                        stdMsg.append(iconFolderPath);
                        WriteToStatusLog(stdMsg, false, true);
                    }
                }
                
                //DO THIS WHEN CHECKING QUEUE ON RENDER SIDE ONLY!!
                //CREATE symbols for each point, and add to our shared list with all the vectortiles
                for(int i = 0 ; i < points->size() ; i++)
                {
                    std::vector<boost::shared_ptr<Kml::Point> > vecPoints = *points.get();
                    boost::shared_ptr<Kml::Point> spPoint = vecPoints[i];
                    vecKmlPathDetails.push_back(spPoint->_kmlPathDetails);
                    if(!listOnly)
                    {
                        //DO THIS WHEN CHECKING QUEUE ON RENDER SIDE ONLY!!
                        std::string iconhref = spPoint->GetIconHref();
                        boost::replace_all(iconhref, "/", "-");
                        std::stringstream customHref;//
                        customHref<<"custom-"<<symbolListUID<<"-"<<iconhref;
                        std::string msg("adding custom symbol; ");
                        msg.append(customHref.str());
                        
                        WriteToStatusLog(msg, false, true);
                        double x = 0.00;
                        double y = 0.00;
                        double z = 0.00;
                        double xcart = 0.00;
                        double ycart = 0.00;
                        double zcart = 0.00;
                        double lat = spPoint->GetLat();
                        double lon = spPoint->GetLon();
                        double elev = _EML->GetElevation(spPoint->GetLat(), spPoint->GetLon());
                        std::string icon = spPoint->GetIconHref();
                        std::string nm = spPoint->GetName();
#if defined(__APPLE__)
                        GLuint whiteTexture = findTextureBasedOnHref("white.png");
                        std::string strCustomHref(customHref.str());
                        GLuint texture = findTextureBasedOnHref(strCustomHref);
#else
                        boost::shared_ptr<QOpenGLTexture> whiteTexture = findTextureBasedOnHref("white.png");
                        std::string strCustomHref(customHref.str());
                        boost::shared_ptr<QOpenGLTexture> texture = findTextureBasedOnHref(strCustomHref);
#endif
                        _coordConverter->GeodeticToCartesian(lat, lon, elev, xcart, ycart, zcart);
                        boost::shared_ptr<iconInfo> iinfo;
                        iinfo = (*(_poiIconMap.get()))["custom"];
                        if( !nm.empty() && texture && !(icon.compare("pinred.png") == 0))//pinred filter doesn't work??
                        {
                            boost::shared_ptr<symbol> sym(new symbol(lat, lon, xcart, ycart, elev, texture, _openglFont, nm, _userSettingsData,
                                                                     _statusLogger, whiteTexture, iinfo));
                            sym->SetKmlPathDetails(spPoint->_kmlPathDetails);
                            sym->InitOpenGLEntities();
                            _customSymbolList->push_back(sym);
                        }
                        else{
                        }
                    }
                }
                //DO THIS WHEN CHECKING QUEUE ON RENDER SIDE ONLY!!
                NotifyCustomSymbolListeners();
                
                
                return vecKmlPathDetails;
            }
            void LFClientEngine::SetSymbolElevations(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > > points)
            {
                using namespace Kml;
                for(int i = 0 ; i < points.size() ; i++)
                {
                    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > spPoints = points[i];
                    for(int j = 0 ; j < spPoints->size() ; j++)
                    {
                        boost::shared_ptr<Kml::Point> point = (*spPoints)[j];
                        double lat = point->GetLat();
                        double lon = point->GetLon();
                        double alt = _EML->GetElevation(lat, lon);
                        point->SetAlt(alt);
                        double x = 0.00;
                        double y = 0.00;
                        double z = 0.00;
                        _coordConverter->GeodeticToCartesian(lat, lon, alt, x, y, z);
                        point->SetX(x);
                        point->SetY(y);
                        point->SetZ(z);
                    }
                }
            }
            //When user enters 2d map, the lfclient instance gets destroyed and re-created.  We want to show all
            //kml files that were previously loaded.  Add files from the global list that stays around in between
            //lfclient destruction.
            void LFClientEngine::AddKMLFileRegen(){
                //foreach _listOfKmlFilesToLoad
                //  AddKMLFile(filename, regen =treu)
                for(int i = 0 ; i < _listOfKmlFilesToLoad.size() ; i++){
                    AddKMLFile(_listOfKmlFilesToLoad[i], true);//this is a regen call, so we don't want to add this to the list again
                }
            }
            std::vector<PlacemarkInfo > LFClientEngine::AddKMLFile(std::string kmlfile, bool regen)
            {
                //DGD for some reason, the filepath used from the document picker thread is valid only from that call, after that
                //it seems we can't find the file anymore.  So I think we need to copy the kmz file to the data folder, then from that point
                //on load it from there.
//                std::string dataPath = GetDataFolderPath();
//                std::size_t found = kmlfile.find_last_of("/");
//                std::string filename = kmlfile.substr(found+1) ;
//                std::ifstream  src(kmlfile, std::ios::binary);
//                std::string destination = dataPath.append("/");
//                destination.append(filename);//copy to this path in the data folder so we can access.
//                std::ofstream  dst(destination,   std::ios::binary);
//                dst << src.rdbuf();
//                kmlfile = destination;
                
                if(regen == false)//this is called from the user, so we want to maintain a list of kml files they loaded
                    _listOfKmlFilesToLoad.push_back(kmlfile);
                
                //todo: mutex
                boost::mutex::scoped_lock lock(_kmlFileReadMutex);
                std::vector<PlacemarkInfo> vecPmInfo;
                std::vector<boost::shared_ptr<KmlPathDetails> > details = AddKMLEntitiesFromFile(kmlfile, true);
                for(int i = 0 ; i < details.size() ; i++)
                {
                    PlacemarkInfo pmInfo;
                    boost::shared_ptr<KmlPathDetails> currentDetail = details[i];
#if defined(__APPLE__)
                    pmInfo._element = currentDetail->_element;
                    pmInfo._filename = currentDetail->_filename;
                    for(int k = 0 ; k < currentDetail->_path.size() ; k++){
                        pmInfo._path.push_back(currentDetail->_path[k]);
                    }
#else
                    pmInfo._element = QString::fromStdString(currentDetail->_element);
                    pmInfo._filename = QString::fromStdString(currentDetail->_filename);
                    for(int k = 0 ; k < currentDetail->_path.size() ; k++){
                        pmInfo._path<<QString::fromStdString(currentDetail->_path[k]);
                    }
#endif
                    vecPmInfo.push_back(pmInfo);
                }
                _newKmlFile = true;//the client wishes to add a kml file, signal to main render thread to do so
                _kmlFilename = kmlfile;
                _kmzFilesLoaded.push_back(kmlfile);
                return vecPmInfo;
            }
            
            
            
            void LFClientEngine::SetAlphaStreetVector(double alpha)
            {
                _alphaStreetVector = alpha;
            }
            void LFClientEngine::SetAlphaStreetLabel(double alpha)
            {
                _alphaStreetLabel = alpha;
            }
            void LFClientEngine::SetAlphaPOI(double alpha)
            {
                _alphaPOI = alpha;
            }
            
#include "png.h"
#if defined(__APPLE__)
            GLuint LFClientEngine::loadSymbolTextureWithFilename(std::string filename,
                                                                 std::string fileType, std::string folder)
#else
            boost::shared_ptr<QOpenGLTexture> LFClientEngine::loadSymbolTextureWithFilename(std::string filename,
                                                                                            std::string fileType, std::string folder)
#endif
            {
                std::string fullPath = folder;
#if defined(__APPLE__)
                fullPath.append("/");
#endif
                fullPath.append(filename);
                std::string errorMsg;
                if(!FileExists(fullPath, errorMsg))
                {
                    std::string rismsg("can't find file ");
                    rismsg.append(fullPath);
                    WriteToStatusLog(errorMsg, true, true);
                    //this->WriteToStatusLog(rismsg, true, true);
                    return 0x00;
                }
#if defined(__APPLE__)
                GLuint textureRef ;
                
                LFObjCFunctionsCPPWrapper ocWrapper;
                int width = 0;
                int height = 0;
                //GLubyte* imageData = 0x00;
                //TODO: TRY USING LIBPNG INSTEAD
                GLubyte* imageData = ocWrapper.loaduserpng(fullPath, width, height);
                
                glGenTextures(1, &textureRef);
                glBindTexture(GL_TEXTURE_2D, textureRef);
                
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
                boost::shared_ptr<QOpenGLTexture> textureRef;// = 0x00;
                textureRef.reset();
                textureRef.reset(new QOpenGLTexture(QImage(fullPath.c_str()).mirrored()));
#endif
                if(!textureRef)
                {
                    std::string rismsg("can't create texture from: ");
                    rismsg.append(fullPath);
                    return 0x00;
                }
                try
                {
#if !defined(__APPLE__)
                    // Set nearest filtering mode for texture minification
                    // Set bilinear filtering mode for texture magnification
                    textureRef->setMagnificationFilter(QOpenGLTexture::Linear);
                    
                    // Wrap texture coordinates by repeating
#endif
                }
                catch(...)
                {
                    std::string rismsg("seg fault in setminification: ");
                    
                }
                
                return textureRef;
            }
            void LFClientEngine::SetPOISettings(boost::shared_ptr<placemarkCollection> pmCollection)
            {
                //copy from pmCollection to _placemarkCollection;
                //_placemarkCollection = pmCollection;
                //boost::shared_ptr<placemarkCollection> pmc2(new placemarkCollection(pmCollection));
                boost::mutex::scoped_lock lock(glbPlacemarkMutex);
                _placemarkCollection->_placemarkVisibleSettings = pmCollection->_placemarkVisibleSettings;
                //(*(_placemarkCollection.get())) = (*(pmCollection.get()));
            }
            
            //todo: move to vectortiles class?
            void LFClientEngine::SetUserSettings()
            {
                boost::mutex::scoped_lock lock(glbPlacemarkMutex);
                if(!(_usettingsdata->_intSettings.find("PLACECITY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_CITY, _usettingsdata->_intSettings["PLACECITY"]);}
                if(!(_usettingsdata->_intSettings.find("PLACETOWN") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_TOWN, _usettingsdata->_intSettings["PLACETOWN"]);}
                if(!(_usettingsdata->_intSettings.find("PLACEVILLAGE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_VILLAGE, _usettingsdata->_intSettings["PLACETOWN"]);}
                if(!(_usettingsdata->_intSettings.find("PLACEHAMLET") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_HAMLET, _usettingsdata->_intSettings["PLACETOWN"]);}
                if(!(_usettingsdata->_intSettings.find("PLACESUBURB") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_SUBURB, _usettingsdata->_intSettings["PLACETOWN"]);}
                if(!(_usettingsdata->_intSettings.find("PLACENEIGHBOURHOOD") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_NEIGHBOURHOOD, _usettingsdata->_intSettings["PLACETOWN"]);}
                if(!(_usettingsdata->_intSettings.find("PLACEISLAND") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_ISLAND, _usettingsdata->_intSettings["PLACETOWN"]);}
                if(!(_usettingsdata->_intSettings.find("PLACEISLET") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_ISLET, _usettingsdata->_intSettings["PLACETOWN"]);}
                if(!(_usettingsdata->_intSettings.find("PLACEARCHIPELAGO") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_ARCHIPELAGO, _usettingsdata->_intSettings["PLACETOWN"]);}
                if(!(_usettingsdata->_intSettings.find("PLACERESIDENTIAL") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_RESIDENTIAL, _usettingsdata->_intSettings["PLACETOWN"]);}
                if(!(_usettingsdata->_intSettings.find("PLACEABORIGINAL_LANDS") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PLACE_ABORIGINAL_LANDS, _usettingsdata->_intSettings["PLACETOWN"]);}
                
                if(!(_usettingsdata->_intSettings.find("PMAIRFIELD") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_AIRFIELD, _usettingsdata->_intSettings["PMAIRFIELD"]);}
                if(!(_usettingsdata->_intSettings.find("PMALCOHOLSHOP") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_ALCOHOL_SHOP, _usettingsdata->_intSettings["PMALCOHOLSHOP"]);}
                if(!(_usettingsdata->_intSettings.find("PMAMUSEMENTPARK") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_AMUSEMENT_PARK, _usettingsdata->_intSettings["PMAMUSEMENTPARK"]);}
                if(!(_usettingsdata->_intSettings.find("PMAQUARIUM") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_AQUARIUM, _usettingsdata->_intSettings["PMAQUARIUM"]);}
                if(!(_usettingsdata->_intSettings.find("PMARTGALLERY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_ART_GALLERY, _usettingsdata->_intSettings["PMARTGALLERY"]);}
                if(!(_usettingsdata->_intSettings.find("PMATTRACTION") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_ATTRACTION, _usettingsdata->_intSettings["PMATTRACTION"]);}
                if(!(_usettingsdata->_intSettings.find("PMBAKERY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_BAKERY, _usettingsdata->_intSettings["PMBAKERY"]);}
                if(!(_usettingsdata->_intSettings.find("PMBANK") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_BANK, _usettingsdata->_intSettings["PMBANK"]);}
                if(!(_usettingsdata->_intSettings.find("PMBAR") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_BAR, _usettingsdata->_intSettings["PMBAR"]);}
                if(!(_usettingsdata->_intSettings.find("PMBEER") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_BEER, _usettingsdata->_intSettings["PMBEER"]);}
                if(!(_usettingsdata->_intSettings.find("PMBICYCLE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_BICYCLE, _usettingsdata->_intSettings["PMBICYCLE"]);}
                if(!(_usettingsdata->_intSettings.find("PMBICYCLESHARE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_BICYCLE_SHARE, _usettingsdata->_intSettings["PMBICYCLESHARE"]);}
                
                if(!(_usettingsdata->_intSettings.find("PMBUS") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_BUS, _usettingsdata->_intSettings["PMBUS"]);}
                if(!(_usettingsdata->_intSettings.find("PMCAFE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_CAFE, _usettingsdata->_intSettings["PMCAFE"]);}
                if(!(_usettingsdata->_intSettings.find("PMCAR") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_CAR, _usettingsdata->_intSettings["PMCAR"]);}
                if(!(_usettingsdata->_intSettings.find("PMCAMPSITE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_CAMPSITE, _usettingsdata->_intSettings["PMCAMPSITE"]);}
                if(!(_usettingsdata->_intSettings.find("PMCASTLE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_CASTLE, _usettingsdata->_intSettings["PMCASTLE"]);}
                if(!(_usettingsdata->_intSettings.find("PMCEMETERY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_CEMETERY, _usettingsdata->_intSettings["PMCEMETERY"]);}
                if(!(_usettingsdata->_intSettings.find("PMCINEMA") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_CINEMA, _usettingsdata->_intSettings["PMCINEMA"]);}
                if(!(_usettingsdata->_intSettings.find("PMCLOTHINGSTORE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_CLOTHING_STORE, _usettingsdata->_intSettings["PMCLOTHINGSTORE"]);}
                if(!(_usettingsdata->_intSettings.find("PMCOLLEGE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_COLLEGE, _usettingsdata->_intSettings["PMCOLLEGE"]);}
                if(!(_usettingsdata->_intSettings.find("PMDENTIST") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_DENTIST, _usettingsdata->_intSettings["PMDENTIST"]);}
                
                if(!(_usettingsdata->_intSettings.find("PMDOGPARK") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_DOG_PARK, _usettingsdata->_intSettings["PMDOGPARK"]);}
                if(!(_usettingsdata->_intSettings.find("PMDRINKINGWATER") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_DRINKING_WATER, _usettingsdata->_intSettings["PMDRINKINGWATER"]);}
                if(!(_usettingsdata->_intSettings.find("PMEMBASSY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_EMBASSY, _usettingsdata->_intSettings["PMEMBASSY"]);}
                if(!(_usettingsdata->_intSettings.find("PMENTRANCE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_ENTRANCE, _usettingsdata->_intSettings["PMENTRANCE"]);}
                if(!(_usettingsdata->_intSettings.find("PMFASTFOOD") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_FAST_FOOD, _usettingsdata->_intSettings["PMFASTFOOD"]);}
                if(!(_usettingsdata->_intSettings.find("PMFERRY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_FERRY, _usettingsdata->_intSettings["PMFERRY"]);}
                if(!(_usettingsdata->_intSettings.find("PMFUEL") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_FUEL, _usettingsdata->_intSettings["PMFUEL"]);}
                if(!(_usettingsdata->_intSettings.find("PMGARDEN") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_GARDEN, _usettingsdata->_intSettings["PMGARDEN"]);}
                if(!(_usettingsdata->_intSettings.find("PMGROCERY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_GROCERY, _usettingsdata->_intSettings["PMGROCERY"]);}
                if(!(_usettingsdata->_intSettings.find("PMHARBOR") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_HARBOR, _usettingsdata->_intSettings["PMHARBOR"]);}
                
                if(!(_usettingsdata->_intSettings.find("PMHELIPORT") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_HELIPORT, _usettingsdata->_intSettings["PMHELIPORT"]);}
                if(!(_usettingsdata->_intSettings.find("PMICECREAM") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_ICE_CREAM, _usettingsdata->_intSettings["PMICECREAM"]);}
                if(!(_usettingsdata->_intSettings.find("PMINFORMATION") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_INFORMATION, _usettingsdata->_intSettings["PMINFORMATION"]);}
                if(!(_usettingsdata->_intSettings.find("PMLAUNDRY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_LAUNDRY, _usettingsdata->_intSettings["PMLAUNDRY"]);}
                if(!(_usettingsdata->_intSettings.find("PMLODGING") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_LODGING, _usettingsdata->_intSettings["PMLODGING"]);}
                if(!(_usettingsdata->_intSettings.find("PMMONUMENT") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_MONUMENT, _usettingsdata->_intSettings["PMMONUMENT"]);}
                if(!(_usettingsdata->_intSettings.find("PMMUSEUM") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_MUSEUM, _usettingsdata->_intSettings["PMMUSEUM"]);}
                if(!(_usettingsdata->_intSettings.find("PMMUSIC") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_MUSIC, _usettingsdata->_intSettings["PMMUSIC"]);}
                if(!(_usettingsdata->_intSettings.find("PMPARK") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_PARK, _usettingsdata->_intSettings["PMPARK"]);}
                if(!(_usettingsdata->_intSettings.find("PMPHARMACY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_PHARMACY, _usettingsdata->_intSettings["PMPHARMACY"]);}
                
                if(!(_usettingsdata->_intSettings.find("PMPICNICSITE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_PICNIC_SITE, _usettingsdata->_intSettings["PMPICNICSITE"]);}
                if(!(_usettingsdata->_intSettings.find("PMPLACEOFWORSHIP") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_PLACE_OF_WORSHIP, _usettingsdata->_intSettings["PMPLACEOFWORSHIP"]);}
                if(!(_usettingsdata->_intSettings.find("PMPLAYGROUND") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_PLAYGROUND, _usettingsdata->_intSettings["PMPLAYGROUND"]);}
                if(!(_usettingsdata->_intSettings.find("PMPOLICE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_POLICE, _usettingsdata->_intSettings["PMPOLICE"]);}
                if(!(_usettingsdata->_intSettings.find("PMPOST") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_POST, _usettingsdata->_intSettings["PMPOST"]);}
                if(!(_usettingsdata->_intSettings.find("PMPRISON") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_PRISON, _usettingsdata->_intSettings["PMPRISON"]);}
                if(!(_usettingsdata->_intSettings.find("PMRELIGIOUSCHRISTIAN") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_RELIGIOUS_CHRISTIAN, _usettingsdata->_intSettings["PMRELIGIOUSCHRISTIAN"]);}
                if(!(_usettingsdata->_intSettings.find("PMRELIGIOUSJEWISH") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_RELIGIOUS_JEWISH, _usettingsdata->_intSettings["PMRELIGIOUSJEWISH"]);}
                if(!(_usettingsdata->_intSettings.find("PMRELIGIOUSMUSLIM") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_RELIGIOUS_MUSLIM, _usettingsdata->_intSettings["PMRELIGIOUSMUSLIM"]);}
                if(!(_usettingsdata->_intSettings.find("PMRESTAURANT") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_RESTAURANT, _usettingsdata->_intSettings["PMRESTAURANT"]);}
                
                if(!(_usettingsdata->_intSettings.find("PMROCKET") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_ROCKET, _usettingsdata->_intSettings["PMROCKET"]);}
                if(!(_usettingsdata->_intSettings.find("PMSCHOOL") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_SCHOOL, _usettingsdata->_intSettings["PMSCHOOL"]);}
                if(!(_usettingsdata->_intSettings.find("PMSHOP") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_SHOP, _usettingsdata->_intSettings["PMSHOP"]);}
                if(!(_usettingsdata->_intSettings.find("PMSTADIUM") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_STADIUM, _usettingsdata->_intSettings["PMSTADIUM"]);}
                if(!(_usettingsdata->_intSettings.find("PMSWIMMING") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_SWIMMING, _usettingsdata->_intSettings["PMSWIMMING"]);}
                if(!(_usettingsdata->_intSettings.find("PMSUITCASE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_SUITCASE, _usettingsdata->_intSettings["PMSUITCASE"]);}
                if(!(_usettingsdata->_intSettings.find("PMTHEATRE") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_THEATRE, _usettingsdata->_intSettings["PMTHEATRE"]);}
                if(!(_usettingsdata->_intSettings.find("PMTOILET") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_TOILET, _usettingsdata->_intSettings["PMTOILET"]);}
                if(!(_usettingsdata->_intSettings.find("PMTOWNHALL") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_TOWN_HALL, _usettingsdata->_intSettings["PMTOWNHALL"]);}
                if(!(_usettingsdata->_intSettings.find("PMVETERINARY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_VETERINARY, _usettingsdata->_intSettings["PMVETERINARY"]);}
                
                if(!(_usettingsdata->_intSettings.find("PMZOO") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_ZOO, _usettingsdata->_intSettings["PMZOO"]);}
                if(!(_usettingsdata->_intSettings.find("PMDOCTOR") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_DOCTOR, _usettingsdata->_intSettings["PMDOCTOR"]);}
                if(!(_usettingsdata->_intSettings.find("PMHOSPITAL") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_HOSPITAL, _usettingsdata->_intSettings["PMHOSPITAL"]);}
                if(!(_usettingsdata->_intSettings.find("PMLIBRARY") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_LIBRARY, _usettingsdata->_intSettings["PMLIBRARY"]);}
                if(!(_usettingsdata->_intSettings.find("PMGOLF") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_GOLF, _usettingsdata->_intSettings["PMGOLF"]);}
                if(!(_usettingsdata->_intSettings.find("PMFIRE_STATION") == _usettingsdata->_intSettings.end())){_placemarkCollection->setValue(PM_FIRE_STATION, _usettingsdata->_intSettings["PMFIRE_STATION"]);}
                if(!_usettingsdata->_stringSettings["VIDEOFILE"].empty())
                {
                    this->_videoFileName = _usettingsdata->_stringSettings["VIDEOFILE"];
                    _playVideo = true;
                }
                else{
                    _playVideo = false;
                    this->_videoFileName = "";
                }
                
                if(!(_usettingsdata->_intSettings.find("STARTLOCATIONLAT") == _usettingsdata->_intSettings.end()))
                {
                    int l = _usettingsdata->_intSettings["STARTLOCATIONLAT"];
                    double dl = static_cast<double>(l);
                    this->_startingLat = dl / 1000000.0;
                    _settingsFileFlight = true;
                }
                else{
                    this->_startingLat = 46.00;
                }
                
                if(!(_usettingsdata->_intSettings.find("ALPHASTREETLABEL") == _usettingsdata->_intSettings.end()))
                {
                    int l = _usettingsdata->_intSettings["ALPHASTREETLABEL"];
                    double dl = static_cast<double>(l);
                    _alphaStreetLabel = dl / 1000.0;
                    //_alpha = true;
                }
                else{
                    _alphaStreetLabel = 1.00;
                }
                if(!(_usettingsdata->_intSettings.find("ALPHASTREETVECTOR") == _usettingsdata->_intSettings.end()))
                {
                    int l = _usettingsdata->_intSettings["ALPHASTREETVECTOR"];
                    double dl = static_cast<double>(l);
                    _alphaStreetVector = dl / 1000.0;
                    //_alpha = true;
                }
                else{
                    _alphaStreetVector = 1.00;
                }
                if(!(_usettingsdata->_intSettings.find("ALPHAPOI") == _usettingsdata->_intSettings.end()))
                {
                    int l = _usettingsdata->_intSettings["ALPHAPOI"];
                    double dl = static_cast<double>(l);
                    _alphaPOI = dl / 1000.0;
                    //_alpha = true;
                }
                else{
                    _alphaPOI = 1.00;
                }
                
                
                if(!(_usettingsdata->_intSettings.find("MAXRENDERTILES") == _usettingsdata->_intSettings.end()))
                {
                    _maxTilesToRender = _usettingsdata->_intSettings["MAXRENDERTILES"];
                }
                else{
                    _maxTilesToRender = 14;
                }
                
                if(!(_usettingsdata->_intSettings.find("STARTLOCATIONLON") == _usettingsdata->_intSettings.end()))
                {
                    int l = _usettingsdata->_intSettings["STARTLOCATIONLON"];
                    double dl = static_cast<double>(l);
                    this->_startingLon = dl / 1000000.0;
                    _settingsFileFlight = true;
                }
                else{
                    this->_startingLon = -106.00;
                }
                
                if(!(_usettingsdata->_intSettings.find("STARTLOCATIONALT") == _usettingsdata->_intSettings.end()))
                {
                    int l = _usettingsdata->_intSettings["STARTLOCATIONALT"];
                    double dl = static_cast<double>(l);
                    this->_startingAlt = dl;
                    _settingsFileFlight = true;
                }
                else{
                    this->_startingAlt = 5000.00;
                }
                
                if(!(_usettingsdata->_intSettings.find("DELTALAT") == _usettingsdata->_intSettings.end()))
                {
                    int l = _usettingsdata->_intSettings["DELTALAT"];
                    double dl = static_cast<double>(l);
                    this->_manualDeltaLat = dl / 1000000.0;
                    _settingsFileFlight = true;
                }
                else{
                    this->_manualDeltaLat = .000037;
                }
                
                if(!(_usettingsdata->_intSettings.find("MINRASTERZOOMLEVEL") == _usettingsdata->_intSettings.end()))
                {
                    _minRasterZoomLevel = _usettingsdata->_intSettings["MINRASTERZOOMLEVEL"];
                }
                else{
                    _minRasterZoomLevel = 0;
                }
                
                if(!(_usettingsdata->_intSettings.find("DELTALON") == _usettingsdata->_intSettings.end()))
                {
                    int l = _usettingsdata->_intSettings["DELTALON"];
                    double dl = static_cast<double>(l);
                    this->_manualDeltaLon = dl / 1000000.0;
                    _settingsFileFlight = true;
                }
                else{
                    this->_manualDeltaLon = 0.00;
                }
                
                _fontPngFile = AppDataDir();
                _fontPngFile.append("/");
                _fontJsonFile = AppDataDir();
                _fontJsonFile.append("/");
                if(!_usettingsdata->_stringSettings["TEXTFONT"].empty())
                {
                    _fontPngFile.append(_usettingsdata->_stringSettings["TEXTFONT"]);
                }
                else
                {
                    _fontPngFile.append("arial.png");
                }
                
                if(!_usettingsdata->_stringSettings["TEXTFONTJSON"].empty())
                {
                    _fontJsonFile.append(_usettingsdata->_stringSettings["TEXTFONTJSON"]);
                }
                else
                {
                    _fontJsonFile.append("arial.json");
                }
            }
#include "boost/filesystem.hpp"
            void LFClientEngine::LoadSymbolTextures(){
                using namespace boost::filesystem;
                
                if(!_symbolTexturesLoaded)
                {
                    std::string iconFolder = AppDataDir();
#if !defined(__APPLE__)
                    iconFolder.append("/icons/");
#endif
                    boost::filesystem::path iconsPath(iconFolder);
                    
                    directory_iterator end_itr;
                    
                    std::string errorMsg;
#if !defined(__APPLE__)
                    iconFolder.append("/");
#endif
                    if(!FileExists(iconFolder, errorMsg))
                    {
                        WriteToStatusLog(errorMsg, true, true);
                        return ;
                    }
                    // cycle through the directory
                    for (directory_iterator itr(iconsPath); itr != end_itr; ++itr)
                    {
                        string current_file = itr->path().string();
                        // cout << current_file << endl;
                        // If it's not a directory, list it. If you want to list directories too, just remove this check.
                        if(!(boost::filesystem::is_directory(itr->path())))
                        {
                            // assign current file name to current_file and echo it out to the console.
                            std::vector<std::string> strVec = TokenizeStringBySubstring(current_file, "/");
                            if(strVec.size() >= 3)
                            {
                                std::string fname = strVec[strVec.size() - 1];
                                char char1 = fname[fname.length() - 3];
                                char char2 = fname[fname.length() - 2];
                                char char3 = fname[fname.length() - 1];
                                if((char1 == 'p' && char2 == 'n' && char3 == 'g')||(char1 == 'P' && char2 == 'N' && char3 == 'G'))
                                {
#if defined(__APPLE__)
                                    GLuint p = loadSymbolTextureWithFilename(fname, "png", iconFolder);
#else
                                    boost::shared_ptr<QOpenGLTexture> p = loadSymbolTextureWithFilename(fname, "png", iconFolder);
#endif
                                    (*(_symbolTextures.get()))[fname] = p;
                                }
                            }
                            
                        }
                    }
                    
                    _symbolTexturesLoaded = true;
                }
            }
#if defined(__APPLE__)
            #include <OpenGLES/ES3/gl.h>
#include "objcUtilityWrapper.h"
            //TODO: I THINK WE NOW CREATE OUR OWN SHADERS IN LFCLIENTENGINE
            int LFClientEngine::Init()
#else
            int LFClientEngine::Init(boost::shared_ptr<SViewData> viewData, QOpenGLShaderProgram* symShader,
                                     QOpenGLShaderProgram* lineworkShader)
#endif
            {
                glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &_maxBindPoints);
                if(_initialized == true)
                    return 0;
                int thCount = 0;
                for (int i = 0; i < boost::thread::hardware_concurrency() ; ++i)
                {
                    thCount++;
                    threadgroup.create_thread(boost::bind(&boost::asio::io_service::run,
                                                      &io_service));
                }
                
                _userSettings.reset( new TextFileSettings);
                _usettingsdata.reset(new UserSettingsData);
                _statusLogger.reset(new StatusLogger);
#if defined(__APPLE__)
                std::string settingsFileName = GetBundleFolderPath();
                settingsFileName.append("/settings.ini");
#endif

                _kmzFilesLoaded.push_back("User Points");
                int error = _userSettings->GetSettings(_usettingsdata);
                _statusLogger->Init(_usettingsdata);
                //this will call the "setusersettings" function automatically, for getting settings from settings.ini file
                LoggableConfigurableObject::Init(_usettingsdata, _statusLogger, "LFClientEngine");
                //"   int numStreets;                                                               \n"
                //"   int streetIndices[50];                                                   \n"
                //"   vec3 translation[200];                                                           \n"
                static const GLchar* uniformNames[3] =
                {
                    "StreetVertices.numStreets",
                    "StreetVertices.streetIndices",
                    "StreetVertices.translation"
                };
                
                
                GLuint uniformIndices = 0;
                glGetUniformIndices(_risShaderProgramAutoScale, 3, uniformNames, &uniformIndices);//get locations of vars inside our uniform buffer
                GLint uniformOffsets[3] = {0, 0, 0};
                GLint arrayStrides[3] = {0, 0, 0};
                GLint matrixStrides[3] = {0, 0, 0};
                
                int indexStreetVerts = 2;
                int indexStreetVertsIndex = 1;
                int indexStreetVertsSize = 0;
                
                glGetActiveUniformsiv(_risShaderProgramAutoScale, 3, &uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);
                glGetActiveUniformsiv(_risShaderProgramAutoScale, 3, &uniformIndices, GL_UNIFORM_ARRAY_STRIDE, arrayStrides);
                glGetActiveUniformsiv(_risShaderProgramAutoScale, 3, &uniformIndices, GL_UNIFORM_MATRIX_STRIDE, matrixStrides);
                
                _autoScaleOffsetSize = uniformOffsets[indexStreetVertsSize];
                _autoScaleOffsetStVecs = uniformOffsets[indexStreetVerts];
                _autoScaleOffsetIndices = uniformOffsets[indexStreetVertsIndex];
                _autoScaleStVecStride = arrayStrides[indexStreetVerts];
                _autoScaleIndicesStride = arrayStrides[indexStreetVertsIndex];
                
                _ublockindex = 999;//test to see if uniform block is found
                _ublockindex = glGetUniformBlockIndex(_risShaderProgramAutoScale, "StreetVertices");
                glUniformBlockBinding(_risShaderProgramAutoScale, _ublockindex, 1);
                
                _openglFont.reset(new openglfont(_usettingsdata, _statusLogger));
                _EML.reset(new ElevationManagerLocal(this->_usettingsdata, _statusLogger));
                _footPrintCalculator.reset(new RAFootprint(_usettingsdata, _statusLogger));
                _footPrintCalculator->SetEML(_EML);
                _footPrintCalculator->SetCoordConverter(_coordConverter);
                
                
                
                
                //_EML->SetUserSettings(_usettingsdata);
                
                //printUserSettings(_usettingsdata);
#if defined VIDEOPLAYER
                vidPlayer.Init();
#endif
                double frameRate = 0.0;
                
                
#if defined VIDEOPLAYER
                sil.SetVideoFileName(_videoFileName);
                sil.Init(frameRate);
#endif
                LoadSymbolTextures();
                _openglFont->loadFont(_fontPngFile, _fontJsonFile);
                _initialized = true;
                return 0;
            }
            
            void LFClientEngine::PrintStatusOfStack(boost::shared_ptr<vector<boost::shared_ptr<tile> > >  stackToPrint){
                //stacktoprint is a ptr to a zoomlevel of tiles to print x,y coords of
                return;
                vector<boost::shared_ptr<tile> > vecTiles = *(stackToPrint.get());
                for(int i = 0 ; i < vecTiles.size() ; i++)
                {
                    boost::shared_ptr<tile> ptrVecTile = vecTiles[i];
                }
                _initialized = true;
            }
            
            //todo: move to rastertiles class?
            //todo: do same in vectortile class except it only goes from 0 - 14
            //calc highest zoom level that doesn't exceed MAXTILESTORENDER, set mintTileX etc to the boundingbox to grab for this footprint
            int LFClientEngine::GetZoomLevel(int& minTileX, int& minTileY, int& maxTileX, int& maxTileY, TILETYPE tiletype)
            {
                int numZoomLevels = 0;
                switch(tiletype)
                {
                    case TTRASTER:
                        numZoomLevels = NUMOFZOOMLEVELSRASTER;
                        break;
                    case TTVECTOR:
                        numZoomLevels = NUMOFZOOMLEVELSVECTILE;
                        break;
                }
                
                //get lower/upper lat lon of footprint and form a box that is aligned with n/s/e/w
                //use these values to gather tile x,y deltas of 0 - NUMOFZOOMLEVELS vectiles
                //multiply them, and use the set that is closest to MAXTILES without going over
                //use that zoomlevel for calculating tile grid
                double lowestLon = _currentFootPrint.fpLon[0];
                double lowestLat = _currentFootPrint.fpLat[0];
                double highestLat = _currentFootPrint.fpLat[0];
                
                double highestLon = _currentFootPrint.fpLon[0];
                
                for(int i = 0  ; i < 4 ; i++)
                {
                    if(_currentFootPrint.fpLon[i] < lowestLon)
                    {
                        lowestLon = _currentFootPrint.fpLon[i];
                    }
                    if(_currentFootPrint.fpLon[i] > highestLon)
                    {
                        highestLon = _currentFootPrint.fpLon[i];
                    }
                    if(_currentFootPrint.fpLat[i] < lowestLat)
                    {
                        lowestLat = _currentFootPrint.fpLat[i];
                    }
                    if(_currentFootPrint.fpLat[i] > highestLat)
                    {
                        highestLat = _currentFootPrint.fpLat[i];
                    }
                }
#if defined(__APPLE__)
                int hardSetRasterZoomLevel = 16;
                int hardSetVecTileZoomLevel = 14;
                int zoomLevelToGrab = 0;
                if(tiletype == TTRASTER)
                    zoomLevelToGrab = hardSetRasterZoomLevel;
                else //it's a vector type
                    zoomLevelToGrab = hardSetVecTileZoomLevel;
#else
                int zoomLevelToGrab = numZoomLevels - 1;
#endif
                int previousLowestTileX = 0;
                int previousHighestTileX = 0;
                int previousLowestTileY = 0;
                int previousHighestTileY = 0;
#if defined(__APPLE__)
                //int lowerIndex = 0;
                int i = 0;
                if(tiletype == TTRASTER)
                {
                    i = hardSetRasterZoomLevel;//medium raster tile resolution
                }
                else{ //it's a vectortile type
                    i = hardSetVecTileZoomLevel;//highest vector tile resolution
                }
                //lowerIndex = numZoomLevels - 1;
                //for(int i = lowerIndex ; i < numZoomLevels ; i++)
                
#else
                for(int i = 0 ; i < numZoomLevels ; i++)
#endif
                {
                    int lowestTileX = long2tilex(lowestLon, i);
                    int highestTileX = long2tilex(highestLon, i);
                    int lowestTileY = lat2tiley(highestLat, i);
                    int highestTileY = lat2tiley(lowestLat, i);
                    int deltaX = highestTileX - lowestTileX;
                    int deltaY = highestTileY - lowestTileY;
#if defined(__APPLE__)
#else
                    if(deltaX * deltaY > _maxTilesToRender && i != 0)
                    {
                        zoomLevelToGrab = i - 1;
                        break;//we're done
                    }
#endif
                    previousLowestTileX = lowestTileX;
                    previousHighestTileX = highestTileX;
                    previousLowestTileY = lowestTileY;
                    previousHighestTileY = highestTileY;
                }
                minTileX = previousLowestTileX - 2;
                minTileY = previousLowestTileY - 2;
                maxTileX = previousHighestTileX + 2;
                maxTileY = previousHighestTileY  + 2;
                
                return zoomLevelToGrab;
            }
            
            void LFClientEngine::DataLoadFunction(){
                //cout<<"data load function"<<endl;
            }
            
            //todo: rename this thread pool function
            int LFClientEngine::tileDataLoadTask(int seconds, boost::shared_ptr<tile> newTile) {
                newTile->DataLoadFunction();
                newTile.reset();
                return 0;
            }
            
            void LFClientEngine::push_job(int seconds, boost::asio::io_service& io_service, std::vector<boost::shared_future<int> >& pending_data, boost::shared_ptr<tile> newTile)
            {
                ptask_t task = boost::make_shared<packed_task_t>(boost::bind(&LFClientEngine::tileDataLoadTask, this, seconds, newTile));
                boost::shared_future<int> fut(task->get_future());
                io_service.post(boost::bind(&packed_task_t::operator(), task));
            }
            
            void printVdata(boost::shared_ptr<SViewData> vdata){
                cout<<"lat"<<vdata->dVehicleLat<<endl;
                cout<<"lon"<<vdata->dVehicleLon<<endl;
                cout<<"alt"<<vdata->dVehicleAltitude<<endl;
                cout<<"p"<<vdata->dVehiclePitch <<endl;
                cout<<"h"<<vdata->dVehicleHeading<<endl;
                cout<<"r"<<vdata->dVehicleRoll<<endl;
                
                cout<<"camp"<<vdata->dCameraPitch<<endl;
                cout<<"camh"<<vdata->dCameraHeading<<endl;
                cout<<"camr"<<vdata->dCameraRoll<<endl;
            }
            
            //this tells us which tile in the current render list is the oldest in terms
            //of symbol render order refresh
            bool compareTileSortNum(boost::shared_ptr<tile> i1, boost::shared_ptr<tile> i2)
            {
                return (i1->GetSortOrderNum() < i2->GetSortOrderNum());
            }
            //this tells us which tile in the current render list is the oldest in terms
            //of symbol render order refresh
            void LFClientEngine::RefreshRenderOrderSubEntities(boost::shared_ptr<vector<boost::shared_ptr<tile> > > newRenderList)
            {
                //foreach through newRenderList
                if(newRenderList->size() > 1)
                {
                    std::vector<boost::shared_ptr<tile> > vRenderList = (*(newRenderList.get()));
                    sort(vRenderList.begin(), vRenderList.end(), compareTileSortNum);//sort in ascending sortordernum
                    boost::shared_ptr<tile> t = vRenderList[0];
                    t->SortEntities(_viewData->dVehicleLat, _viewData->dVehicleLon);
                }
            }
            
            
            bool compareTileDistToVehicle(boost::shared_ptr<tile> i1, boost::shared_ptr<tile> i2)
            {
                return (i1->GetDistToVehicleQuick() > i2->GetDistToVehicleQuick());
            }
            void LFClientEngine::RefreshTileRenderOrder(boost::shared_ptr<vector<boost::shared_ptr<tile> > > newRenderList)
            {
                vector<boost::shared_ptr<tile> > tiles = (*(newRenderList.get()));
                for(int i = 0 ; i < tiles.size() ;i ++)
                {
                    boost::shared_ptr<tile> t = tiles[i];
                    t->UpdateDistToVehicle(_viewData->dVehicleLat, _viewData->dVehicleLon);
                }
                //foreach through newRenderList
                if(newRenderList->size() > 1)
                {
                    sort(newRenderList->begin(), newRenderList->end(), compareTileDistToVehicle);//sort in ascending sortordernum
                }
                vector<boost::shared_ptr<tile> > tiles2 = (*(newRenderList.get()));
            }
            

            
            void LFClientEngine::SetupRenderList(int minTileY, int maxTileY, int minTileX, int maxTileX, bool& listChange,
                                                 boost::shared_ptr<vector<boost::shared_ptr<tile> > > newRenderList, TILETYPE type, int zoomLevel)
            {
                
                
                int tileLoadCount = 0;
                boost::shared_ptr<vector<boost::shared_ptr<tile> > > tileCache ;
                boost::shared_ptr<vector<boost::shared_ptr<tile> > > tileRenderList ;
                tileCache = _rasterTileCache[zoomLevel] ; //set it to something to avoid segfault
                tileRenderList = _rasterTileRendering[zoomLevel];
                switch(type)
                {
                    case TTRASTER:
                        tileCache = _rasterTileCache[zoomLevel];
                        tileRenderList = _rasterTileRendering[zoomLevel];
                        break;
                    case TTVECTOR:
                        tileCache = _vectorTileCache[zoomLevel];
                        tileRenderList = _vectorTileRendering[zoomLevel];
                        break;
                }
                
                
                for(int y =  minTileY; y <= maxTileY ; y++)
                {
                    for(int x =  minTileX; x <= maxTileX ; x++)
                    {
                        boost::shared_ptr<tile> tileptr;
                        tileptr = SearchForTileOnList(tileRenderList, x, y);
#if defined(__APPLE__)
                        boost::shared_ptr<tile>  tileptr2 = SearchForTileOnList(tileCache, x, y);
#endif
                        if(tileptr )//= SearchForTileOnList(_vecTileRendering[_currentZoomLevel], x, y))
                        {
                            //if we get a tileptr back, that tile is on the current/old render list
                            //push it onto the new/future render list
                            newRenderList->push_back(tileptr);
                            
                        }
#if defined(__APPLE__)
                        else if(tileptr2 )
                        {
                            //copy the share_ptr to the rendering stack
                            listChange = true;
                            //tileptr = tileptr2;
                            newRenderList->push_back(tileptr2);
                        }
#else
                        else if(tileptr = SearchForTileOnList(tileCache, x, y) )
                        {
                            //copy the share_ptr to the rendering stack
                            listChange = true;
                            newRenderList->push_back(tileptr);
                        }
#endif
                        else
                        {
                            if(tileLoadCount < 6)///do some load balancing since this occurs each frame
                            {
                                if(!(type == TTRASTER) || (type == TTRASTER && _minRasterZoomLevel < zoomLevel))
                                {
                                    boost::shared_ptr<tile> newTile;
                                    switch(type)
                                    {
                                        case TTRASTER:
                                            
                                            //newTile.reset(new RasterTile(x, y, zoomLevel, _usettingsdata, _statusLogger, _coordConverter));
                                            break;
                                        case TTVECTOR:
                                        {
                                            boost::mutex::scoped_lock lock(glbPlacemarkMutex);
                                            newTile.reset(new VectorTile(x, y, zoomLevel, _usettingsdata, _statusLogger, _coordConverter,
                                                                         _symbolTextures, _placemarkCollection, _poiIconMap, _placeIconMap,
                                                                         _customSymbolList, 5.0,_autoScaleOffsetStVecs, _autoScaleOffsetIndices,
                                                             _autoScaleOffsetSize, _autoScaleStVecStride, _autoScaleIndicesStride));
                                        }
                                            break;
                                    }
                                    newTile->SetEML(_EML);
                                    newTile->SetFont(_openglFont);
                                    push_job(3, io_service, pending_data, newTile);
                                    newRenderList->push_back(newTile);
                                    tileCache->push_back(newTile);
                                    listChange = true;
                                    tileLoadCount++;
                                }
                            }
                        }
                    }
                }
            }
            void LFClientEngine::GetLatLon(int screenX, int screenY, int screenWidth, int screenHeight, double* lat, double* lon, int vpwidth, int vpheight) {
                double elev;
                _footPrintCalculator->GetLatLon4Pixels(_viewData, screenX, screenY, lat, lon, &elev, screenWidth, screenHeight, vpwidth, vpheight);
            }
            int LFClientEngine::AddUserPoint(int screenX, int screenY, double* lat, double* lon, double* elev, int screenWidth, int screenHeight, std::string name, int vpwidth, int vpheight)
            {
                int range = _footPrintCalculator->GetLatLon4Pixels(_viewData, screenX, screenY, lat, lon, elev, screenWidth, screenHeight, vpwidth, vpheight);
                {
                    //DO THIS WHEN CHECKING QUEUE ON RENDER SIDE ONLY!!
                    std::stringstream customHref;//
                    std::string msg("adding custom symbol; ");
                    msg.append(customHref.str());
                    
                    WriteToStatusLog(msg, false, true);
                    double xcart = 0.00;
                    double ycart = 0.00;
                    double zcart = 0.00;
                    *elev = _EML->GetElevation(*lat, *lon);
                    GLuint whiteTexture = findTextureBasedOnHref("white.png");
                    std::string strCustomHref(customHref.str());
                    GLuint texture = findTextureBasedOnHref("pinred.png");
                    _coordConverter->GeodeticToCartesian(*lat, *lon, *elev, xcart, ycart, zcart);
                    boost::shared_ptr<iconInfo> iinfo;
                    iinfo = (*(_poiIconMap.get()))["custom"];
                    if( !name.empty() && texture )//pinred filter doesn't work??
                    {
                        boost::shared_ptr<symbol> sym(new symbol(*lat, *lon, xcart, ycart, *elev, texture, _openglFont, name, _userSettingsData,
                                                                 _statusLogger, whiteTexture, iinfo));
                        boost::shared_ptr<KmlPathDetails> kmlPathDetails(new KmlPathDetails());
                        kmlPathDetails->_filename = "User Points";
                        kmlPathDetails->_element = name;
                        sym->SetKmlPathDetails(kmlPathDetails);
                        sym->InitOpenGLEntities();
                        _customSymbolList->push_back(sym);
                    }
                }
                return range;
            }
        
#if defined(__APPLE__)
            void CalculateFOVFromDiagFOV(double diagFOV, double frameWidth, double frameHeight, double& vfov, double& hfov)
            {
                double diagSize = sqrt(frameWidth * frameWidth + frameHeight * frameHeight);
                double horizontalFraction = frameWidth / diagSize;
                double verticalFraction = frameHeight / diagSize;
                
                hfov = diagFOV * horizontalFraction;
                vfov = diagFOV * verticalFraction;
            }
            void LFClientEngine::StartRecordingFlightData()
            {
                if(!_initialized)
                    Init();
                _statusLogger->OpenLogFile();
                //_statusLogger->Init(_usettingsdata);
                _recordingFlightData = true;
                //cout<<"start recording"<<endl;
            }
            void LFClientEngine::StopRecordingFlightData()
            {
                if(_recordingFlightData == true)
                {
                    _statusLogger->CloseCurrentLogFile();
                    //lets close out the log file
                    //shutdown(<#int#>, <#int#>);
                }
                _recordingFlightData = false;
                //cout<<"stop recording"<<endl;
            }
            double testrot = 0.00;
#include "RIMatrixMath.hpp"
#endif
#if defined(__APPLE__)
            
            //TODO: REMOVE LATER
            //static const GLfloat lftestdata[] = {
            //    0.0f, 0.0f, -0.95f, -0.95f, 1.0f,
            //    0.0f, 1.0f, -0.95f, 0.95f, 1.0f,
            //    1.0f, 0.0f, 0.95f,  -0.95f, 1.0f,
            //
            //    1.0f, 0.0f, 0.95f,  -0.95f, 1.0f,
            //    0.0f, 1.0f, -0.95f, 0.95f, 1.0f,
            //    1.0f, 1.0f, 0.95f, 0.95f, 1.0f
            //};


            void LFClientEngine::InitAutoscaleShader()
            {
                static const char * vShaderStr[] =
                {
                    "#version 300 es                                \n"
                    //"#version 450 core                                                          \n"
                    "in vec2 texcoord;                                                          \n"
                    "in vec4 position;                                                          \n"
                    "in vec3 center;                                                            \n"
                    "in float streetIndex;                                                         \n"
                    "uniform float scale;                                                            \n"
                    "uniform vec3 vehLocation;                                                  \n"
                    "uniform vec3 targetLocation;                                               \n"
                    "uniform vec4 labelColor;                                               \n"
                    //DENSITY: ADD UNIFORM FOR MODE 1 = CENTER TEXT ON FULL LINE, 2 = FIND 1/4 POSITION 3 = FIND 3/4 POSITION
                    "uniform int lblDensityLevel;                                                   \n"
                    "float endpoint1x;                                                          \n"
                    "float endpoint1y;                                                          \n"
                    "float endpoint2x;                                                          \n"
                    "float endpoint2y;                                                          \n"
    
                    "layout(std140) uniform StreetVertices                                          \n"
                    "{                                                                              \n"
                    "   int numStreets;                                                               \n"
                    "   int streetIndices[200];                                                   \n"//50
                    "   vec3 translation[800];                                                           \n"//200
                    "} street_vertices;                                                               \n"
                    "out VS_OUT                                                                             \n"
                    "{                                                                                      \n"
                    "   vec4 color;                                                                         \n"
                    "   vec2 texcoordd;                                                                     \n"
                    "} vs_out;                                                                              \n"
                    "uniform mat4 mv_matrix;                                                                \n"
                    "uniform mat4 proj_matrix;                                                              \n"
                    "uniform mat4 mvp;                                                                      \n"//todo: remove??
                    "void main(void)                                                                        \n"
                    "{                                                                                      \n"
                    "   int sindex = int(center.z);                                                         \n"
                    //"   int sindex = lblDensityLevel;                                                         \n"
                    "   int startIndex = int(street_vertices.streetIndices[sindex]);                        \n"
                    "   int endIndex = (int(street_vertices.streetIndices[sindex + 1]))-1;                  \n"
                    "   int numStreetVerts = int(street_vertices.streetIndices[sindex + 1]) - startIndex;    \n"
                    "   float vehX = vehLocation.x;                                                         \n"
                    "   float vehY = vehLocation.y;                                                         \n"
                    "   float targetX = targetLocation.x;                                                         \n"
                    "   float targetY = targetLocation.y;                                                         \n"
    
                    "   float startX = street_vertices.translation[startIndex].x;                           \n"
                    "   float startY = street_vertices.translation[startIndex].y;                           \n"
                    "   float endX = street_vertices.translation[endIndex].x;                           \n"
                    "   float endY = street_vertices.translation[endIndex].y;                           \n"
                    "   float deltaXLine = endX - startX;                                  \n"
                    "   float deltaYLine = endY - startY;                                  \n"
    
                    //"   float targetX = startX;                                                         \n"
                    //"   float targetY = startY;                                                         \n"
                    "   float deltaXVeh = targetX - vehX;                                                   \n"
                    "   float deltaYVeh = targetY - vehY;                                                   \n"
    
    
                    // double zcross = deltaLonVeh * deltaLatLine - deltaLatVeh * deltaLonLine;
                    "   float zcross = deltaXVeh * deltaYLine - deltaYVeh * deltaXLine;                     \n"
                    "   float newCenterX = center.x;                                                                 \n"
                    "   float newPositionX = position.x;                                                                 \n"
                    "   float newPositionY = position.y;                                                                 \n"
                    "   if(zcross > 0.0){                                                                    \n"
                    "       newCenterX = center.x * (-1.0);                                                     \n"
                    "       newPositionX = position.x * (-1.0);                                                   \n"
                    "       newPositionY = position.y * (-1.0);                                                   \n"
                    "   }                                                                                       \n"
                                       ///////////////end new stuff
                    //we'll use this as the index into the street array, tells us what street this
                    //vert belongs too
    
                    "   float streetlength = 0.00;                                                  \n"
                    "   for(int i = 0 ; i < numStreetVerts - 1; i++)                                                 \n"
                    "   {                                                                           \n"
                    "       float dist = distance(street_vertices.translation[i + startIndex],                    \n"
                    "                               street_vertices.translation[i+ 1 + startIndex]);               \n"
                    "       streetlength += dist;                                                   \n"
                    "   }                                                                           \n"
                    //DENSITY: I THINK ALL WE NEED TO DO IS MOVE THE CENTER POINT IE IF WE ARE IN MODE2, THEN CENTERDIST = STREETLENGTH/4.0
                    "   //find center pt length                                                     \n"
                    //DENSITY: IF MODE = 1, DIVIDE BY 2 IF =2 DIVIDE BY4, IF 3 FIND 3/4 POSITION ETC..
                    "   float centerdist = streetlength / 2.0;                                      \n"
                    //DENSITY: NEW TEST CODE
                    "   if(lblDensityLevel == 2){                                                   \n"
                    "       centerdist = streetlength * 0.25;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 3){                                              \n"
                    "       centerdist = streetlength * 0.75;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 4){                                              \n"
                    "       centerdist = streetlength * 0.125;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 5){                                              \n"
                    "       centerdist = streetlength * 0.375;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 6){                                              \n"
                    "       centerdist = streetlength * 0.625;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 7){                                              \n"
                    "       centerdist = streetlength * 0.875;                                       \n"
                    "   }                                                                           \n"
                    
                    
                    "   else if(lblDensityLevel == 8){                                              \n"
                    "       centerdist = streetlength * 0.0625;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 9){                                              \n"
                    "       centerdist = streetlength * 0.1875;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 10){                                              \n"
                    "       centerdist = streetlength * 0.3125;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 11){                                              \n"
                    "       centerdist = streetlength * 0.4375;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 12){                                              \n"
                    "       centerdist = streetlength * 0.5625;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 13){                                              \n"
                    "       centerdist = streetlength * 0.6875;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 14){                                              \n"
                    "       centerdist = streetlength * 0.8125;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 15){                                              \n"
                    "       centerdist = streetlength * 0.9375;                                       \n"
                    "   }                                                                           \n"
                    ///////////new
                    "   else if(lblDensityLevel == 16){                                              \n"
                    "       centerdist = streetlength * 0.03125;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 17){                                              \n"
                    "       centerdist = streetlength * 0.09375;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 18){                                              \n"
                    "       centerdist = streetlength * 0.15625;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 19){                                              \n"
                    "       centerdist = streetlength * 0.21875;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 20){                                              \n"
                    "       centerdist = streetlength * 0.28125;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 21){                                              \n"
                    "       centerdist = streetlength * 0.34375;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 22){                                              \n"
                    "       centerdist = streetlength * 0.40625;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 23){                                              \n"
                    "       centerdist = streetlength * 0.46875;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 24){                                              \n"
                    "       centerdist = streetlength * 0.53125;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 25){                                              \n"
                    "       centerdist = streetlength * 0.59375;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 26){                                              \n"
                    "       centerdist = streetlength * 0.65625;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 27){                                              \n"
                    "       centerdist = streetlength * 0.71875;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 28){                                              \n"
                    "       centerdist = streetlength * 0.78125;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 29){                                              \n"
                    "       centerdist = streetlength * 0.84375;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 30){                                              \n"
                    "       centerdist = streetlength * 0.90625;                                       \n"
                    "   }                                                                           \n"
                    "   else if(lblDensityLevel == 31){                                              \n"
                    "       centerdist = streetlength * 0.96875;                                       \n"
                    "   }                                                                           \n"
                    //DENSITY: END NEW TEST CODE
                    "   //find scaled centerpt of this vertices x = newCenterX;                     \n"
                    "   float centerofcharlocation = centerdist + (newCenterX * scale);                    \n"
                    "   float deltacentercharlocation = centerofcharlocation;                       \n"
                    "   bool done = false;                                                          \n"
                    "   vec3 vCenterOfCharLocation;                                                 \n"
                    "   float rotangle;                                                             \n"
                    "   for(int i = 0 ; i < numStreetVerts - 1 && done == false; i++)               \n"
                    "   {                                                                           \n"
                    "       vec3 startpt = street_vertices.translation[i + startIndex];                          \n"
                    "       vec3 endpt = street_vertices.translation[i + 1 + startIndex];                          \n"
                    "       float distBetweenCurrPts = distance(startpt, endpt);                    \n"
                    "       deltacentercharlocation -= distBetweenCurrPts;                          \n"
                    "       if(deltacentercharlocation < 0.0)                                         \n"
                    "       {                                                                       \n"
                    "           done = true;                                                        \n"
                    "           float percentOfWay = (distBetweenCurrPts + deltacentercharlocation) / distBetweenCurrPts;   \n"
                    "           vCenterOfCharLocation = mix(startpt, endpt, percentOfWay);          \n"
                    "           float deltax = endpt.x - startpt.x;                                 \n"
                    "           float deltay = endpt.y - startpt.y;                                 \n"
                    "           rotangle = atan(deltay, deltax);                                 \n"
                    "       }                                                                       \n"
                    "   }                                                                           \n"
                    "   float ddnewx = newPositionX - newCenterX;                                     \n"
                    "   float ddnewy = newPositionY;                                                \n"
                    "   float ddnewz = position.z;                                                \n"
                    "   vec4 ddnewPoint = vec4(ddnewx, ddnewy, ddnewz, 1.0);                            \n"
                    "   //rotate about center of character                                      \n"
                    "   mat4 ddrotMatrix ;                                                        \n"
                    "   ddrotMatrix[0] = vec4(cos(rotangle), sin(rotangle), 0.0, 0.0);                  \n"
                    "   ddrotMatrix[1] = vec4(-1.0 * sin(rotangle), cos(rotangle), 0.0, 0.0);                 \n"
                    "   ddrotMatrix[2] = vec4(0.0, 0.0, 1.0, 0.0);                                    \n"
                    "   ddrotMatrix[3] = vec4(0.0, 0.0, 0.0, 1.0);                                    \n"
                    "   vec4 ddrotnewpoint = ddrotMatrix * ddnewPoint;                                  \n"
                    "   ddnewx = ddrotnewpoint.x  ;                                                   \n"
                    "   ddnewy = ddrotnewpoint.y ;                                                    \n"
                    "   ddnewz = ddrotnewpoint.z;                                                    \n"
                    //we are now centered at origin and rotated
                    "   //scale the vertices for the character out by the scale                     \n"
                    "   vec4 ddnewPosition = vec4(ddnewx * scale, ddnewy * scale, ddnewz, 1.0);             \n"
                    //move to the point on the street line
                    "   vec4 ddnewPosXlatRot = vec4(ddnewPosition.x + vCenterOfCharLocation.x,      \n"
                    "           ddnewPosition.y + vCenterOfCharLocation.y, vCenterOfCharLocation.z, 1.0);                 \n"
                    "   int ssdgd = street_vertices.streetIndices[1]; //int(street_vertices.streetIndices[1]) ;               \n"
                   //"   if(startIndex == 3)                                            \n"
                    "       gl_Position = proj_matrix * mv_matrix * ddnewPosXlatRot;                       \n"
                    "   vs_out.color = labelColor;                                                  \n"
                    "   vs_out.texcoordd = texcoord;                                                \n"
                     // " gl_Position = proj_matrix * mv_matrix * position;                             \n"
                    "}\n"
                    
                };
                
                static const char * fShaderStr[] =
                {
                    "#version 300 es                                \n"
                    "precision mediump float;                       \n"
                    "out vec4 color;                                \n"
                    "uniform sampler2D s;                           \n"
                    "in VS_OUT                                      \n"
                    "{                                              \n"
                    "   vec4 color;                                 \n"
                    "   vec2 texcoordd;                             \n"
                    "} fs_in;                                       \n"
                    "void main(void)                                \n"
                    "{                                              \n"
                    "   color = texture(s, fs_in.texcoordd);        \n"
                    "   color = color * fs_in.color;                \n"
                    "}                                              \n"
                    
//                    "precision mediump float;                               \n"
//                    "varying lowp vec2 v_texCoord;                               \n"
//                    "uniform sampler2D u_texture;                            \n"
//                    "varying lowp vec4 colorVarying;                             \n"
//                    "void main()                                            \n"
//                    "{                                                      \n"
//                    "   vec4 col = texture2D(u_texture, v_texCoord);          \n"
//                    "   col.x = colorVarying.x * col.x;                     \n"
//                    "   col.y = colorVarying.y * col.y;                     \n"
//                    "   col.z = colorVarying.z * col.z;                     \n"
//                    "   col.w = colorVarying.w * col.w;                     \n"
//                    "   gl_FragColor =  col;                                \n"
//                    "}                                                      \n"
                    
                };
                
                GLint compiled;
                char buffer[1024];
                GLuint vertexShader;
                GLuint fragmentShader;
                GLint linked;
                //glUniformBlockBinding(_risShaderProgramAutoScale, 0, 50);
                vertexShader = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertexShader, 1, vShaderStr, NULL);
                glCompileShader(vertexShader);
                glGetShaderInfoLog(vertexShader, 1024, NULL, buffer);
                glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
                std::cout<<compiled<<endl;
                
                fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragmentShader, 1, fShaderStr, NULL);
                glCompileShader(fragmentShader);
                glGetShaderInfoLog(fragmentShader, 1024, NULL, buffer);
                glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
                std::cout<<compiled<<endl;
                
                _risShaderProgramAutoScale = glCreateProgram();
                
                glAttachShader(_risShaderProgramAutoScale, vertexShader);
                glAttachShader(_risShaderProgramAutoScale, fragmentShader);
                glLinkProgram(_risShaderProgramAutoScale);
                //(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog)
                glGetProgramInfoLog(_risShaderProgramAutoScale, 1024, NULL, buffer);
                glGetShaderiv(_risShaderProgramAutoScale, GL_LINK_STATUS, &linked);
                std::cout<<linked<<endl;
                glUseProgram(_risShaderProgramAutoScale);
                //todo: fix this
                // Get the attribute locations
//                _apositionLocation = glGetAttribLocation ( _risShaderProgramAutoScale, "a_position" );
//                _atextureLocation = glGetAttribLocation ( _risShaderProgramAutoScale, "a_texCoord" );
//
//                // Get the sampler location
                //GLuint color2 = glGetUniformLocation ( _risShaderProgramAutoScale, "u_lineColor2" );
                //gbl_u_colorLocation = glGetUniformLocation ( _risShaderProgramAutoScale, "u_lineColor" );
                gbl_u_mvpLocationAutoScale = glGetUniformLocation(_risShaderProgramAutoScale, "mv_matrix");
                gbl_u_projLocationAutoScale = glGetUniformLocation(_risShaderProgramAutoScale, "proj_matrix");
                gbl_u_lblDensityLevelLocationAutoScale = glGetUniformLocation(_risShaderProgramAutoScale, "lblDensityLevel");
                
                gbl_u_scaleLocationAutoScale = glGetUniformLocation ( _risShaderProgramAutoScale, "scale" );
                gbl_u_vehLocationAutoScale = glGetUniformLocation(_risShaderProgramAutoScale, "vehLocation");
                gbl_u_targetLocationAutoScale = glGetUniformLocation(_risShaderProgramAutoScale, "targetLocation");
                
                gbl_u_stLabelColorLocationAutoScale = glGetUniformLocation(_risShaderProgramAutoScale, "labelColor");
                gbl_u_SamplerLocationAutoScale = glGetUniformLocation ( _risShaderProgramAutoScale, "s" );
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);
                
                
            }
            void LFClientEngine::InitShaders()
            {
                static const char * vShaderStr[] =
                {
                    "uniform mat4 u_mvpMatrix;                      \n"
                    "uniform vec4 u_lineColor;                        \n"
                    "attribute vec4 a_position;                     \n"
                    "attribute vec2 a_texCoord;                     \n"
                    "varying lowp vec2 v_texCoord;                       \n"
                    "varying lowp vec4 colorVarying;                     \n"
                    "void main()                                    \n"
                    "{                                              \n"
                    "   gl_Position = u_mvpMatrix * a_position;     \n"
                    "   v_texCoord = a_texCoord;                    \n"
                    "   colorVarying = u_lineColor;                   \n"
                    "}                                              \n"
                    
                };
                
                static const char * fShaderStr[] =
                {
                    "precision mediump float;                               \n"
                    "varying lowp vec2 v_texCoord;                               \n"
                    "uniform sampler2D u_texture;                            \n"
                    "varying lowp vec4 colorVarying;                             \n"
                    "void main()                                            \n"
                    "{                                                      \n"
                    "   vec4 col = texture2D(u_texture, v_texCoord);          \n"
                    "   col.x = colorVarying.x * col.x;                     \n"//mix with color from cpu generated color
                    "   col.y = colorVarying.y * col.y;                     \n"
                    "   col.z = colorVarying.z * col.z;                     \n"
                    "   col.w = colorVarying.w * col.w;                     \n"//alpha value
                    "   gl_FragColor =  col;                                \n"
                    "}                                                      \n"
                    
                };
                
                GLint compiled;
                char buffer[1024];
                GLuint vertexShader;
                GLuint fragmentShader;
                GLint linked;
                
                vertexShader = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertexShader, 1, vShaderStr, NULL);
                glCompileShader(vertexShader);
                glGetShaderInfoLog(vertexShader, 1024, NULL, buffer);
                glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
                std::cout<<compiled<<endl;
                
                fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragmentShader, 1, fShaderStr, NULL);
                glCompileShader(fragmentShader);
                glGetShaderInfoLog(fragmentShader, 1024, NULL, buffer);
                glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
                std::cout<<compiled<<endl;
                
                _risShaderProgram = glCreateProgram();
                glAttachShader(_risShaderProgram, vertexShader);
                glAttachShader(_risShaderProgram, fragmentShader);
                glLinkProgram(_risShaderProgram);
                //(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog)
                glGetProgramInfoLog(_risShaderProgram, 1024, NULL, buffer);
                glGetShaderiv(_risShaderProgram, GL_LINK_STATUS, &linked);
                std::cout<<linked<<endl;
                
                // Get the attribute locations
                _apositionLocation = glGetAttribLocation ( _risShaderProgram, "a_position" );
                _atextureLocation = glGetAttribLocation ( _risShaderProgram, "a_texCoord" );
                
                // Get the sampler location
                //GLuint color2 = glGetUniformLocation ( _risShaderProgram, "u_lineColor2" );
                gbl_u_colorLocation = glGetUniformLocation ( _risShaderProgram, "u_lineColor" );
                gbl_u_mvpLocation = glGetUniformLocation(_risShaderProgram, "u_mvpMatrix");
                gbl_u_SamplerLocation = glGetUniformLocation ( _risShaderProgram, "u_texture" );
                
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);
                
                InitAutoscaleShader();
                return ;
            }
#endif
//#if defined(__APPLE__)
            int LFClientEngine::Render( double lowerLeftX, double lowerLeftY, double windowWidth,
                                       double windowHeight,  boost::shared_ptr<SViewData> viewData)
            {
                
                if(!_overlaysOn)
                    return 0;
                
                glBindVertexArrayOES(vao);
                //glBindFramebuffer(GL_FRAMEBUFFER, 2);
                //glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, _framebuffer);
                //glEnable(GL_DEPTH_TEST);
                double vfov = viewData->dfov;
                this->SetVP(viewData);
                
                double groundElev = _EML->GetElevation(viewData->dVehicleLat, viewData->dVehicleLon);
                viewData->dVehicleAltitude += groundElev;
                
                if(_recordingFlightData == true)
                    WriteToStatusLog(viewData);
//                if(_regenKml)
//                {
//                    AddKMLFileRegen();//re-add the kml entities from the previous instance of lfclient in case user clicked map and destroyed/recreated lfclient
//                    _regenKml = false;
//                }
                boost::mutex::scoped_lock lock(_kmlFileReadMutex);//make sure client thread doesn' mess with symbols/adding kml while we're in the middle
                //of rendering
                
                if(_newKmlFile){
                    //the client(from another thread) added a kmlfile, we'll do the actual loading of it here
                    //since all opengl entities need to be created from the main render thread
                    AddKMLEntitiesFromFile(_kmlFilename, false);
                    _newKmlFile = false;
                }
                double camx = 0.00;
                double camy = 0.00;
                double camz = 0.00;
                
                _coordConverter->GeodeticToCartesian(viewData->dVehicleLat, viewData->dVehicleLon, viewData->dVehicleAltitude , camx, camy, camz);
                
                glUseProgram(_risShaderProgram);
                
                glBindVertexArrayOES(vao);
                _windowWidth = windowWidth;
                _windowHeight = windowHeight;
                glViewport(lowerLeftX, lowerLeftY, windowWidth, windowHeight);
                
                //set default draw color to white
                //TODO SET COLOR FOR SHADE
                int minTileXRaster = 0;
                int minTileYRaster = 0;
                int maxTileXRaster = 0;
                int maxTileYRaster = 0;
                _currentRasterZoomLevel = GetZoomLevel(minTileXRaster, minTileYRaster, maxTileXRaster, maxTileYRaster, TTRASTER);
                
                int minTileXVecTile = 0;
                int minTileYVecTile = 0;
                int maxTileXVecTile = 0;
                int maxTileYVecTile = 0;
                _currentVecTileZoomLevel = GetZoomLevel(minTileXVecTile, minTileYVecTile, maxTileXVecTile, maxTileYVecTile, TTVECTOR);
                boost::shared_ptr<vector<boost::shared_ptr<tile> > > newRasterRenderList(new vector<boost::shared_ptr<tile> >);
                boost::shared_ptr<vector<boost::shared_ptr<tile> > > newVecTileRenderList(new vector<boost::shared_ptr<tile> >);
                int tileLoadCount = 0;
                bool listChangeRaster = false;//if the list hasn't changed, we don't need to swap to a new render list, just keep moving
                bool listChangeVecTile = false;
                
                SetupRenderList(minTileYVecTile, maxTileYVecTile, minTileXVecTile, maxTileXVecTile,
                                listChangeVecTile, newVecTileRenderList, TTVECTOR, _currentVecTileZoomLevel);
                
                //SetupRenderList(minTileYRaster, maxTileYRaster, minTileXRaster, maxTileXRaster, listChangeRaster, newRasterRenderList,
                  //              TTRASTER, _currentRasterZoomLevel);
                
                
                
                if(listChangeRaster == true)
                {
                    
                    //CACHESIZELIMIT
                    //swap old render list with new one, eventually place lock around cause this will need to happen on other thread.
                    //lock here and wherever render thread accesses the render list
                    for(int i = 0 ; i < NUMOFZOOMLEVELSRASTER ; i++)
                    {
                        while(_rasterTileCache[i]->size() > _rasterTileCacheLimits[i])
                        {
                            _rasterTileCache[i]->pop_back();
                        }
                    }
                    _rasterTileRendering[_currentRasterZoomLevel] = newRasterRenderList;
                }
                
                if(listChangeVecTile == true)
                {
                    
                    //CACHESIZELIMIT
                    //swap old render list with new one, eventually place lock around cause this will need to happen on other thread.
                    //lock here and wherever render thread accesses the render list
                    for(int i = 0 ; i < NUMOFZOOMLEVELSVECTILE ; i++)
                    {
                        while(_vectorTileCache[i]->size() > _vectorTileCacheLimits[i])
                        {
                            _vectorTileCache[i]->pop_back();
                        }
                    }
                    _vectorTileRendering[_currentVecTileZoomLevel] = newVecTileRenderList;
                }
                
                
                RIMatrix4x4 riProj_matrix;
                riProj_matrix.SetToPerspectiveMatrix(vfov, windowWidth/windowHeight, 0.1f, 1000000.0f);
                
                RIMatrix4x4 rimv_matrix;
                rimv_matrix.SetToIdentity();
                rimv_matrix = rimv_matrix.PerformTranslate(0.0f, 0.0f, 0.0f);
                rimv_matrix = rimv_matrix.PerformRotateAboutZ(viewData->dCameraRoll);
                rimv_matrix = rimv_matrix.PerformRotateAboutX((float)(-(viewData->dCameraPitch)));
                rimv_matrix = rimv_matrix.PerformRotateAboutY(viewData->dCameraHeading);
                rimv_matrix = rimv_matrix.PerformRotateAboutX(-(90.0));
                rimv_matrix = rimv_matrix.PerformTranslate((float)(-camx), (float)(-camy), (float)(-camz));
                
                RIMatrix4x4 riMVP = riProj_matrix * rimv_matrix;
                
                RIMatrix4x4 ritestid;
                ritestid.SetToIdentity();
                
                glUniformMatrix4fv(gbl_u_mvpLocation, 1, GL_FALSE, riMVP.get());
                //glUniform4fv(<#GLint location#>, <#GLsizei count#>, <#const GLfloat *v#>)
                int risglEntityLoadLimitRaster = 2;//if tiles aren't loaded yet, load at most x of them otherwise we'll bog down
                bool limitMetRaster = false;
                int risglEntityCounterRaster = 0;
                RIVector4 RIVecColorRaster(1.0, 1.0, 1.0, 0.3);
                glUniform4fv(gbl_u_colorLocation, 1, RIVecColorRaster.get());//&vecColor[0]);
                glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                glEnable(GL_BLEND);
                glActiveTexture ( GL_TEXTURE0 );
                if(_streetLinesOn)//this is attached to the switch in the gui
                {
//                    for(int i = 0 ; i < newRasterRenderList->size() ; i++)
//                    {
//                        vector<boost::shared_ptr<tile> > tilelist = *(newRasterRenderList.get());
//                        boost::shared_ptr<tile> ptrTile = tilelist[i];
//
//                        //bool loadedEntities = ptrTile->RenderPerspectiveEntities(_risShaderProgram, limitMetRaster, _viewData, riProj_matrix, _pixelSize);
//                        if(loadedEntities){
//                            risglEntityCounterRaster++;
//                        }
//                        if(risglEntityCounterRaster >= risglEntityLoadLimitRaster){
//                            limitMetRaster = true;
//                        }
//                    }
                }
                for( int i = 0 ; i < _pathways.size() ; i++)
                {
                    boost::shared_ptr<pathway> pw = _pathways[i];
                    pw->RenderPerspectiveEntities(_risShaderProgram, viewData, riMVP);
                }
                int risglEntityCounterVecTile = 0;
                int risglEntityLoadLimitVecTile = 100;
                bool limitMetVecTile = false;
                RIVector4 RIVecColorVectorTile(1.0, 1.0, 1.0, 1.0);
                //glUniform4fv(gbl_u_colorLocation, 1, RIVecColorVectorTile.get());
                //RENDER STREET LINES FIRST
                //glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, 1280, 720);
                GLboolean bval = true;
                glGetBooleanv(GL_STENCIL_TEST, &bval);
                glEnable(GL_STENCIL_TEST);
                glStencilFunc(GL_GREATER, 1, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                glStencilMask(0xFF);
                glClear(GL_STENCIL_BUFFER_BIT);
                glGetBooleanv(GL_STENCIL_TEST, &bval);
                int de = 0;
                glGetIntegerv(GL_RENDERBUFFER_DEPTH_SIZE, &de);
                glGetIntegerv(GL_STENCIL_BITS, &de);
                //glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &de);

                RIVector4 vStreetVectorColor(1.0, 1.0, 1.0, _alphaStreetVector);
                glUniform4fv(gbl_u_colorLocation, 1, vStreetVectorColor.get());
                if(_streetLinesOn)//todo: set alpha /vectors
                {
                    for(int i = 0 ; i < newVecTileRenderList->size() ; i++)
                    {
                        vector<boost::shared_ptr<tile> > tilelist = *(newVecTileRenderList.get());
                        boost::shared_ptr<tile> ptrTile = tilelist[i];
                        bool loadedEntities = ptrTile->RenderPerspectiveEntities(_risShaderProgram, limitMetRaster, viewData, riMVP, _pixelSize, OT_STREETLINE, _billboardStyle, 1.0, _labelDensity);//text scale = 1?? for now??
                        if(loadedEntities){
                            risglEntityCounterVecTile++;
                        }
                        if(risglEntityCounterVecTile >= risglEntityLoadLimitVecTile){
                            break;
                        }
                    }
                }
                glDisable(GL_STENCIL_TEST);
                
                //THEN RENDER STREET LABELS
                //float textScale = _pixelSize * 1.0;
                float textScale = _viewData->dVehicldAltitudeAGL * 0.001;
                if(_streetLabelsOn)//set alpha for street labels
                {
                    glUseProgram(_risShaderProgramAutoScale);
                    //programAutoScale->bind();

                    glUniformMatrix4fv(gbl_u_mvpLocationAutoScale, 1, GL_FALSE, rimv_matrix.get());
                    //programAutoScale->setUniformValue("mv_matrix", matrixKLV);
                    glUniformMatrix4fv(gbl_u_projLocationAutoScale, 1, GL_FALSE, riProj_matrix.get());
                    //programAutoScale->setUniformValue("proj_matrix", projection2);

                    //DENSITY: SET THE MODE IE 1 = DISPLAY LABEL AS NORMAL, 2 = SPLIT THE LINE SEGMENT IN 2 AND SHOW 2X THE LABELS
                    glUniform1f(gbl_u_scaleLocationAutoScale, (GLfloat)textScale);
                    //RIVector4 vStreetLabelColor(1.0, 1.0, 1.0, _alphaStreetLabel);
                    //glUniform4fv(gbl_u_colorLocation, 1, vStreetVectorColor.get());
                    //glUniform4fv(gbl_u_stLabelColorLocationAutoScale, 1, vStreetLabelColor.get());
                    glUniform4f(gbl_u_stLabelColorLocationAutoScale, 1.0, 1.0, 1.0, _alphaStreetLabel);
                    //programAutoScale->setUniformValue("scale", (GLfloat)textScale);
                    //RIVector3 vecLocation(camx, camy, camz);
                    //glUniform3fv(gbl_u_vehLocationAutoScale, 1, vecLocation.get());
                    glUniform3f(gbl_u_vehLocationAutoScale, camx, camy, camz);

                    double targetx = 0.00;
                    double targety = 0.00;
                    double targetz = 0.00;
                    _coordConverter->GeodeticToCartesian(_viewData->dTargetLat, _viewData->dTargetLon, _viewData->dTargetAlt, targetx, targety, targetz);
                    
                    //RIVector3 targetLocation(targetx, targety, targetz);
                    //RIVector3 targetLocation(10000.0, 10000.0, 500.0);//DENSITY: FIX!!@!
                    //glUniform3fv(gbl_u_targetLocationAutoScale, 1, targetLocation.get());
                    glUniform3f(gbl_u_targetLocationAutoScale, targetx, targety, targetz);
                    
                    for(int i = 0 ; i < newVecTileRenderList->size() ; i++)
                    {
                        vector<boost::shared_ptr<tile> > tilelist = *(newVecTileRenderList.get());
                        boost::shared_ptr<tile> ptrTile = tilelist[i];
                        if(ptrTile->_ubBindingPoint == -1)
                        {
                            int numUsedBinPoints = _uniformBlockBindings.size();
                            if(_indexUniformBufferBindList >=  _maxBindPoints){
                                _indexUniformBufferBindList = 0;
                            }
                            ptrTile->_ubBindingPoint = _indexUniformBufferBindList;
                            if(numUsedBinPoints <= _indexUniformBufferBindList)
                            {
                                _uniformBlockBindings.push_back(ptrTile);
                            }
                            else
                            {
                                std::vector<boost::shared_ptr<tile> >::iterator it;
                                it = _uniformBlockBindings.begin() + _indexUniformBufferBindList;
                                boost::shared_ptr<tile> oldTile = _uniformBlockBindings[_indexUniformBufferBindList];
                                oldTile->_ubBindingPoint = -1;
                                *it = ptrTile;
                            }
                            _indexUniformBufferBindList++;
                        }
                        GLuint tileBindingPoint = ptrTile->_ubBindingPoint;
                        if(tileBindingPoint != -1)
                            glUniformBlockBinding(_risShaderProgramAutoScale, _ublockindex, tileBindingPoint);
                        bool loadedEntities = ptrTile->RenderPerspectiveEntities(_risShaderProgramAutoScale, limitMetRaster, viewData, riMVP, _pixelSize, OT_STREETLABEL, _billboardStyle, textScale, _labelDensity);
                        if(loadedEntities){
                            risglEntityCounterVecTile++;
                        }
                        if(risglEntityCounterVecTile >= risglEntityLoadLimitVecTile){
                            break;
                        }
                    }
                    glUseProgram(_risShaderProgram);//back to the old shader for the rest of rendering
                }
                RIMatrix4x4 riProjection;
                //vmath::mat4 projection;
                riProjection.SetToOrthographicMatrix((float)0.0, (float)windowWidth, (float)0.0, (float)windowHeight, (float)0.1, (float)10.0);
                
                RefreshRenderOrderSubEntities(newVecTileRenderList);
                RefreshTileRenderOrder(newVecTileRenderList);
                //all symbols use this list to compare to see if a street label was alread drawn close within a certain range of degrees
                //from vehicle.  ie if a stlabel was drawn at 90 degrees from veh, and the next vehicle wants to draw at 93, maybe 3 dgrees
                //is too close so we compare with this list and bypass drawing this st label.
                vector<double> alreadyDrawnStLabels;
                RIVector4 vPOIColor(1.0, 1.0, 1.0, _alphaPOI);
                glUniform4fv(gbl_u_colorLocation, 1, vPOIColor.get());
                for(int i = 0 ; i < newVecTileRenderList->size() ; i++)//todo: set alpha pois
                {
                    vector<boost::shared_ptr<tile> > tilelist = *(newVecTileRenderList.get());
                    boost::shared_ptr<tile> ptrTile = tilelist[i];
                    ptrTile->RenderOrthographicEntities(_risShaderProgram, limitMetVecTile, this->_viewData, windowWidth, windowHeight, riProjection, alreadyDrawnStLabels,
                                                        _billboardStyle, _streetShieldsOn, _labelDensity);

                }
                return 1;
            }
//#else
//            int LFClientEngine::Render(QOpenGLShaderProgram *program, QMatrix4x4 projection, double lowerLeftX,
//                                       double lowerLeftY, double windowWidth, double windowHeight)
//            {
//                //{
//                boost::mutex::scoped_lock lock(_kmlFileReadMutex);//make sure client thread doesn' mess with symbols/adding kml while we're in the middle
//                //of rendering
//
//                if(_newKmlFile){
//                    //the client(from another thread) added a kmlfile, we'll do the actual loading of it here
//                    //since all opengl entities need to be created from the main render thread
//                    AddKMLEntitiesFromFile(_kmlFilename, false);
//                    _newKmlFile = false;
//                }
//                //}
//                glClearColor(0.0, 0.0, 0.0, 0.0);
//
//                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//                _windowWidth = windowWidth;
//                _windowHeight = windowHeight;
//                glViewport(lowerLeftX, lowerLeftY, windowWidth, windowHeight);
//
//                //set default draw color to white
//                QVector4D c;
//                c.setX(1.0);
//                c.setY(1.0);
//                c.setZ(1.0);
//                c.setW(_alpha);
//
//
//                program->setUniformValue("lineColor", c);
//                QMatrix4x4 unityMatrix;
//                unityMatrix.normalMatrix();
//
//                int width = 0;
//                int height = 0;
//#if defined VIDEOPLAYER
//                if(_playVideo)
//                {
//                    boost::shared_ptr<SViewData> vdata(new SViewData);
//                    vdata = sil.GetLatestViewData();
//                    SetVP(vdata, true);
//
//                    uint8_t* videoFrameFast = sil.GetLatestVideoFast(width, height);
//                    vidPlayer.RenderFast(videoFrameFast, width, height, true, false, 3, program);
//                }
//#endif
//                //vidPlayer.Render(saVideo, width, height, true, false, 3);
//                //get lower/upper lat lon of footprint and form a box that is aligned with n/s/e/w
//                //use these values to gather tile x,y deltas of 0 - NUMOFZOOMLEVELS vectiles
//                //multiply them, and use the set that is closest to MAXTILES without going over
//                //use that zoomlevel for calculating tile grid
//                int minTileXRaster = 0;
//                int minTileYRaster = 0;
//                int maxTileXRaster = 0;
//                int maxTileYRaster = 0;
//                _currentRasterZoomLevel = GetZoomLevel(minTileXRaster, minTileYRaster, maxTileXRaster, maxTileYRaster, TTRASTER);
//
//                int minTileXVecTile = 0;
//                int minTileYVecTile = 0;
//                int maxTileXVecTile = 0;
//                int maxTileYVecTile = 0;
//                _currentVecTileZoomLevel = GetZoomLevel(minTileXVecTile, minTileYVecTile, maxTileXVecTile, maxTileYVecTile, TTVECTOR);
//                boost::shared_ptr<vector<boost::shared_ptr<tile> > > newRasterRenderList(new vector<boost::shared_ptr<tile> >);
//                boost::shared_ptr<vector<boost::shared_ptr<tile> > > newVecTileRenderList(new vector<boost::shared_ptr<tile> >);
//                int tileLoadCount = 0;
//                bool listChangeRaster = false;//if the list hasn't changed, we don't need to swap to a new render list, just keep moving
//                bool listChangeVecTile = false;
//
//                SetupRenderList(minTileYVecTile, maxTileYVecTile, minTileXVecTile, maxTileXVecTile,
//                                listChangeVecTile, newVecTileRenderList, TTVECTOR, _currentVecTileZoomLevel);
//
//                SetupRenderList(minTileYRaster, maxTileYRaster, minTileXRaster, maxTileXRaster, listChangeRaster, newRasterRenderList,
//                                TTRASTER, _currentRasterZoomLevel);
//
//
//
//                if(listChangeRaster == true)
//                {
//
//                    //CACHESIZELIMIT
//                    //swap old render list with new one, eventually place lock around cause this will need to happen on other thread.
//                    //lock here and wherever render thread accesses the render list
//                    for(int i = 0 ; i < NUMOFZOOMLEVELSRASTER ; i++)
//                    {
//                        while(_rasterTileCache[i]->size() > _rasterTileCacheLimits[i])
//                        {
//                            _rasterTileCache[i]->pop_back();
//                        }
//                    }
//                    _rasterTileRendering[_currentRasterZoomLevel] = newRasterRenderList;
//                }
//
//                if(listChangeVecTile == true)
//                {
//
//                    //CACHESIZELIMIT
//                    //swap old render list with new one, eventually place lock around cause this will need to happen on other thread.
//                    //lock here and wherever render thread accesses the render list
//                    for(int i = 0 ; i < NUMOFZOOMLEVELSVECTILE ; i++)
//                    {
//                        while(_vectorTileCache[i]->size() > _vectorTileCacheLimits[i])
//                        {
//                            _vectorTileCache[i]->pop_back();
//                        }
//                    }
//                    _vectorTileRendering[_currentVecTileZoomLevel] = newVecTileRenderList;
//                }
//
//                QMatrix4x4 projection2;
//                projection2.setToIdentity();
//                projection2.perspective(_viewData->dfov , windowWidth / windowHeight, 0.1, 1000000.0);
//
//                // Calculate model view transformati on
//                QMatrix4x4 matrixKLV;
//
//                //since the camera is mounted on the plane, we start by rotating the camera, then the plane rotations occur which will
//                //affect the current camera rotations just like in real life.
//                //rotate the camera
//                double pitch = -(_viewData->dCameraPitch) - _viewData->dVehiclePitch;
//                double heading =  _viewData->dCameraHeading + _viewData->dVehicleHeading;
//                // Feb 10 2017 MFA camera roll rotation was missing.
//
//                matrixKLV.rotate(_viewData->dCameraRoll, 0.0, 0.0, 1.0);
//                matrixKLV.rotate(pitch, 1.0, 0.0, 0.0);
//                matrixKLV.rotate(heading, 0.0, 1.0, 0.0);
//
//                matrixKLV.rotate(-90.0, 1.0, 0.0, 0.0);
//                matrixKLV.translate((GLdouble) -RAextCamx,(GLdouble) -RAextCamy,(GLdouble) -RAextCamz);
//                // Set modelview-projection matrix
//                program->setUniformValue("mvp_matrix", projection2 * matrixKLV);
//                int risglEntityLoadLimitRaster = 2;//if tiles aren't loaded yet, load at most x of them otherwise we'll bog down
//                bool limitMetRaster = false;
//                int risglEntityCounterRaster = 0;
//                WriteToStatusLog("****************Rendering tiles", false, true);
//                for(int i = 0 ; i < newRasterRenderList->size() ; i++)
//                {
//                    vector<boost::shared_ptr<tile> > tilelist = *(newRasterRenderList.get());
//                    boost::shared_ptr<tile> ptrTile = tilelist[i];
//                    bool loadedEntities = ptrTile->RenderPerspectiveEntities(program, limitMetRaster, _viewData, projection2, _pixelSize);
//                    if(loadedEntities){
//                        risglEntityCounterRaster++;
//                    }
//                    if(risglEntityCounterRaster >= risglEntityLoadLimitRaster){
//                        limitMetRaster = true;
//                    }
//                }
//                for( int i = 0 ; i < _pathways.size() ; i++)
//                {
//                    boost::shared_ptr<pathway> pw = _pathways[i];
//                    pw->RenderPerspectiveEntities(program, _viewData, projection2);
//                }
//
//                for(int i = 0 ; i < newVecTileRenderList->size() ; i++)
//                {
//                    vector<boost::shared_ptr<tile> > tilelist = *(newVecTileRenderList.get());
//                    boost::shared_ptr<tile> ptrTile = tilelist[i];
//                    bool loadedEntities = ptrTile->RenderPerspectiveEntities(program, limitMetRaster, _viewData, projection2, _pixelSize);
//                    //bool loadedEntities = false;
//                }
//
//                //set main color back to white in case kml path changed color
//                //QVector4D c;
//                c.setX(1.0);
//                c.setY(1.0);
//                c.setZ(1.0);
//                c.setW(_alpha);
//
//                //BL: don't do this each frame,use vbo
//                program->setUniformValue("lineColor", c);
//
//                //WriteToStatusLog("****************END ***************Rendering tiles", false, true);
//                //cout<<"loaded raster entities = "<<risglEntityCounterRaster<<endl;
//                int risglEntityCounterVecTile = 0;
//                int risglEntityLoadLimitVecTile = 5;
//                bool limitMetVecTile = false;
//                QMatrix4x4 _projection;
//                _projection.ortho(0, windowWidth, 0, windowHeight, 0.1, 10.0);
//
//
//                RefreshRenderOrderSubEntities(newVecTileRenderList);
//                RefreshTileRenderOrder(newVecTileRenderList);
//
//                for(int i = 0 ; i < newVecTileRenderList->size() ; i++)
//                {
//                    vector<boost::shared_ptr<tile> > tilelist = *(newVecTileRenderList.get());
//                    boost::shared_ptr<tile> ptrTile = tilelist[i];
//                    bool loadedEntities = ptrTile->RenderOrthographicEntities(program, limitMetVecTile, this->_viewData, windowWidth, windowHeight, _projection);
//                    //bool loadedEntities = true;
//                    if(loadedEntities){
//                        risglEntityCounterVecTile++;
//                    }
//                    if(risglEntityCounterVecTile >= risglEntityLoadLimitVecTile){
//                        limitMetVecTile = true;
//                    }
//                }
//                glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, 0);
//                glDisable(GL_DEPTH_TEST);
//                //todo: add drawing the new texture to a quad
//                return 1;
//            }
//#endif
            //todo: move to tile class
            boost::shared_ptr<tile> LFClientEngine::SearchForTileOnList(boost::shared_ptr<vector<boost::shared_ptr<tile> > > listToSearch, int x, int y){
                boost::shared_ptr<tile> foundTile;
                foundTile.reset();//initially it's null
                for(int i = 0 ; i < listToSearch->size() ; i++)
                {
                    vector<boost::shared_ptr<tile> > veclist = *(listToSearch.get());
                    boost::shared_ptr<tile> ptrTile = veclist[i];
                    if(ptrTile->GetX() == x && ptrTile->GetY() == y)
                    {
                        //found = true;
                        foundTile = ptrTile;
                        break;
                    }
                }
                //return -1 if not found otherwise return the index it was found at
                return foundTile;
            }
#include "math.h"
            
            void LFClientEngine::SetVP(boost::shared_ptr<SViewData> viewPointData, bool fromKLV)
            {
                if(_settingsFileFlight){
                    //move based on delta from settings file if we're in manual settings file flight mode
                    _startingLat += _manualDeltaLat;
                    _startingLon += _manualDeltaLon;
                    
                    //override settings coming in for lat/lon/alt.  zoom and panning should still work with hot keys
                    viewPointData->dVehicleLat = _startingLat;
                    viewPointData->dVehicleLon = _startingLon;
                    viewPointData->dVehicleAltitude = _startingAlt;
                }
                
                //fromKLV means that the function calling this function is/isnot using klv data ie
                //the video klv routine is saying true, the manual flight mode is saying false
                if((fromKLV == false && _playVideo == true)||(fromKLV == true && _playVideo == false))
                {
                    //we're either in video play mode and the function calling this is from manual flight mode
                    //or we're not in videoplay mode and the function calling this "is not" from the manual flight mode(videoklv)
                    //ie 2 functions are calling this, we're skipping the one that isn't relevent depending on what mode we're in
                    //for grabbing 6dof
                    return ;//skip it, we're grabbing klv to use as 6dof
                }
                else
                {
                    _viewData = viewPointData;
#if defined(__APPLE__)
#else
                    _currentFootPrint = _footPrintCalculator->ComputePrecisionFootprint(viewPointData->dVehicleLat, viewPointData->dVehicleLon,
                                                                                        viewPointData->dVehicleAltitude, viewPointData->dVehiclePitch,
                                                                                        viewPointData->dVehicleHeading, viewPointData->dVehicleRoll,
                                                                                        viewPointData->dCameraPitch, viewPointData->dCameraHeading, viewPointData->dCameraRoll,
                                                                                        viewPointData->dFovHorizontalAngle, viewPointData->dfov, this->_windowWidth, this->_windowHeight);
                    
                    //this is the take off point if we grabbed initial lat lon
                    
                    
#endif
                    if(_coordConverter->_baselat == 0.00 && viewPointData->dVehicleLat != 0.00){
#if defined(__APPLE__)
                        _currentFootPrint.fpLat[0] = viewPointData->dVehicleLat - 0.0167 * GRIDSIZEMILES;//about 1 mile
                        _currentFootPrint.fpLat[1] = viewPointData->dVehicleLat + 0.0167* GRIDSIZEMILES;//about 1 mile
                        _currentFootPrint.fpLat[2] = viewPointData->dVehicleLat + 0.0167* GRIDSIZEMILES;//about 1 mile
                        _currentFootPrint.fpLat[3] = viewPointData->dVehicleLat - 0.0167* GRIDSIZEMILES;//about 1 mile
                        
#endif
                        _coordConverter->_baselat= viewPointData->dVehicleLat;
                        
                    }
                    if(_coordConverter->_baselon == 0.00 && viewPointData->dVehicleLon != 0.00){
#if defined(__APPLE__)
                        int multiplier = 3;//we want to load an elevation grid greater than the footprint size so no tearing
                        _currentFootPrint.fpLon[0] = viewPointData->dVehicleLon - 0.0167* GRIDSIZEMILES;
                        _currentFootPrint.fpLon[1] = viewPointData->dVehicleLon - 0.0167* GRIDSIZEMILES;
                        _currentFootPrint.fpLon[2] = viewPointData->dVehicleLon + 0.0167* GRIDSIZEMILES;
                        _currentFootPrint.fpLon[3] = viewPointData->dVehicleLon + 0.0167* GRIDSIZEMILES;
                        _EML->LoadArea(viewPointData->dVehicleLat - 0.0167 * GRIDSIZEMILES * multiplier, viewPointData->dVehicleLon - 0.0167* GRIDSIZEMILES * multiplier, viewPointData->dVehicleLat + 0.0167 * GRIDSIZEMILES * multiplier, viewPointData->dVehicleLon + 0.0167* GRIDSIZEMILES * multiplier);
#endif
                        _coordConverter->_baselon = viewPointData->dVehicleLon;
                    }
                    double spLat = 0.00;
                    double spLon = 0.00;
                    double spElev = 0.00;
                    double spRange = 0.00;
                    _footPrintCalculator->GetTerrainIntercept(viewPointData->dVehicleLat, viewPointData->dVehicleLon,
                                                              viewPointData->dVehicleAltitude, viewPointData->dCameraPitch,
                                                              viewPointData->dCameraHeading, viewPointData->dCameraRoll, &spLat, &spLon, &spElev, &spRange);
#if !defined(__APPLE__)
                    _coordConverter->GeodeticToCartesian(viewPointData->dVehicleLat, viewPointData->dVehicleLon, viewPointData->dVehicleAltitude,  RAextCamx, RAextCamy, RAextCamz);
#endif
                    _viewData->dTargetLat = spLat;
                    _viewData->dTargetLon = spLon;
                    _viewData->dTargetAlt = spElev;
                    _viewData->dSlantDistance = spRange;
                    //degree
                    double halfvfovrad = degreeToRadians(viewPointData->dFovVerticalAngle / 2.0);
                    double vdist = spRange * tan(halfvfovrad);
                    double totalDist = vdist * 2.0;
                    _pixelSize = totalDist/_windowHeight;
                }
            }
            
            LFClientEngine::~LFClientEngine()
            {
                if(_statusLogger)
                    _statusLogger->Shutdown();
            }
            
            LFClientEngine::LFClientEngine():
            _labelDensity(2),
            _billboardStyle(true),
            _initialized(false),
            _recordingFlightData(false),
            _currentRasterZoomLevel(0),
            _currentVecTileZoomLevel(0),
            _playVideo(false),
            //_baseLat(0.00),
            //_baseLon(0.00),
            _settingsFileFlight(false),
            _startingLat(0.0),
            _startingLon(0.0),
            _startingAlt(0.0),
            _manualDeltaLat(0.0),
            _manualDeltaLon(0.0),
            _symbolTexturesLoaded(false),
            _windowWidth(0),
            _windowHeight(0),
            _placemarkCollection(new placemarkCollection(false)),
            _maxTilesToRender(14),
            _minRasterZoomLevel(0),
            _alphaStreetVector(1.0),
            _alphaStreetLabel(1.0),
            _alphaPOI(1.0),
            _poiIconMap(new std::map<std::string, boost::shared_ptr<iconInfo> >),
            _placeIconMap(new std::map<std::string, boost::shared_ptr<iconInfo> > ),
            _customSymbolList(new std::vector<boost::shared_ptr<symbol> > ),
            _newKmlFile(false),
            _indexUniformBufferBindList(0),
            _maxBindPoints(0),
            _ublockindex(0)
#if defined(__APPLE__)
            , _overlaysOn(true),
            _streetLinesOn(true),
            _streetLabelsOn(true),
            _streetShieldsOn(true),
            _regenKml(true)//check _listofkmlfilestoload and add those files, if there are any
            
#endif
            
            {
                _coordConverter.reset(new CoordinateConverter);
                
                //_listOfKmlFilesToLoad.push_back("oldkml");
                //todo: let the tile classes keep track of this
                _rasterTileCacheLimits[0] = 1;
                _rasterTileCacheLimits[1] = 1;
                _rasterTileCacheLimits[2] = 1;
                _rasterTileCacheLimits[3] = 1;
                _rasterTileCacheLimits[4] = 1;
                _rasterTileCacheLimits[5] = 1;
                _rasterTileCacheLimits[6] = 1;
                _rasterTileCacheLimits[7] = 1;
                _rasterTileCacheLimits[8] = 1;
                _rasterTileCacheLimits[9] = 1;
                _rasterTileCacheLimits[10] = 1;
                _rasterTileCacheLimits[11] = 20;
                _rasterTileCacheLimits[12] = 20;
                _rasterTileCacheLimits[13] = 20;
                _rasterTileCacheLimits[14] = 20;
                _rasterTileCacheLimits[15] = 20;
#if defined(__APPLE__)
                _rasterTileCacheLimits[16] = 1000;
#else
                _rasterTileCacheLimits[16] = 50;
#endif
                _rasterTileCacheLimits[17] = 50;
                _rasterTileCacheLimits[18] = 50;
                _rasterTileCacheLimits[19] = 50;
                _rasterTileCacheLimits[20] = 50;
                
                _vectorTileCacheLimits[0] = 1;
                _vectorTileCacheLimits[1] = 1;
                _vectorTileCacheLimits[2] = 1;
                _vectorTileCacheLimits[3] = 1;
                _vectorTileCacheLimits[4] = 1;
                _vectorTileCacheLimits[5] = 1;
                _vectorTileCacheLimits[6] = 1;
                _vectorTileCacheLimits[7] = 1;
                _vectorTileCacheLimits[8] = 1;
                _vectorTileCacheLimits[9] = 1;
                _vectorTileCacheLimits[10] = 1;
                _vectorTileCacheLimits[11] = 20;
                _vectorTileCacheLimits[12] = 20;
                _vectorTileCacheLimits[13] = 20;
#if defined(__APPLE__)
                _vectorTileCacheLimits[14] = 200;
                _symbolTextures.reset(new std::map<std::string, GLuint >);
#else
                _vectorTileCacheLimits[14] = 20;
                _symbolTextures.reset(new std::map<std::string, boost::shared_ptr<QOpenGLTexture> >);
#endif
                
                //todo: symboltextures can be stored in
                
                for(int i = 0 ; i < NUMOFZOOMLEVELSRASTER; i++)
                {
                    boost::shared_ptr<vector<boost::shared_ptr<tile> > > tileList(new vector<boost::shared_ptr<tile> >);
                    _rasterTileCache[i] = tileList;
                    boost::shared_ptr<vector<boost::shared_ptr<tile> > > tileListRender(new vector<boost::shared_ptr<tile> >);
                    _rasterTileRendering[i] = tileListRender;
                }
                
                for(int i = 0 ; i < NUMOFZOOMLEVELSVECTILE; i++)
                {
                    boost::shared_ptr<vector<boost::shared_ptr<tile> > > tileList(new vector<boost::shared_ptr<tile> >);
                    _vectorTileCache[i] = tileList;
                    boost::shared_ptr<vector<boost::shared_ptr<tile> > > tileListRender(new vector<boost::shared_ptr<tile> >);
                    _vectorTileRendering[i] = tileListRender;
                }
                //;//set icon hrefs from names that are used in vector tiles
                FillIconMap();
                if(_regenKml)
                {
                    AddKMLFileRegen();//re-add the kml entities from the previous instance of lfclient in case user clicked map and destroyed/recreated lfclient
                    _regenKml = false;
                }
            }
            
