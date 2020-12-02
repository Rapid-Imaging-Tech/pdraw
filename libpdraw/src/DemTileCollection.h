#pragma once
#include "LoggableObject.h"
#if defined(__APPLE__)
#include <boost/thread.hpp>
#else
#include </usr/local/include/boost/thread.hpp>
#endif
#include "DEMTile.h"
#define TILE_ARRAY_SIZE 9

class DemTileCollection : public LoggableConfigurableObject
{
protected:
    //virtual void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
    virtual void SetUserSettings();
public:
	~DemTileCollection();
    int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);
	DemTileCollection();
	bool AreTilesLoaded();
	
	void Read( std::string path, bool rem);
	
	double GetValueAtPoint(int index, double lat, double lon, double *weight);
	int GetTileCount();
	bool GetIntercept(int index, double lat, double lon, double alt,
		double mx, double my, double mz, 
		double * val, double *latOut, double * lonOut, double * range);
private:
	boost::mutex _mutexTileCount;
	boost::mutex _mutexTileAccess;
	int _tileCount;
	bool _tilesLoaded;
	DEMTile _tiles[TILE_ARRAY_SIZE];
	//int _tileCount;
};
