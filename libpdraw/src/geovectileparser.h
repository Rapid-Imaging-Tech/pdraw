#ifndef GEOVECTILEPARSER_H
#define GEOVECTILEPARSER_H
#include "utilities.h"
#include <boost/shared_ptr.hpp>
#include "KMLEntities.h"
#include <string>
#include "LoggableObject.h"
//class Point{
//public:
//    Point():lat(0.00), lon(0.00)
//    {};
//    double lat;
//    double lon;
//};
using namespace std;
class geovectileparser : public LoggableConfigurableObject
{
private:
    virtual void SetUserSettings();
public:
    geovectileparser(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger);
    int getCommandInteger(int id, int count);
    int getId(int CommandInteger);
    int getCount(int CommandInteger);
    int getValue(int ParameterInteger);
    void parseMoveTo(const boost::shared_ptr<std::vector<uint32_t> > commands, int& currentIndex, int& currentx, int& currenty);//, BoundingBox& boundBox, double& lat, double& lon);
    void parseLineTo(const boost::shared_ptr<std::vector<uint32_t> > commands, int& currentIndex, int& currentx, int& currenty);//, BoundingBox& boundBox, double& lat, double& lon);
    bool interpolateXY(const BoundingBox& boundBox, int currentx, int currenty, int tilesize, double& lat, double& lon);
    void parseLineString(const boost::shared_ptr<std::vector<uint32_t> > commands, int vectilex, int vectiley, int zoomlevel, int tilesize,
                         boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecStreets, bool& outsideTileN,
                         bool& outsideTileS, bool& outsideTileE, bool& outsideTileW);
                         //boost::shared_ptr<Kml::Vectors> vecStreets);
    void parseLineStringStreetName(const boost::shared_ptr<std::vector<uint32_t> > commands, int vectilex, int vectiley, int zoomlevel, int tilesize,
                                   boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecStreets, string name);
    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > parsePlace(const boost::shared_ptr<std::vector<uint32_t> > commands,
                                                                               int vectilex, int vectiley, int zoomlevel, int tilesize, kmldom::StylePtr ptrStyle, std::string name);


};



#endif // GEOVECTILEPARSER_H
