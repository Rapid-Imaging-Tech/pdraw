#ifndef GEODATAROW
#define GEODATAROW
#include <geodatacachesettings.h>
#include <vector>
#include <geokmltile.h>
#include <boost/shared_ptr.hpp>
#include <LoggableObject.h>
#include <geotile.h>
#include <georow.h>
class GeoKmlRow : public GeoRow
{
public:
    GeoKmlRow(int lod, int tiley, int zoomlevel);//{

    ~GeoKmlRow();
    int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);

    boost::shared_ptr<GeoTile> AddWestTile();//create kml tile in this class
    boost::shared_ptr<GeoTile> AddEastTile();//create kml tile in this class
    boost::shared_ptr<GeoTile> CreateTile(int tilex, int column, int row);//create kml tile in this class

    //this class returns kml, an elev tile returns nothing for this
    int GrabDataForROI(double maxLat, double maxLon, double minLat, double minLon, std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > >& points,
                          std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > >& streets,
                        std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > >& streetLabels );

    std::vector<boost::shared_ptr<GeoTile> > GetTilesNotReady(double maxLat, double maxLon, double minLat, double minLon);
private:
    //int _tiley;//tiley of the north line of the row
    int _lod;
    void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
    //int _zoomlevel;
};

#endif // GEODATAROW

