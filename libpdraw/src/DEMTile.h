//
// DEMTile.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2015 Eric Cox for Rapid Imaging Software Inc.
//
#pragma once
//LibGeoTiff header
#include "geotiff.h"
//LibTiff headers
#include "xtiffio.h"
#include "tiffio.hxx"
#include "tiffutil.h"
#include "LoggableObject.h"
#include "SViewData.h"
class DEMTile;
//class ElevationTreeNode
//{//Elevation treenode serves a quad-tree based index of the values in DEMTile
// //This allows for significantly faster intercept searching
// //It could also be used to render quickly render the DEM at different zoom levels
//private:
//	bool isLeaf;
//    boost::shared_ptr<ElevationTreeNode> children[4];
//	ElevationTreeNode(double xmin, double ymin, double xmax, double ymax,
//		int index, int maxIndex, DEMTile * elevations);
//	void SetupBranch(int index, int maxIndex, DEMTile * elevations);
//	bool CheckKid(double x, double y, bool checked[], double x0, double y0, double z0,
//		double mx, double my, double mz, double * val,double *lat, double *lon, double *range);
//public:
//    ~ElevationTreeNode();
//	//The basic data we need to store is a rectangular prism that encloses the elevation surface
//	//Storing xmid and ymid saves on recalculating it for various operations
//	double _xmin, _xmax, _xmid, _ymin, _ymax, _ymid;
//	float _maxel, _minel;
//	ElevationTreeNode(DEMTile * elevations);
//	bool GetIntercept(double x0, double y0, double z0,
//		double mx, double my, double mz, double * val,double *lat, double * lon, double * range);
//	float GetMinElevation(){ return this->_minel; };
//	float GetMaxElevation() { return this->_maxel; };
//};
class DEMTile : public LoggableConfigurableObject
{//DEMTile represents the elevation data in a single geotiff-float32 image
protected:
    //virtual void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
    virtual void SetUserSettings();
public:
    int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);
	double GetValueAtPoint(double lat, double lon, double *weight);
	double GetValueAtCartesian(double x, double y);
	void ConvertToCartesian(double lat, double lon, double *xOut, double *yOut);
	void ConvertToGeodetic(double x, double y, double *lat, double *lon);
	bool GetIntercept(double lat, double lon, double alt,
		double mx, double my, double mz, 
		double * val, double *latOut, double * lonOut, double * range);
	bool GetIntercept2(double lat, double lon, double alt,
		double mx, double my, double mz,
		double * val, double *latOut, double * lonOut, double * range);
	DEMTile();
	~DEMTile();
	void Read(std::istream * stream);//Not working
	void Read(std::string path, bool remove);
	void Read(TIFF * tiff, GTIF * geo);
	int GetWidth(){ return this->_pixelWidth; };
	double GetMidLat(){ return (this->_lat1 + this->_lat2) / 2.0; };
	void GetBounds(double * lt1, double * lt2, double * ln1, double * ln2);
	
private:
	void ReadData(TIFF * t);
	double PixelValue(int x, int y);
	int _pixelWidth, _pixelHeight;
	float * _data_array;//Holds the elevation values

	double _lat1, _lon1, _lat2, _lon2,_cLat,_cLon;
	double _xScale, _yScale;
	double _xmin, _ymin, _xmax, _ymax;
    //boost::shared_ptr<ElevationTreeNode> _searchTree;
};
