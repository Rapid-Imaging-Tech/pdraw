//  This software is Copyrighted (c) by Michael F. Abernathy, 1996-2015
//  All rights reserved.  No part of this software may be used without the
//  written consent of the author.
#pragma once
//#ifndef ROIMantype
#include "RotationToolKit.h"
//#include "SViewData.h"
#include "boost/shared_ptr.hpp"
#include "ElevationManager.h"
//#include <Windows.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "SViewData.h"
#include "StatusLogger.h"
#include "FootPrint.h"
#include "LoggableObject.h"
#include "boost/thread/mutex.hpp"
#include <roilistener.h>
#include <vector>
#include <coordinateconverter.h>
#include <QOpenGLShaderProgram>
class LoggableObject;

class ROIMan : public LoggableConfigurableObject

{
private:
    void NotifyNewRoi();
    int GetBestLODforROISize();
    double _elevLat1;
    double _elevLat2;
    double _elevLon1;
    double _elevLon2;

    //int CheckElevationBounds(double lat, double lon);
    boost::shared_ptr<DemTileCollection> _demTileCollection;
    //bool GetElevationAtLocation(double lat, double lon, double &elevation);
    double _kmlGridNorthLatLine ;
    double _kmlGridSouthLatLine ;
    double _kmlGridEastLonLine ;
    double _kmlGridWestLonLine ;
    bool CheckRoiChangeWithFootprint(boost::shared_ptr<SViewData> viewData, bool& forceNewROI);
    bool CheckRoiListenersForPendingSwap();
    bool _resync;// = false;//when true, we need to check all listeners, make sure all
    //coordconverter id's are =, when that is true signal the swap, and set resync to false;
    //set roichange lockout to true.  when alpha = 0 do the swap, don't allow roichanges during the
    //fadeout/in,  when complete set roichangelockout = false;
    bool _roiChangeLockout;// = false;
    int CheckRoiListenerSync();
    void ResetRoiListeners();
    std::vector<CoordinateConverter> _coordinateConverters;
    void NotifyPendingRoiChange();
    void NotifyRoiChangeSwap();

    std::vector<RoiListener*> _roiListeners;
	boost::mutex _footprintMutex;
protected:
	virtual void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData) ;
public:
    void SetLatestKmlGridBorder(double northLine, double southLine, double eastLine,
                                double westLine);
    void AddRoiListener(RoiListener* listener);
    boost::mutex _streetLabelMutex;
	FootPrint GetFootPrint();
    int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);
    //void SetupElevMgrURL(std::string url);
    //void SetStatusLoggerReference(boost::shared_ptr<StatusLogger> logger){this->_statusLogger = logger;};
    //double GetElevationAtGeodetic(double lat, double lon){return 4600.00;};//todo: fix this
	
    //void SetDLBase(GLuint displayListBase){_dlBase = displayListBase;};
    //void ObjectiveROI(double lat, double lon, double size);//Original
    //void ObjectiveROI(double lat, double lon, double size, double ratio, double head);//Improved
    //bool IsNewROI(double lat, double lon, double Heading, double alpha, bool& roiLockout);
	void GetNewROI(double& x1,double& x2,double& y1,double& y2);

    //this function is the master converter to follow, we basically use the stale
    //coord conversion until all models are ready and up to date with newwer
    //coord conversions.  when all models locked into place, we do a roilock
    //meaning no roichanges can occur until the alpha fade out, and model matrices swap occurs
    //then we can unlock, and use the latest coordinates
    void GeodeticToCartesian(boost::shared_ptr<SViewData> viewData,
                             double& x, double& y, double& z);

    void GeodeticToCartesian(double lat, double lon, double altitude,
                             double& x, double& y, double& z);

    void CartesianToGeodetic(double& latitude,double& longitude,double& altitude,
                             double x, double y, double z);
    void SetViewPoint(double x, double y, double z,
                      double heading, double pitch, double roll);
    void SetFrustum(double fov, double hither, double yon, double ScreenAspect);
	//double GetGroundElevation(double lat, double lon);
	bool GetGroundElevation(double lat, double lon, double &elevation);

	bool GetStarePoint(SViewData * sview, double * sLat, double * sLon, double * sAlt, double * range);
	ROIMan();
    ROIMan(double lat1, double lon1, double lat2, double lon2);
	~ROIMan();
	// Sets the viewpoint, manages the ROI, creates entities, calls render and drawscene
	//int ViewPointUpdate(void);
	// clears, draws the scene and swaps
    //int Render(void);
	// Calls the lists for viewpoint and entities
    //int DrawScene(void);
	// Check for LOS (Line of Sight) composite angles, and if they are not set, compute them from platform relative angles.
	int CheckSetLOS(boost::shared_ptr<SViewData> svd);
	// This function places performs the OpenGL translate and rotate necessary to place an object in the scene at the give location and p,h,r angles
