#include "utilities.h"
#include <math.h>
#include "consts.h"
#include <string>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <iostream>
#include <sys/stat.h>
#include "boost/filesystem.hpp"
#include <sstream>
//VECTOR TILE CALCULATIONS
using namespace std;

//struct stat sb;
//u<u1, u2> v<v1, v2>
double CalculateStreetVectorLength(boost::shared_ptr<Kml::Vectors> lineString, boost::shared_ptr<CoordinateConverter> coordConverter){
    //using namespace Kml;
    int length = 0;
    //NSMutableArray* points = lineString.coordinateArray;
    std::vector<boost::shared_ptr<Kml::Point> > points = lineString->GetStreetVectors();// coordinateArray;
    size_t size = points.size();// count];

    if(size >= 2)
    {
        for(int i = 1 ; i < size ; i++)
        {
            boost::shared_ptr<Kml::Point> coord1 = points[i];
            boost::shared_ptr<Kml::Point> coord2 = points[i-1];// objectAtIndex:i-1];
            double x1, y1, z1 = 0.00;
            double x2, y2, z2 = 0.00;
            coordConverter->GeodeticToCartesian( coord1->GetLat(), coord1->GetLon(), coord1->GetAlt(), x1, y1, z1);
            coordConverter->GeodeticToCartesian( coord2->GetLat(), coord2->GetLon(), coord2->GetAlt(), x2, y2, z2);

            double deltaX = abs(x1 - x2);
            double deltaY = abs(y1 - y2);
            double segLength = sqrt((deltaX * deltaX) + (deltaY * deltaY));
            length += segLength;
        }
    }
    return length;
}
void SplitStreetVector(boost::shared_ptr<Kml::Vectors> original, boost::shared_ptr<Kml::Vectors>& firstHalf, boost::shared_ptr<Kml::Vectors>& secondHalf, boost::shared_ptr<CoordinateConverter> coordConverter)
{
    std::vector<boost::shared_ptr<Kml::Point> > pointsOrig = original->GetStreetVectors();
    double totalLength = CalculateStreetVectorLength(original, coordConverter);
    const double halfWayLength = totalLength / 2.0;
    
    //create new streetlinevector for first half of split
    firstHalf.reset(new Kml::Vectors);
    std::vector<boost::shared_ptr<Kml::Point> > firstHalfPoints;
    
    //find the midpoint of the full streetvector list, we'll use as the last point of the first half, and first point of 2nd half
    double midptx = 0.0;
    double midpty = 0.0;
    double midptz = 0.0;
    double midLat = 0.0;
    double midLon = 0.0;
    double midalt = 0.0;
    original->FindMidPoint(midptx, midpty, midptz);
    boost::shared_ptr<Kml::Point> midPointOrig(new Kml::Point);
    midPointOrig->SetX(midptx);
    midPointOrig->SetY(midpty);
    midPointOrig->SetZ(midptz);
    
    coordConverter->CartesianToGeodetic(midptx, midpty, midptz, midLat, midLon, midalt);
    midPointOrig->SetLat(midLat);
    midPointOrig->SetLon(midLon);
    midPointOrig->SetAlt(midalt);
    
    //do a for loop here adding pts to firsthalf until we get to midptx/y
    double currentDistance = 0.0;
    
    //the first point of the first half of the original street vec list is just the first point of the orig list
    //keep adding existing points from orignal street vec list until we go beyond the midpoint, then just add the midpoint of the orig list
    //as the last point of the first half streetvec list
    int index = 0;
    while(halfWayLength > currentDistance){
        firstHalfPoints.push_back(pointsOrig[index]);
        currentDistance += original->DistanceBetweenPoints(pointsOrig[index], pointsOrig[index + 1]);
        index++;
    }
    firstHalfPoints.push_back(midPointOrig);//midpoint of the original line is the end of the 1st half line
    
    //set the first half streetvectors for calling method to use
    firstHalf->SetStreetVectors(firstHalfPoints);
    
    /////////////now do the 2nd half/////////////////
    secondHalf.reset(new Kml::Vectors);
    std::vector<boost::shared_ptr<Kml::Point> > secondHalfPoints;
    
    secondHalfPoints.push_back(midPointOrig);//midpoint of the original line is the start of the second line
    //just add the rest of the street vector points all the way to the end of the original streetline vec since this is the second half
    //index should point at the point that is just beyond the midpoint of the original line
    while(index < pointsOrig.size()){
        secondHalfPoints.push_back(pointsOrig[index]);
        index++;
    }
    //set the second half streetvectors for calling method to use
    secondHalf->SetStreetVectors(secondHalfPoints);
}
///create a vector of street segments, which are basically a set of lines with a start x,y and end x,y
/// but also gives it a start and endpoint for distance.  ie 2 street segments, 1 of them is 50 units long
/// the other is 100, the start and end of the first segment is 0 and 50, the start and end of the second
/// segment is 50 150
double CreateStreetSegmentVector(boost::shared_ptr<CoordinateConverter> coordConverter,
                                 std::vector<boost::shared_ptr<Kml::Point> > points, std::vector<boost::shared_ptr<StreetSegment> >& streetSegments){
    double streetLength = 0.00;
    double endPoint = 0.0;
    for(int j = 0 ; j < points.size()-1 ; j++)
    {
        boost::shared_ptr<Kml::Point> point1 = points[j];
        double x1, y1, z1 = 0.00;
        double lat1 = point1->GetLat();
        double lon1 = point1->GetLon();
        double alt1 = point1->GetAlt();
        coordConverter->GeodeticToCartesian( lat1, lon1, alt1, x1, y1, z1);

        point1->SetX(x1);
        point1->SetY(y1);
        point1->SetZ(z1);

        boost::shared_ptr<Kml::Point> point2 = points[j + 1];
        double x2, y2, z2 = 0.00;
        double lat2 = point2->GetLat();
        double lon2 = point2->GetLon();
        double alt2 = point2->GetAlt();
        coordConverter->GeodeticToCartesian( lat2, lon2, alt2, x2, y2, z2);
        point2->SetX(x2);
        point2->SetY(y2);
        point2->SetZ(z2);

        boost::shared_ptr<StreetSegment> seg(new StreetSegment(x1, y1, x2, y2, lat1, lon1, lat2, lon2, alt1, endPoint));
        endPoint = seg->GetEndPoint();
        streetSegments.push_back(seg);
        streetLength = endPoint;
    }
    return streetLength;
}

