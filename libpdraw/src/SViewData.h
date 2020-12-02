#pragma once
#import <memory>

#if !defined SMASK
#define SMASK
using namespace std;
struct SMaskColor
{
    unsigned char cRed;
    unsigned char cBlue;
    unsigned char cGreen;
};

struct SImageData
{
    enum {RGBA=0, RGB = 1, BGR = 2, BGRA = 3};
    int iType;
    int iWidth;
    int iHeight;
};

class SViewData
{
public:
    SViewData()
    {
        cAvailableData = 'd';
        dTime = -0.00;
        dDisplayAspectRatio = 0.00;
        dVehicleLat = -0.00;
        dVehicleLon = -0.00;
        dVehicleAltitude = -0.00;
        dVehicleHeading = -0.00;
        dfov = -0.00;
        dCameraPitch = -0.00;
        dCameraHeading = -0.00;
        dCameraRoll = -0.00;
        dVehiclePitch = -0.00;
        dVehicleRoll = -0.00;
        dFovVerticalAngle = -0.00;
        dFovHorizontalAngle = -0.00;
        dSlantDistance = -0.00;
        dTargetLat = -0.00;
        dTargetLon = -0.00;
        dTargetAlt = -0.00;
        LOSPitch=LOSHeading=LOSRoll=-0.0;
    }

    SViewData(std::shared_ptr<SViewData> ref){
        //dataVersion = ref.dataVersion;
        cAvailableData = ref->cAvailableData;
        dDisplayAspectRatio = ref->dDisplayAspectRatio;
        dVehicleLat = ref->dVehicleLat;
        dVehicleLon = ref->dVehicleLon;
        dVehicleAltitude = ref->dVehicleAltitude;
        dVehicleHeading = ref->dVehicleHeading;
        dfov = ref->dfov;
        dCameraPitch = ref->dCameraPitch;
        dCameraHeading = ref->dCameraHeading;
        dCameraRoll = ref->dCameraRoll;
        dVehiclePitch = ref->dVehiclePitch;
        dVehicleRoll = ref->dVehicleRoll;
        dFovVerticalAngle = ref->dFovVerticalAngle;
        dFovHorizontalAngle = ref->dFovHorizontalAngle;
        dSlantDistance = ref->dSlantDistance;
        dTargetLat = ref->dTargetLat;
        dTargetLon = ref->dTargetLon;
        dTargetAlt = ref->dTargetAlt;
        LOSPitch = ref->LOSPitch;
        LOSHeading = ref->LOSHeading;
        LOSRoll = ref->LOSRoll;
    }

    SViewData(const SViewData& ref)
    {
        //dataVersion = ref.dataVersion;
        cAvailableData = ref.cAvailableData;
        dDisplayAspectRatio = ref.dDisplayAspectRatio;
        dVehicleLat = ref.dVehicleLat;
        dVehicleLon = ref.dVehicleLon;
        dVehicleAltitude = ref.dVehicleAltitude;
        dVehicleHeading = ref.dVehicleHeading;
        dfov = ref.dfov;
        dCameraPitch = ref.dCameraPitch;
        dCameraHeading = ref.dCameraHeading;
        dCameraRoll = ref.dCameraRoll;
        dVehiclePitch = ref.dVehiclePitch;
        dVehicleRoll = ref.dVehicleRoll;
        dFovVerticalAngle = ref.dFovVerticalAngle;
        dFovHorizontalAngle = ref.dFovHorizontalAngle;
        dSlantDistance = ref.dSlantDistance;
        dTargetLat = ref.dTargetLat;
        dTargetLon = ref.dTargetLon;
        dTargetAlt = ref.dTargetAlt;
        LOSPitch = ref.LOSPitch;
        LOSHeading = ref.LOSHeading;
        LOSRoll = ref.LOSRoll;

    }

