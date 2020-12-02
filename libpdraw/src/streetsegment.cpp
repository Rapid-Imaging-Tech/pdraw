#include "StreetSegment.h"
#include <math.h>

StreetSegment::StreetSegment(double x1, double y1, double x2, double y2, double lat1, double lon1, double lat2, double lon2,
                             double alt, double startPoint):
_x1(x1),
_y1(y1),
_x2(x2),
_y2(y2),
_startPoint(startPoint),
_elevation(alt),
_endPoint(0.00),
_distance(0.00),
_lat1(lat1),
_lon1(lon1),
_lat2(lat2),
_lon2(lon2)
{

}
double StreetSegment::GetStartPoint()
{
    return _startPoint;
}
double StreetSegment::GetEndPoint()
{
    //get the dist between lat1lon1 lat2lon2, add that to start point and return the value
    double deltaX = _x1 - _x2;
    double deltaY = _y1 - _y2;
    _distance = sqrt((deltaX * deltaX) + (deltaY * deltaY));
    _endPoint = _startPoint + _distance;
    return _endPoint;
}
