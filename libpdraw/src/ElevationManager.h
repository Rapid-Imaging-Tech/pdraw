//
// ElevationManager.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2015 Eric Cox for Rapid Imaging Software Inc.
//
#pragma once
#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <string>
#include </usr/local/include/boost/asio.hpp>
#include </usr/local/include/boost/bind.hpp>
#include </usr/local/include/boost/thread.hpp>
#include </usr/local/include/boost/filesystem/fstream.hpp>
#include </usr/local/include/boost/filesystem/operations.hpp>
#include </usr/local/include/boost/shared_array.hpp>
#include </usr/local/include/boost/thread/mutex.hpp>

#include "DEMTile.h"
#include "WMSQuery.h"
#include "UserSettingsData.h"
#include "LoggableObject.h"
#include "boost/shared_ptr.hpp"
#include "DemTileCollection.h"
#include <elevationclient.h>
using boost::asio::ip::tcp;



class ElevationManager: public LoggableConfigurableObject
{
	//Elevation manager handles requests for elevation and manages a collection of DEMTiles
	//It also does async handling for the WMS requests
public:
    boost::shared_ptr<DemTileCollection> GetDemTile(){return _demTileCollection;}
    bool DemTileComplete();//{return _demTileCollectionComplete;}
    int SetLocationAndSize(double lowerLat, double  lowerLon, double upperLat, double upperLon, boost::shared_ptr<DemTileCollection> demTileCollection);
    int SetLocationAndSize(double lat, double  lon, double SizeInKM, boost::shared_ptr<DemTileCollection> demTileCollection,
                           double& lat1, double& lon1, double& lat2, double& lon2);

    int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);
	void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
	void SetURLPath(std::string urlPath);
	void SetTempPath(std::string tempPath);
    void SetURL(WebMapURLGenerator * query);
    ElevationManager();
	bool GetElevationAtLocation(double lat, double lon, double &elevation);

	bool RequestTile(double lat1, double lon1, double lat2, double lon2,int size);

	int ComputeIntercept(SViewData * sview, double * lat, double * lon, double * alt, double * range);
	double ComputeTheSceneSizeAtRange(double range, double HorizontalFOV);
	int QualifyLimits(double lat, double lon, double size);
	int CheckElevationBounds(double lat, double lon);
    bool GetRequestInProgress(){return _elevClient->_requestInProgress;}
private:
    bool _demTileCollectionComplete;
    boost::shared_ptr<ElevationClient> _elevClient;
    bool _requestInProgress;
    boost::shared_ptr<UserSettingsData> _usettingsdata;
    std::string _tempPath;

	//These are related to the DEM tiles
    boost::shared_ptr<DemTileCollection> _demTileCollection;
    //boost::shared_ptr<DemTileCollection> _demTileCollectionBackBuff;

    WebMapURLGenerator * _urlGenerator;
	int _requestedTiles, _pendingRequests;
	double Lt1, Lt2, Ln1, Ln2;
};

ElevationManager& elevMgr();