/// find out which street segment contains the center location of the entire linestring of streetsegmens, then get the x, y location
/// of the actual center point of the linestring.


void findCenterLocationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double& x, double& y, double& z,
                                double& lat, double& lon){
    int count = segments.size();
    boost::shared_ptr<StreetSegment> lastSegment = segments[count-1];
    double endpoint = lastSegment->GetEndPoint();
    double midway = endpoint / 2.0;
    bool foundLocation = false;
    for(int i = 0 ; i < count && foundLocation == false; i++)
    {
        boost::shared_ptr<StreetSegment> seg = segments[i];
        if(midway > seg->GetStartPoint() && midway < seg->GetEndPoint())
        {
            //this segment contains the midpoint
            double ratio = (midway - seg->GetStartPoint()) / (seg->GetEndPoint() - seg->GetStartPoint());
            x = (( seg->GetX2() - seg->GetX1())* ratio) + seg->GetX1();
            lon = (( seg->GetLon2() - seg->GetLon1())* ratio) + seg->GetLon1();
            y = (( seg->GetY2() - seg->GetY1())* ratio) + seg->GetY1();
            lat = (( seg->GetLat2() - seg->GetLat1())* ratio) + seg->GetLat1();
            z = seg->GetElevation();

            foundLocation = true;
        }
    }
}

std::string findStringForKey(std::string keyString, vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat)
{
    int indexForKey =-1;
    std::string returnString;

    for (std::size_t k=0;k<static_cast<std::size_t>(layer.keys_size());++k)
    {
        std::string const& key = layer.keys(k);
        if(key.compare(keyString) == 0)
        {
            //this is the index we want to look into values field for the name
            indexForKey = k;
        }
    }

    for (std::size_t m=0; m < static_cast<std::size_t>(feat.tags_size()) ; m += 2)
    {
        uint32_t tagkey = feat.tags(m);
        uint32_t tagvalue = feat.tags(m + 1);
        if(tagkey == indexForKey)
        {
            vector_tile::Tile_Value val = layer.values(tagvalue);
            if(val.has_string_value())
            {
                returnString = val.string_value();
            }
            if(val.has_int_value())
            {
                std::stringstream ss;
                ss<<"intval:"<<val.int_value();
                returnString = ss.str();
            }
            if(val.has_bool_value())
            {
                std::stringstream ss;
                ss<<"boolval:"<<val.bool_value();
                returnString = ss.str();
            }
            if(val.has_double_value())
            {
                std::stringstream ss;
                ss<<"doubleval:"<<val.double_value();
                returnString = ss.str();
            }
            if(val.has_float_value())
            {
                std::stringstream ss;
                ss<<"floatval:"<<val.float_value();
                returnString = ss.str();
            }
            if(val.has_sint_value())
            {
                std::stringstream ss;
                ss<<"sintval:"<<val.sint_value();
                returnString = ss.str();
            }
            if(val.has_uint_value())
            {
                std::stringstream ss;
                ss<<"uint:"<<val.uint_value();
                returnString = ss.str();
            }
        }
    }
    return returnString;
}
double DotProduct(double u1, double u2, double v1, double v2)
{
    //xdoty = x1y1 + x2y2
    return u1 * v1 + u2 * v2;
}

