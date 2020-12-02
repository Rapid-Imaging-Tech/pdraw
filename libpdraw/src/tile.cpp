#include "tile.h"
#include <iostream>
#include "utilities.h"
unsigned int sortNum = 0;
//void tile::Update
tile::tile(int x, int y, int zoomLevel):
_carryFlag(false),
  //_centerLat(0.00),
  //_centerLon(0.00),
  _distToVehicle(0.00),
  _x(x),
   _y(y),
   _zoomLevel(zoomLevel),
   _clearCacheFolder(false),
_ubBindingPoint(-1)
{
    UpdateSortOrderNum();
    //std::cout<<"called super constructor"<<endl;
    double westLong = tilex2long(_x, _zoomLevel);
    double eastLong = tilex2long(_x + 1, _zoomLevel);
    _centerLon = (eastLong + westLong) / 2.0;

    double northLat = tiley2lat(_y, _zoomLevel);
    double southLat = tiley2lat(_y + 1, _zoomLevel);
    _centerLat = (northLat + southLat) / 2.0;

    _boundingBox = tile2boundingBox(x, y, zoomLevel);
   // cout<<"centerlat="<<_centerLat<<"centerLon="<<_centerLon<<endl;
}

void tile::UpdateDistToVehicle(double vehLat, double vehLon) {
    _distToVehicle = distLatLonQuick(vehLat, _centerLat, vehLon, _centerLon);
    //cout<<"centerlat="<<_centerLat<<"centerLon="<<_centerLon<<endl;
    //cout<<"disttoveh="<<_distToVehicle<<endl;
}
 void tile::UpdateSortOrderNum()
 {
     sortNum++;
     //TODO: WHAT IF sortnum carries over, ie hits the wall and starts at 0 again?
     if(sortNum == 0)
     {
         _carryFlag = true;
     }
    _sortOrderNum  = sortNum;

 }

bool tile::IsPointInside(const double& lat, const double& lon)
{
    bool insideNS = false;
    bool insideEW = false;
    if(lat >= _boundingBox.south && lat <= _boundingBox.north)
        insideNS = true;
    if(lon >= _boundingBox.west && lon <= _boundingBox.east)
        insideEW = true;

    if(insideNS && insideEW)
        return true;
    else
        return false;
}
