// LFClient Copyright (c) 2015 Michael Aberanthy
// This implements a static library containing the LandForm Client Engine in a small footprint,
// with minimal dependencies.

// This particular class is a wrapper the contains only public members.
// The rest of the functionality is private and located in other members called from the CPP.
#pragma once
//#include <Windows.h>

#include "SViewData.h"
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include "UserSettingsData.h"
#include <vector>
#if defined(__APPLE__)
#else

#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLShaderProgram>
#endif
#include "kmlpathdetails.h"
#include "placemarkinfo.h"
//#include <WinDef.h>

/** currently unused
 */
enum LAYERENUM{
    LandForm_Server,
    UrbanAreas,
    Hydrolines,
    HydroPolygons,
    NARoadsHiRes,
    Roadlines,
    Roadlabels,
    RailRoads,
    POI,
    Airports,
    USAirportPolygons,
    Runways,
    Boundaries,
    RailYards
};

/** this is a list of all available points of interest that can be shown/hidden by use of the SetPOISettings function
 */
enum POIENUM{
    SHIELD_WHITERECT,
    SHIELD_HIGHWAY,
    SHIELD_INTERSTATE,
    
    SL_STREETLABELX1,
    SL_STREETLABELX2,
    SL_STREETLABELX4,
    SL_STREETLABELX8,
    SL_STREETLABELX16,
    PM_CUSTOM,
    PLACE_CITY,
    PLACE_TOWN,
    PLACE_VILLAGE,
    PLACE_HAMLET,
    PLACE_SUBURB,
    PLACE_NEIGHBOURHOOD,
    PLACE_ISLAND,
    PLACE_ISLET,
    PLACE_ARCHIPELAGO,
    PLACE_RESIDENTIAL,
    PLACE_ABORIGINAL_LANDS,
    PM_UNKNOWN,
    PM_AIRFIELD,
    PM_ALCOHOL_SHOP,
    PM_AMUSEMENT_PARK,
    PM_AQUARIUM,
    PM_ART_GALLERY,
    PM_ATTRACTION,
    PM_BAKERY,
    PM_BANK,
    PM_BAR,
    PM_BEER,
    PM_BICYCLE,
    PM_BICYCLE_SHARE,
    PM_BUS,
    PM_CAFE,
    PM_CAR,
    PM_CAMPSITE,
    PM_CASTLE,
    PM_CEMETERY,
    PM_CINEMA,
    PM_CLOTHING_STORE,
    PM_COLLEGE,
    PM_DENTIST,
    PM_DOG_PARK,
    PM_DRINKING_WATER,
    PM_EMBASSY,
    PM_ENTRANCE,
    PM_FAST_FOOD,
    PM_FERRY,
    PM_FUEL,
    PM_GARDEN,
    PM_GROCERY,
    PM_HARBOR,
    PM_HELIPORT,
    PM_ICE_CREAM,
    PM_INFORMATION,
    PM_LAUNDRY,
    PM_LODGING,
    PM_MONUMENT,
    PM_MUSEUM,
    PM_MUSIC,
    PM_PARK,
    PM_PHARMACY,
    PM_PICNIC_SITE,
    PM_PLACE_OF_WORSHIP,
    PM_PLAYGROUND,
    PM_POLICE,
    PM_POST,
    PM_PRISON,
    PM_RELIGIOUS_CHRISTIAN,
    PM_RELIGIOUS_JEWISH,
    PM_RELIGIOUS_MUSLIM,
    PM_RESTAURANT,
    PM_ROCKET,
    PM_SCHOOL,
    PM_SHOP,
    PM_STADIUM,
    PM_SWIMMING,
    PM_SUITCASE,
    PM_THEATRE,
    PM_TOILET,
    PM_TOWN_HALL,
    PM_VETERINARY,
    PM_ZOO,
    PM_DOCTOR,
    PM_HOSPITAL,
    PM_LIBRARY,
    PM_GOLF,
    PM_FIRE_STATION
};


/// A class that holds the state of points of interest hide/show characteristics
class placemarkCollection
{
public:

    ///setValue initially hides/shows all elements the same value
    placemarkCollection(bool setValue);
    ///reset all elements to hide/show
    void reset(bool setValue);//set all flags to "val"
    ///set individual point of interest/placemark to hide/show
    void setValue(POIENUM placemark, bool val);
    ///get the value of whether said poi is hidden or visible
    bool isVisible(POIENUM placemark);