bool FileExists (const std::string& name, std::string& errorMessage) {
  struct stat buffer;
  bool retValue = stat (name.c_str(), &buffer) == 0;
  if(retValue == false){
      errorMessage.append("could not find file: ");
      errorMessage.append(name);
  }
  return retValue;
}
bool DeleteFile(const std::string& filename, std::string& errorString)
{
    bool success = true;
    if(FileExists(filename, errorString))
    {
        boost::filesystem::remove(filename);
        success = true;
    }
    else
    {
        success = false;
    }
    return success;
}

void printUserSettings(boost::shared_ptr<UserSettingsData> usettingsdata){

    //std::map<std::string, std::string> _stringSettings;
    //std::map<std::string, int> _intSettings;
    //std::map<std::string, double> _doubleSettings;
    typedef map<string, string>::const_iterator MapIterator;
    for (MapIterator iter = usettingsdata->_stringSettings.begin(); iter != usettingsdata->_stringSettings.end(); iter++)
    {
        cout << iter->first << "= " << iter->second<<endl;
    }

    typedef map<string, int>::const_iterator MapIteratorInt;
    for (MapIteratorInt iter = usettingsdata->_intSettings.begin(); iter != usettingsdata->_intSettings.end(); iter++)
    {
        cout << iter->first << "= " << iter->second<<endl;
    }

    typedef map<string, double>::const_iterator MapIteratorDouble;
    for (MapIteratorDouble iter = usettingsdata->_doubleSettings.begin(); iter != usettingsdata->_doubleSettings.end(); iter++)
    {
        cout << iter->first << "= " << iter->second<<endl;
    }
}

double degreeToRadians(double degrees)
{
    return degrees * 0.0174533;
}

double radiansToDegrees(double radians)
{
    return radians * 57.2958;
}


void distBetween2latlon(double lat1, double lon1, double lat2, double lon2, double& x, double& y)
{

    double R = 6378737;//Earth's nmean radius in meters
    //gulf of guinea
    //double lat1 = 0.00;
    //double lon1 = 0.00;

    //somewhere around Esko MN
    //double lat2 = 46.00;
    //double lon2 = -92.00;

    double sigma1 = degreeToRadians(lat1);
    double sigma2 = degreeToRadians(lat2);

    double lambda1 = degreeToRadians(lon1);
    double lambda2 = degreeToRadians(lon2);

    x = (lambda2 - lambda1) * cos((sigma1 + sigma2)/2) * R;
    y = (sigma2 - sigma1) * R;
}


