#ifndef ELEVATIONMANAGERLOCAL_H
#define ELEVATIONMANAGERLOCAL_H
#include "dted.h"
#include <boost/shared_ptr.hpp>
#include "UserSettingsData.h"
#include "LoggableObject.h"
/**
 *  \class ElevationManagerLocal
 *  \ingroup LFClientEngine
 *  \note This class is used to read dted cells and generate elevations at a given lat/lon
 *  \defgroup LFClientEngine
 */
class ElevationManagerLocal: public LoggableConfigurableObject
{
private:
    string MakeSRTM_CDTName(double Lat, double Lon) ;
    virtual void SetUserSettings();
public:
    ElevationManagerLocal(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger);
    ~ElevationManagerLocal();
    boost::shared_ptr<dted_ops> World[180][360]; // holds every geocell in the world potentially.
    string DTEDpath;

    int LoadArea(double lat1, double lon1, double lat2, double lon2);
    int LoadCell(double lat1, double lon1);
    // get the elevation in meters
    float GetElevation(double lat, double lon);
    // these indices tell us which geocell to pull within the 180x360 world matrix
    int GetLatIndex(double lat);
    int GetLonIndex(double lon);
    // get the interpolated elevation in meters
    float GetElevationKNN(double lat, double lon);
    string MakeCDTName(double Lat, double Lon);
    int SetCDTpath(string pth);
    int CellsReadCount, CellsWaterOnly;
    int GetCellsReadCount();
    int GetCellsWaterOnlyCount();
    void Clear();
};

#endif // ELEVATIONMANAGERLOCAL_H
