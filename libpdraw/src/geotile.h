#ifndef GEOTILE_H
#define GEOTILE_H
#include <boost/thread.hpp>
#include <KMLEntities.h>
#include <geotileservergl.h>
class GeoTile: public LoggableConfigurableObject
{
public:

    virtual  bool LoadData() = 0;//each class has it's own loaddata
    GeoTile();
    //kml and elev
    double GetLowerLat();
    double GetLowerLon();
    double GetUpperLat();
    double GetUpperLon();
    double GetCenterLat();
    double GetCenterLon();
    double GetTileX(){return _tilex;};
    double GetTileY(){return _tiley;};
    bool GetDataLoaded();

    void SetDistToStarePoint(double dist);//{_distToStarePoint = dist;};
    double GetDistToStarePoint();//{return _distToStarePoint;};
    bool CheckTileReady();
    int GetUID();//{return _UID;};

    //virtual void GrabDataForStarePoint(double starePointLat, double starePointLon, std::vector<boost::shared_ptr<Kml::Point> >& points,
      //                    std::vector<boost::shared_ptr<Kml::Vectors> >& streets, std::vector<boost::shared_ptr<Kml::StreetLabel> >& streetLabels ) ;

    virtual void GrabDataForROI(double maxLat, double maxLon, double minLat, double minLon, std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > >& points,
                          std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > >& streets,
                                std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > >& streetLabels ) ;



    int _originalColumn;//TODO: THESE ARE JUST FOR DEBUGGING AND MAYBE BE CONFUSING ONCE GRID SHIFTS AROUND, REMOVE THEM
    int _originalRow;//TODO: TH

    boost::mutex _mutexDataLoaded;

    double _distToStarePoint;

    bool _dataLoaded;
    int _tilex;
    int _tiley;
    int _zoomlevel;
    //double _latCenter;
    //double _lonCenter;

    //double _lowerLat;
    //double _lowerLon;
    //double _upperLat;
    //double _upperLon;

    int _UID;
};

#endif // GEOTILE_H