double toDegrees(double val)
{
    return val * 57.295779513;
}
int long2tilex(double lon, int z)
{
    return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

int lat2tiley(double lat, int z)
{
    return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}

double tilex2long(int x, int z)
{
    return x / pow(2.0, z) * 360.0 - 180;
}

double tiley2lat(int y, int z)
{
    double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}
//double tile2lon(int x, int z) {
//   return x / pow(2.0, z) * 360.0 - 180;
//}

//double tile2lat(int y, int z) {
//  double n = 3.14 - (2.0 * 3.14 * y) / pow(2.0, z);
//  return toDegrees(atan(sinh(n)));
//}

  BoundingBox tile2boundingBox(int x, int y, int zoom) {
    BoundingBox bb;// = new BoundingBox();
    bb.north = tiley2lat(y, zoom);
    bb.south = tiley2lat(y + 1, zoom);
    bb.west = tilex2long(x, zoom);
    bb.east = tilex2long(x + 1, zoom);
    return bb;
  }


int PointInsidePolyNumVertices(int nvert, double* vertx, double* verty, float testx, float testy)
{
    int i, j, c = 0;
    for(i = 0, j = nvert - 1 ; i<nvert; j=i++)
    {
        if(((verty[i] > testy) != (verty[j] > testy)) &&
           (testx<(vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
            c = !c;
    }
    return c;
}

double CalculateSymbolZRotDeltaX(double & losHeading)
{
    double rotationAboutZ = 0.00;
    double thetaRadians = 0.00;
    double thetaDegrees = 0.00;

    while(losHeading > 360.0){
        losHeading -= 360.0;
    }

    while(losHeading < 0.00){
        losHeading += 360.0;
    }

    if(losHeading >= 0 && losHeading <=90.0){
        thetaDegrees = 90.0 - losHeading;
        rotationAboutZ = -1*(90.0 - thetaDegrees + 180.0);
    }
    else if(losHeading >= 90 && losHeading <= 180.0){
        thetaDegrees = losHeading - 90.0;
        rotationAboutZ = (90.0 - thetaDegrees) ;//add 180 to flip the icon to initially face north
    }
    else if(losHeading >= 180 && losHeading <= 270.0){
        thetaDegrees  = 90.0 - losHeading ;
        rotationAboutZ = -1.0*(90.0 - thetaDegrees + 180.0);//add 180 to flip the icon to initially face north
    }
    else if(losHeading >= 270.0 && losHeading <= 360.0){
        thetaDegrees  = losHeading - 270.0;
        rotationAboutZ = (90.0 - thetaDegrees + 180.0);
    }
    return rotationAboutZ;
}

double AbsValue(double val){
    if(val < 0.00){
        val = (-1) * val;
    }
    return val;
}

//starting at latOrigin and moving North by "offsetDistance" in meters, what is that latitude location
double LatitudeAtNorthOffset(double latOrigin, double offsetDistance){
    double degreeNorthInMeters = 111320.0;
    double newLat = latOrigin + (offsetDistance / degreeNorthInMeters);
    return newLat;
}

//starting at latOrigin and moving South by "offsetDistance" in meters, what is that latitude location
double LatitudeAtSouthOffset(double latOrigin, double offsetDistance){
    double degreeNorthInMeters = 111320.0;
    double newLat = latOrigin - (offsetDistance / degreeNorthInMeters);
    return newLat;
}

//starting at lonOrigin and moving East by "offsetDistance" in meters, what is that longitude location
double LongitudeAtEastOffset(double lonOrigin, double latOrigin, double offsetDistance){
    double newLon = lonOrigin + (offsetDistance/(DegreeNorthInMeters*cos(latOrigin*PI180)));
    return newLon;
}

//starting at lonOrigin and moving West by "offsetDistance" in meters, what is that longitude location
double LongitudeAtWestOffset(double lonOrigin, double latOrigin, double offsetDistance){
    double newLon = lonOrigin - (offsetDistance/(DegreeNorthInMeters * cos(latOrigin*PI180)));
    return newLon;
}


double DistanceBetweenGeoCoordinates(double lat1, double lon1, double lat2, double lon2){
    //converting to meters and centered at center of latest roi
    //double pi180  =  0.0174532925;
    double degreeNorthInMeters = 111320.0;
    double x = (lon2 - lon1) * cos(lat2 * PI180) * degreeNorthInMeters;
    double y = (lat2 - lat1) * degreeNorthInMeters;
     double dist = sqrt(x * x + y * y);
     return dist;
    //z = altitude;
}
double distLatLonQuick(double lat1, double lat2, double lon1, double lon2)
{
    double deltaLatSqr = pow(lat1 - lat2, 2);
    double deltaLonSqr = pow(lon1 - lon2, 2);
    return sqrt(deltaLatSqr + deltaLonSqr);

}

std::vector<std::string> TokenizeStringBySubstring(std::string inputString, std::string token)
{
    std::vector<std::string> strVec;

    typedef std::vector<std::string> Tokens;
    Tokens tokens;
    boost::split( tokens, inputString, boost::is_any_of(token) );

    //std::cout << tokens.size() << " tokens" << std::endl;
    BOOST_FOREACH( const std::string& i, tokens ) {
        strVec.push_back(i);
    }
    return strVec;
}
bool DoesStringContainSubstring(std::string inputString, std::string substring)
{
    bool contained = false;
    if (inputString.find(substring) != std::string::npos)
    {
        contained = true;
        //std::cout << "found!" << '\n';
    }
    return contained;
}