    ///stores the actual settings
    std::map<POIENUM, bool> _placemarkVisibleSettings;
};

/** currently unused
 */
enum SERVERSTATE{
    LOADING,
    SERVICEUNAVAILABLE,
    READY
};
class LFClient
{
public:

    virtual std::vector<KmlPathDetails> GetKMLEntitiesFromAlreadyLoadedFile(std::string kmlFileName) = 0;
    virtual std::vector<std::string> GetLoadedKMZFilenames() = 0;
    /** This function turns specific point of interest icons on/off.  Create a shared_ptr to a placemarkcollection and turn on/off
     * each item as desired then call this function
     */
    virtual void SetPOISettings(boost::shared_ptr<placemarkCollection> pmCollection)=0;///< Set point of interest icons hide/show


    /** if client wishes to insert elements from a kmz file.  Works with custom icons, not google specific icons
     * currently kml files have not been tested
     */
    virtual std::vector<PlacemarkInfo > AddKMLFile(std::string kmlfile, bool regen = false) = 0;

    virtual void AddKMLFileRegen() = 0;
    /** after user adds elements from a kmz file, a placemarkinfo object is sent back with information for
     * all the items detected and added into the scene.  Use this object to gather data to hide said element
     */
#if defined(__APPLE__)
#else
    virtual void HideKMLEntities(QString filename, QStringList path, QString element) = 0;
#endif
    /** after user adds elements from a kmz file, a placemarkinfo object is sent back with information for
     * all the items detected and added into the scene.  Use this object to gather data to show said element
     */
#if defined(__APPLE__)
#else
    virtual void ShowKMLEntities(QString filename, QStringList path, QString element) = 0;

    /** after user adds elements from a kmz file, a placemarkinfo object is sent back with information for
     * all the items detected and added into the scene.  Use this object to gather data to delete said element
     */
    virtual void DeleteKMLEntities(QString filename, QStringList path, QString element) = 0;
#endif


    /** sets the transparency of video overlays AR.  Valid values are 0.0 - 1.0
     */
    //virtual void SetAlpha(double alpha) = 0;
    virtual void SetAlphaStreetVector(double alpha) = 0;
    virtual void SetAlphaStreetLabel(double alpha) = 0;
    virtual void SetAlphaPOI(double alpha) = 0;
    
    /** once an opengl shader is created and valid viewdata is observed, use them as arguments to the init function
     * this should be called before any other routine
     */
#if defined(__APPLE__)
#else
    virtual int Init(boost::shared_ptr<SViewData> viewData, QOpenGLShaderProgram* symShader,
                     QOpenGLShaderProgram* lineworkShader) = 0;
#endif
    /** currently unused
     */
    virtual void UpdateROI() = 0;

    /** currently unused
     */
    virtual int enableStreetVectors(bool enable) = 0;

    /** currently unused
     */
    virtual int enableStreetLabels(bool enable) = 0;

    /** currently unused
     */
    virtual int enablePlacemarks(bool enable) = 0;

    /** currently unused
     */
    virtual void SetLODOverride(int lod) = 0;

    /** call this function each iteration of the main render loop of video ie this should be called approximately the frame rate.
     * see example in smartcamlayer in how to set up.  Some of these items will eventually be removed and simplified
     */
#if defined(__APPLE__)
#else
    //    /** call this function each iteration of the main render loop of video ie this should be called approximately the frame rate.
    //     * see example in smartcamlayer in how to set up.  Some of these items will eventually be removed and simplified
    //     */
    virtual int Render(QOpenGLShaderProgram *program, QMatrix4x4 projection, double lowerLeftX, double lowerLeftY, double windowWidth, double windowHeight) = 0;

#endif
    /** currently unused
     */
    virtual int SetServiceLayers(std::vector <LAYERENUM>) = 0;

    /** currently unused
     */
    virtual void SetStreetLineWidth(double lineWidth) = 0;

    /** currently unused
     */
    virtual void SetStreetLineColor(double red, double green, double blue) = 0;

    /** currently unused
     */
    virtual void SetStreetLabelScale(double scale) = 0;

    /** currently unused, but is planned to be implemented in the future
     */
    virtual void GetStarePoint(double& latitude, double& longitude, double& elevation, double& range) = 0;

