#include "coordinateconverter.h"
#include <math.h>
#include "consts.h"
#include <iostream>
//int uniqueID = 1;
//int CoordinateConverter::GetUniqueID(){
//    return _uniqueId;
//}

CoordinateConverter::CoordinateConverter(double baselat, double baselon):
_baselat(baselat),
_baselon(baselon),
_degreeNorthInMeters ( 111320.0)
{
    std::cout<<"creating coordconv"<<std::endl;
}

CoordinateConverter::~CoordinateConverter(){

}

void CoordinateConverter::CartesianToGeodetic(double x, double y, double z, double& lat, double& lon, double& alt)
{
    //calulate lat first because lon relies on it
    lat = (y / _degreeNorthInMeters) + _baselat;
    lon = (x / (cos(lat * PI180) * _degreeNorthInMeters)) + _baselon;
    alt = z;
}


//void LFClientEngine::LFGeodeticToCartesian(double lat, double lon, double altitude,
//                         double& x, double& y, double& z)
//{
//    double _degreeNorthInMeters = 111320.0;

//    //converting to meters and centered at center of latest roi
//    x = (lon - _baseLon)*cos(lat * PI180) * _degreeNorthInMeters;
//    y = (lat - _baseLat)*_degreeNorthInMeters;
//    z = altitude;

//}

void CoordinateConverter::GeodeticToCartesian(double lat, double lon, double altitude,
                                              double& x, double& y, double& z)
{
    //converting to meters and centered at center of latest roi
    x = (lon - _baselon)*cos(lat * PI180) * _degreeNorthInMeters;
    y = (lat - _baselat)*_degreeNorthInMeters;
    z = altitude;
}

//#include <coordinateconverter.h>
//#include <math.h>
//#include <consts.h>
////int uniqueID = 1;
////int CoordinateConverter::GetUniqueID(){
////    return _uniqueId;
////}

//CoordinateConverter::CoordinateConverter(double baselat, double baselon):
//    _baselat(baselat),
//    _baselon(baselon),
//    _degreeNorthInMeters ( 111320.0)
//{

//}

//CoordinateConverter::~CoordinateConverter(){

//}

//void CoordinateConverter::GeodeticToCartesian(double lat, double lon, double altitude,
//                         double& x, double& y, double& z)
//{
//    //converting to meters and centered at center of latest roi
//    x = (lon - _baselon)*cos(lat * PI180) * _degreeNorthInMeters;
//    y = (lat - _baselat)*_degreeNorthInMeters;
//    z = altitude;
//}