    enum AvailableData
    {
        HAS_VEHICLELOC  = 0x1,
        HAS_SLANT       = 0X2,
        HAS_TARGETLOC   = 0X4,
        HAS_FOV         = 0X8,
        HAS_DEPANGLE    = 0X16,
        HAS_AZMANGLE    = 0X32,
        HAS_YPR         =0X64
    };
    char cAvailableData;
    //int dataVersion;//check that this is different everytime
    double dTime; // Time in seconds after 1970 - Unix time but expressed in decimal seconds
    double dDisplayAspectRatio;    // physical display size aspect(eg 11/9 or 4/3)

    double dVehicleLat;
    double dVehicleLon;            // lat & lon camera position in degrees WGS84
    double dVehicleAltitude;    // altitude in meters above mean sea level
    double dVehicldAltitudeAGL;// height above ground without ground elevation added in
    //new////////////////////
    double dfov;                // DEPRECATED - formerly held vertical FOV - dont use
    double dCameraPitch;        // camera pitch relative to the aircraft. (degrees)
    double dCameraHeading;        // camera azimuth relative to the aircraft. (degrees)
    double dCameraRoll;            // camera roll relative to the aircraft. (degrees)
    //////////////////////////
    double dVehicleHeading;        // Vehicle (platform) HEADING degrees
    double dVehiclePitch;        // Vehicle pitch degrees
    double dVehicleRoll;        // Vehicle roll degrees

    double dFovVerticalAngle;    // Vertical camera field of view in degrees
    double dFovHorizontalAngle;    // Horizontal camera field of view in degrees

    double dSlantDistance;        // Range to target in meters

    double dTargetLat;            // stare point latitude and longitude in degrees
    double dTargetLon;
    double dTargetAlt;            // stare point altitude in degrees

    double LOSPitch, LOSHeading, LOSRoll;  // line of sight (composite) look-angles:
    double dts;
    // heading relative true North
    // pitch relative horizon positive up
    // roll relative horizont postive right

};
#endif


//#pragma once
//#include "boost/shared_ptr.hpp"

//#if !defined SMASK
//#define SMASK
///**
// * struct to contain color data
// */
//struct SMaskColor
//{
//    unsigned char cRed;
//    unsigned char cBlue;
//    unsigned char cGreen;
//};

///**
// * struct to contain data about an image
// */
//struct SImageData
//{
//    enum {RGBA=0, RGB = 1, BGR = 2, BGRA = 3};
//    int iType;
//    int iWidth;
//    int iHeight;
//};

///**
// * a class that contains 6dof such as latitude longitude altitude pitch heading roll of the vehicle/camera
// */
//class SViewData
//{
//public:
//    SViewData()
//    {
//        cAvailableData = 'd';
//        dTime = 0.00;
//        dDisplayAspectRatio = 0.00;
//        dVehicleLat = 0.00;
//        dVehicleLon = 0.00;
//        dVehicleAltitude = 0.00;
//        dVehicleHeading = 0.00;
//        dfov = 0.00;
//        dCameraPitch = 0.00;
//        dCameraHeading = 0.00;
//        dCameraRoll = 0.00;
//        dVehiclePitch = 0.00;
//        dVehicleRoll = 0.00;
//        dFovVerticalAngle = 0.00;
//        dFovHorizontalAngle = 0.00;
//        dSlantDistance = 0.00;
//        dTargetLat = 0.00;
//        dTargetLon = 0.00;
//        dTargetAlt = 0.00;
//        LOSPitch=0.0;
//        LOSHeading=0.0;
//        LOSRoll=0.0;
//	}
  
