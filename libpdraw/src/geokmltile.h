#ifndef GEODATA
#define GEODATA
#include <geotile.h>
#include <boost/shared_ptr.hpp>
#include <UserSettingsData.h>
#include <StatusLogger.h>
#include <LoggableObject.h>
#include <WMSQuery.h>
#include <vector>
#include <KMLEntities.h>
#include <boost/thread.hpp>
#include <customkmlfunctions.h>
#include <geotileservergl.h>
using namespace Kml;

class GeoKmlTilebak :  public GeoTile
{
public:
    GeoKmlTilebak(double lowLat, double lowLon, double upperLat, double upperLon, int colunmn, int row, int lod);
    ~GeoKmlTilebak();
    int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);
    virtual bool LoadData();//geotileservergl* server);

    //for kml only
    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > GetPolygons();
    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > GetStreets();
    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > GetStreetLabels();
    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > GetPoints();
    //void GrabDataForStarePoint(double starePointLat, double starePointLon, std::vector<boost::shared_ptr<Kml::Point> >& points,
      //                    std::vector<boost::shared_ptr<Kml::Vectors> >& streets, std::vector<boost::shared_ptr<Kml::StreetLabel> >& streetLabels );

    void GrabDataForROI(double maxLat, double maxLon, double minLat, double minLon, std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > >& points,
                          std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > >& streets,
                        std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > >& streetLabels );

private:
     boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > _points;
     boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > _points9;
     boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > _streets;
     boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > _streetLabels;

    int _lod;
    int _numEntities;
    int _numEntities9;//this is only needed for lod12, which uses lod9 for placemarks otherwise way too many and slowing down cpu

    boost::shared_ptr<CustomKmlFunctions> _gEntities;
    boost::shared_ptr<CustomKmlFunctions> _gEntities9;//add additional lod 9 for symbols

    std::string _kml;//todo: remove later?
    std::string _kml9;

    std::string SetKmlList();
    std::string GetKMLSCServerDC(double roiMaxLat, double roiMaxLon, double roiMinLat, double roiMinLon, int lod, DATATYPE dataType);
    std::string _kmlPort;
    std::string _kmlURL;
    void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);

};

#endif // GEODATA

