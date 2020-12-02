#pragma once
#ifndef UTILITIES
#define UTILITIES
#include "KMLEntities.h"
#include <vector>
#include <string>

#include "UserSettingsData.h"
#include <boost/shared_ptr.hpp>
#include "vector_tile.pb.h"
#include <StreetSegment.h>

#include "coordinateconverter.h"
double CalculateStreetVectorLength(boost::shared_ptr<Kml::Vectors> lineString, boost::shared_ptr<CoordinateConverter> coordConverter);
void SplitStreetVector(boost::shared_ptr<Kml::Vectors> original, boost::shared_ptr<Kml::Vectors>& firstHalf, boost::shared_ptr<Kml::Vectors>& secondHalf, boost::shared_ptr<CoordinateConverter> coordConverter);
std::string findStringForKey(std::string keyString, vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat);
double DotProduct(double u1, double u2, double v1, double v2);
bool FileExists (const std::string& name, std::string& errorMessage);
bool DeleteFile(const std::string& filename, std::string& errorString);
double distLatLonQuick(double lat1, double lat2, double lon1, double lon2);
void printUserSettings(boost::shared_ptr<UserSettingsData> usettingsdata);

double CreateStreetSegmentVector(boost::shared_ptr<CoordinateConverter> coordConverter,
                                 std::vector<boost::shared_ptr<Kml::Point> > points, std::vector<boost::shared_ptr<StreetSegment> >& streetSegments);
void findCenterLocationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double& x, double& y, double& z,
                                double& lat, double& lon);

double degreeToRadians(double degrees);
double radiansToDegrees(double radians);

void distBetween2latlon(double lat1, double lon1, double lat2, double lon2, double& x, double& y);

int PointInsidePolyNumVertices(int nvert, double* vertx, double* verty, float testx, float testy);

double AbsValue(double val);
//starting at latOrigin and moving North by "offsetDistance" in meters, what is that latitude location
double LatitudeAtNorthOffset(double latOrigin, double offsetDistance);

//starting at latOrigin and moving South by "offsetDistance" in meters, what is that latitude location
double LatitudeAtSouthOffset(double latOrigin, double offsetDistance);

//starting at lonOrigin and moving East by "offsetDistance" in meters, what is that longitude location
double LongitudeAtEastOffset(double lonOrigin, double latOrigin, double offsetDistance);

//starting at lonOrigin and moving West by "offsetDistance" in meters, what is that longitude location
double LongitudeAtWestOffset(double lonOrigin, double latOrigin, double offsetDistance);

double DistanceBetweenGeoCoordinates(double lat1, double lon1, double lat2, double lon2);
double calculateSymbolScaleCamx(double camx, double camy, double camz, double symx, double symy,
                                double symz, double fov, double pixHeight);

double CalculateSymbolZRotDeltaX(double & losHeading);
std::vector<std::string> TokenizeStringBySubstring(std::string inputString, std::string token);
bool DoesStringContainSubstring(std::string inputString, std::string substring);


//VECTOR TILE CALCULATIONS
class BoundingBox {
public:
    double north;
    double south;
    double east;
    double west;
  };

double toDegrees(double val);
int long2tilex(double lon, int z);
int lat2tiley(double lat, int z);
double tilex2long(int x, int z);
double tiley2lat(int y, int z);
//double tile2lon(int x, int z) ;
//double tile2lat(int y, int z) ;
BoundingBox tile2boundingBox(int x, int y, int zoom) ;


#endif // UTILITIES