    /** call this function whenever new viewdata/sixdof is encountered.  It updates the scene view
     */
    virtual void SetVP(boost::shared_ptr<SViewData> viewPointData, bool fromKLV = false) = 0;

private:
    // window size and location
    // video size and packing
    int _videoNX;
    int _videoNY;
    int _videoType;
    //double _screenAspectRatio;
};

//// LFClient Copyright (c) 2015 Michael Aberanthy
//// This implements a static library containing the LandForm Client Engine in a small footprint,
//// with minimal dependencies.

//// This particular class is a wrapper the contains only public members.
//// The rest of the functionality is private and located in other members called from the CPP.
//#pragma once
////#include <Windows.h>

//#include "SViewData.h"
//#include "boost/shared_ptr.hpp"
//#include "boost/shared_array.hpp"
//#include "UserSettingsData.h"
//#include <vector>
//#include <QtGui/QOpenGLShaderProgram>
//#include <QOpenGLShaderProgram>
//#include "kmlpathdetails.h"
//#include "placemarkinfo.h"
////#include <WinDef.h>

///** currently unused
// */
//enum LAYERENUM{
//        LandForm_Server,
//        UrbanAreas,
//        Hydrolines,
//        HydroPolygons,
//        NARoadsHiRes,
//        Roadlines,
//        Roadlabels,
//        RailRoads,
//        POI,
//        Airports,
//        USAirportPolygons,
//        Runways,
//        Boundaries,
//        RailYards
//};

///** this is a list of all available points of interest that can be shown/hidden by use of the SetPOISettings function
// */
//enum POIENUM{
//    PM_CUSTOM,
//    PLACE_CITY,
//    PLACE_TOWN,
//    PLACE_VILLAGE,
//    PLACE_HAMLET,
//    PLACE_SUBURB,
//    PLACE_NEIGHBOURHOOD,
//    PLACE_ISLAND,
//    PLACE_ISLET,
//    PLACE_ARCHIPELAGO,
//    PLACE_RESIDENTIAL,
//    PLACE_ABORIGINAL_LANDS,
//    PM_UNKNOWN,
//    PM_AIRFIELD,
//    PM_ALCOHOL_SHOP,
//    PM_AMUSEMENT_PARK,
//    PM_AQUARIUM,
//    PM_ART_GALLERY,
//    PM_ATTRACTION,
//    PM_BAKERY,
//    PM_BANK,
//    PM_BAR,
//    PM_BEER,
//    PM_BICYCLE,
//    PM_BICYCLE_SHARE,
//    PM_BUS,
//    PM_CAFE,
//    PM_CAR,
//    PM_CAMPSITE,
//    PM_CASTLE,
//    PM_CEMETERY,
//    PM_CINEMA,
//    PM_CLOTHING_STORE,
//    PM_COLLEGE,
//    PM_DENTIST,
//    PM_DOG_PARK,
//    PM_DRINKING_WATER,
//    PM_EMBASSY,
//    PM_ENTRANCE,
//    PM_FAST_FOOD,
//    PM_FERRY,
//    PM_FUEL,
//    PM_GARDEN,
//    PM_GROCERY,
//    PM_HARBOR,
//    PM_HELIPORT,
//    PM_ICE_CREAM,
//    PM_INFORMATION,
//    PM_LAUNDRY,
//    PM_LODGING,
//    PM_MONUMENT,
//    PM_MUSEUM,
//    PM_MUSIC,
//    PM_PARK,
//    PM_PHARMACY,
//    PM_PICNIC_SITE,
//    PM_PLACE_OF_WORSHIP,
//    PM_PLAYGROUND,
//    PM_POLICE,
//    PM_POST,
//    PM_PRISON,
//    PM_RELIGIOUS_CHRISTIAN,
//    PM_RELIGIOUS_JEWISH,
//    PM_RELIGIOUS_MUSLIM,
//    PM_RESTAURANT,
//    PM_ROCKET,
//    PM_SCHOOL,
//    PM_SHOP,
//    PM_STADIUM,
//    PM_SWIMMING,
//    PM_SUITCASE,
//    PM_THEATRE,
//    PM_TOILET,
//    PM_TOWN_HALL,
//    PM_VETERINARY,
//    PM_ZOO,
//    PM_DOCTOR,
//    PM_HOSPITAL,
//    PM_LIBRARY,
//    PM_GOLF,
//    PM_FIRE_STATION
//};