//	int PlaceObjectInTheScene(const sixdof& sdf);
    //void SetAutoTrack(bool val){_autoTrack = val;};
//	void SetVP(boost::shared_ptr<SViewData> val);
	// computes terrain intercept
	int GetTerrainIntercept(double lat, double lon, double alt, double p, double h, double r, double *lt, double *ln, double *al, double *range);
	// compute new point from location + bearing + direction
	int GetNewLocationFromBearingAndDistance(double lat, double lon, double bearing, double distance, double *nlat, double *nlon);
	// get roi limits
    //void GetLimits(double *limits);
    int ComputePrecisionFootprint(double lat, double lon, double alt, double pitch, double heading, double roll,
        double camPitch, double camHeading, double camRoll, double hFOV, double FOV);
	FootPrint _FP;
	/// MFA added members needed to track changes that effect ROI
	/// compute the field of view projected to the starepoint size in meters
	double FieldSizeMeters(boost::shared_ptr<SViewData> svd);
	/// see if the ROI Size has changed, and if so return new size.
	double ROISizeChanged(boost::shared_ptr<SViewData> svd);
	/// compute optimal roisize
	double BestROISize(boost::shared_ptr<SViewData> svd);
	/// computer how far apart (in meters) are two different starepoints
	double SPDriftDistanceMeters(boost::shared_ptr<SViewData> svd1, boost::shared_ptr<SViewData> svd2);
	/// this member replaces objective ROI
    bool IsNewROI(boost::shared_ptr<SViewData> svd, double alpha, bool& impendingRoiChange, bool& forceNewROI);
    bool _novus;
	/// this member will compute the distance between two points of graticule into meters
	double ComputeDistanceMeters(double LT, double LN, double lt, double ln);
	/// check whether we are still within our elevtation tile.
    //int CheckElevationBounds(double lat, double lon);

	virtual double GetGeo4Pixel(boost::shared_ptr<SViewData> viewPointData, double xpxl, double ypxl, double * latitude, double * longitude, double * altitude);
	// compute the x and y pixel location for a given latitutde and longitude
	//return the current roisize in degrees
	virtual double GetROISize();
private:
    void RefreshCoordConverters();
    int AddNewCoordConverter(double lat1, double lon1, double lat2, double lon2);
    CoordinateConverter GetLatestCoordConverter();
    CoordinateConverter GetOldestCoordConverter();
	//void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
	boost::shared_ptr<UserSettingsData> _usettingsdata;

	//void WriteToStatusLog(std::string message);

	boost::shared_ptr<StatusLogger> _statusLogger;
	GLuint _dlBase;
	bool _autoTrack;
	bool _valid;
	double _ox1;
    //double _ox2;
	double _oy1;
    //double _oy2;
	double _leadROI;
	double _anticipateROI;
	double _ROISize;
	double _standardROISize;
	double _sizeROIMeters;
    double _degreeNorthInMeters;
	double _hither;
	double _yon;
	double _fieldOfView;
	double _pixelXYaspectRatio;
	double _screenAspectRatio;
    double _camX;
	double _camY;
	double _camZ;
    double _originLat;
	double _originLon;
    GLuint _dlVPXLate;
	GLuint _dlVPXForm;
	RotationToolKit * _rtk;
    boost::shared_ptr<SViewData> _ROI_SVD;

};

