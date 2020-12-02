#pragma once

class StreetSegment
{
public:
    StreetSegment(double x1, double y1, double x2, double y2, double lat1, double lon1, double lat2, double lon2, double alt, double startPoint);
    //void Init();//calculate length of segment, calculate endpoint ie startpoint + length
    bool IsInRange(double val);//does val fit between startPoint and endpoint?
    double GetEndPoint();
    double GetStartPoint();
    double GetX1(){return _x1;};
    double GetX2(){return _x2;};
    double GetY1(){return _y1;};
    double GetY2(){return _y2;};
    double GetLat1(){return _lat1;};
    double GetLon1(){return _lon1;};
    double GetLat2(){return _lat2;};
    double GetLon2(){return _lon2;};

    double GetElevation(){return _elevation;};
private:
    double _lat1;
    double _lon1;
    double _lat2;
    double _lon2;

    double _x1;
    double _y1;
    double _x2;
    double _y2;

    /*double _lat1;
     double _lon1;
     double _lat2;
     double _lon2;*/
    double _startPoint;
    double _endPoint;
    double _distance;
    double _elevation;
};