///// A class that holds the state of points of interest hide/show characteristics
//class placemarkCollection
//{
//public:

//    ///setValue initially hides/shows all elements the same value
//    placemarkCollection(bool setValue);
//    ///reset all elements to hide/show
//    void reset(bool setValue);//set all flags to "val"
//    ///set individual point of interest/placemark to hide/show
//    void setValue(POIENUM placemark, bool val);
//    ///get the value of whether said poi is hidden or visible
//    bool isVisible(POIENUM placemark);
//private:
//    ///stores the actual settings
//    std::map<POIENUM, bool> _placemarkVisibleSettings;
//};

///** currently unused
// */
//enum SERVERSTATE{
//        LOADING,
//        SERVICEUNAVAILABLE,
//        READY
//};
//class LFClient
//{
//public:

//    /** This function turns specific point of interest icons on/off.  Create a shared_ptr to a placemarkcollection and turn on/off
//     * each item as desired then call this function
//     */
//    virtual void SetPOISettings(boost::shared_ptr<placemarkCollection> pmCollection)=0;///< Set point of interest icons hide/show


//    /** if client wishes to insert elements from a kmz file.  Works with custom icons, not google specific icons
//     * currently kml files have not been tested
//     */
//    virtual std::vector<PlacemarkInfo > AddKMLFile(std::string kmlfile) = 0;

//    /** after user adds elements from a kmz file, a placemarkinfo object is sent back with information for
//     * all the items detected and added into the scene.  Use this object to gather data to hide said element
//     */
//    virtual void HideKMLEntities(QString filename, QStringList path, QString element) = 0;
//    /** after user adds elements from a kmz file, a placemarkinfo object is sent back with information for
//     * all the items detected and added into the scene.  Use this object to gather data to show said element
//     */
//    virtual void ShowKMLEntities(QString filename, QStringList path, QString element) = 0;

//    /** after user adds elements from a kmz file, a placemarkinfo object is sent back with information for
//     * all the items detected and added into the scene.  Use this object to gather data to delete said element
//     */

//virtual void DeleteKMLEntities(QString filename, QStringList path, QString element) = 0;


//    /** sets the transparency of video overlays AR.  Valid values are 0.0 - 1.0
//     */
//    virtual void SetAlpha(double alpha) = 0;

//    /** once an opengl shader is created and valid viewdata is observed, use them as arguments to the init function
//     * this should be called before any other routine
//     */
//    virtual int Init(boost::shared_ptr<SViewData> viewData, QOpenGLShaderProgram* symShader,
//                      QOpenGLShaderProgram* lineworkShader) = 0;
//    /** currently unused
//     */
//    virtual void UpdateROI() = 0;

//    /** currently unused
//     */
//    virtual int enableStreetVectors(bool enable) = 0;

//    /** currently unused
//     */
//    virtual int enableStreetLabels(bool enable) = 0;

//    /** currently unused
//     */
//    virtual int enablePlacemarks(bool enable) = 0;

//    /** currently unused
//     */
//    virtual void SetLODOverride(int lod) = 0;

//    /** call this function each iteration of the main render loop of video ie this should be called approximately the frame rate.
//     * see example in smartcamlayer in how to set up.  Some of these items will eventually be removed and simplified
//     */
//    virtual int Render(QOpenGLShaderProgram *program, QMatrix4x4 projection, double lowerLeftX, double lowerLeftY, double windowWidth, double windowHeight) = 0;

//    /** currently unused
//     */
//    virtual int SetServiceLayers(std::vector <LAYERENUM>) = 0;

//    /** currently unused
//     */
//    virtual void SetStreetLineWidth(double lineWidth) = 0;

//    /** currently unused
//     */
//    virtual void SetStreetLineColor(double red, double green, double blue) = 0;

//    /** currently unused
//     */
//    virtual void SetStreetLabelScale(double scale) = 0;

//    /** currently unused, but is planned to be implemented in the future
//     */
//    virtual void GetStarePoint(double& latitude, double& longitude, double& elevation, double& range) = 0;

//    /** call this function whenever new viewdata/sixdof is encountered.  It updates the scene view
//     */
//    virtual void SetVP(boost::shared_ptr<SViewData> viewPointData, bool fromKLV = false) = 0;

//private:
//        // window size and location
//        // video size and packing
//        int _videoNX;
//        int _videoNY;
//        int _videoType;
//        //double _screenAspectRatio;
//};
