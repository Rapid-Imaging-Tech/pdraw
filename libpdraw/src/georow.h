#pragma once
#include <LoggableObject.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <KMLEntities.h>
#include <geotile.h>

class GeoRow : public LoggableConfigurableObject
{
public:
    //kml tile returns nothing, elev tile returns elevation at that point
    virtual double GetElevationAt(double lat, double lon);
    virtual boost::shared_ptr<GeoTile> AddWestTile() = 0;

    virtual boost::shared_ptr<GeoTile> AddEastTile() = 0;

    virtual boost::shared_ptr<GeoTile> CreateTile(int tilex, int column, int row) = 0;


    double GetLowerLat();

    double GetUpperLat();

    void RemoveEastTile();
    void RemoveWestTile();

    //for kml only
    //virtual void GrabDataForStarePoint(double starePointLat, double starePointLon, std::vector<boost::shared_ptr<Kml::Point> >& points,
      //                    std::vector<boost::shared_ptr<Kml::Vectors> >& streets, std::vector<boost::shared_ptr<Kml::StreetLabel> >& streetLabels );
    virtual int GrabDataForROI(double maxLat, double maxLon, double minLat, double minLon, std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > >& points,
                          std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > >& streets,
                                std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > >& streetLabels );

    std::vector<boost::shared_ptr<GeoTile> > GetTilesNotReady(double maxLat, double maxLon, double minLat, double minLon);
     std::vector<boost::shared_ptr<GeoTile> > GatherTilesForRoi(double maxLat, double maxLon, double minLat, double minLon, bool fullGrid);
protected:



    std::vector<boost::shared_ptr<GeoTile> > _vecGeoData ;//todo: rename to tiles
    int _tiley;
    int _zoomlevel;
    //double _latLower;
    //double _latUpper;
};