//	SViewData(boost::shared_ptr<SViewData> ref){
//		       //dataVersion = ref.dataVersion;
//        cAvailableData = ref->cAvailableData;
//        dDisplayAspectRatio = ref->dDisplayAspectRatio;
//        dVehicleLat = ref->dVehicleLat;
//        dVehicleLon = ref->dVehicleLon;
//        dVehicleAltitude = ref->dVehicleAltitude;
//        dVehicleHeading = ref->dVehicleHeading;
//        dfov = ref->dfov;
//        dCameraPitch = ref->dCameraPitch;
//        dCameraHeading = ref->dCameraHeading;
//        dCameraRoll = ref->dCameraRoll;
//        dVehiclePitch = ref->dVehiclePitch;
//        dVehicleRoll = ref->dVehicleRoll;
//        dFovVerticalAngle = ref->dFovVerticalAngle;
//        dFovHorizontalAngle = ref->dFovHorizontalAngle;
//        dSlantDistance = ref->dSlantDistance;
//        dTargetLat = ref->dTargetLat;
//        dTargetLon = ref->dTargetLon;
//        dTargetAlt = ref->dTargetAlt;
//		LOSPitch = ref->LOSPitch;
//		LOSHeading = ref->LOSHeading;
//		LOSRoll = ref->LOSRoll;
//	}
//	~SViewData()
//	{
//		dVehicleHeading = 0.00;
//	}
//    SViewData(const SViewData& ref)
//    {
//        //dataVersion = ref.dataVersion;
//        cAvailableData = ref.cAvailableData;
//        dDisplayAspectRatio = ref.dDisplayAspectRatio;
//        dVehicleLat = ref.dVehicleLat;
//        dVehicleLon = ref.dVehicleLon;
//        dVehicleAltitude = ref.dVehicleAltitude;
//        dVehicleHeading = ref.dVehicleHeading;
//        dfov = ref.dfov;
//        dCameraPitch = ref.dCameraPitch;
//        dCameraHeading = ref.dCameraHeading;
//        dCameraRoll = ref.dCameraRoll;
//        dVehiclePitch = ref.dVehiclePitch;
//        dVehicleRoll = ref.dVehicleRoll;
//        dFovVerticalAngle = ref.dFovVerticalAngle;
//        dFovHorizontalAngle = ref.dFovHorizontalAngle;
//        dSlantDistance = ref.dSlantDistance;
//        dTargetLat = ref.dTargetLat;
//        dTargetLon = ref.dTargetLon;
//        dTargetAlt = ref.dTargetAlt;
//		LOSPitch = ref.LOSPitch;
//		LOSHeading = ref.LOSHeading;
//		LOSRoll = ref.LOSRoll;

//    }
    
//    enum AvailableData
//    {
//        HAS_VEHICLELOC  = 0x1,
//        HAS_SLANT       = 0X2,
//        HAS_TARGETLOC   = 0X4,
//        HAS_FOV         = 0X8,
//        HAS_DEPANGLE    = 0X16,
//        HAS_AZMANGLE    = 0X32,
//        HAS_YPR         =0X64
//    };
//    char cAvailableData;
//    //int dataVersion;//check that this is different everytime
//        long long dTime; ///< Time in seconds after 1970 - Unix time but expressed in decimal seconds
//    double dDisplayAspectRatio;	///< physical display size aspect(eg 11/9 or 4/3)

//    double dVehicleLat;
//    double dVehicleLon;			///< lat & lon camera position in degrees WGS84
//    double dVehicleAltitude;	///< altitude in meters above mean sea level
//    //new////////////////////
//    double dfov;				///< DEPRECATED - formerly held vertical FOV - dont use
//    double dCameraPitch;		///< camera pitch relative to the aircraft. (degrees)
//    double dCameraHeading;		///< camera azimuth relative to the aircraft. (degrees)
//    double dCameraRoll;			///< camera roll relative to the aircraft. (degrees)
//    //////////////////////////
//    double dVehicleHeading;		///< Vehicle (platform) HEADING degrees
//    double dVehiclePitch;		///< Vehicle pitch degrees
//    double dVehicleRoll;		///< Vehicle roll degrees

//    double dFovVerticalAngle;	///< Vertical camera field of view in degrees
//    double dFovHorizontalAngle;	///< Horizontal camera field of view in degrees

//    double dSlantDistance;		///< Range to target in meters

//    double dTargetLat;			///< stare point latitude and longitude in degrees
//    double dTargetLon;
//    double dTargetAlt;			///< stare point altitude in degrees

//        double LOSPitch, LOSHeading, LOSRoll;  ///< line of sight (composite) look-angles:
//								// heading relative true North
//								// pitch relative horizon positive up
//								// roll relative horizont postive right

//};
//#endif
