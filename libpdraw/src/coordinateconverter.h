#ifndef COORDINATECONVERTER
#define COORDINATECONVERTER
class CoordinateConverter{
public:
    void CartesianToGeodetic(double x, double y, double z, double& lat, double& lon, double& alt);
    CoordinateConverter():
    _baselat(0.00),
    _baselon(0.00),
    _degreeNorthInMeters ( 111320.0)
    {};

    CoordinateConverter(double baselat, double baselon);
    ~CoordinateConverter();
    void GeodeticToCartesian(double lat, double lon, double altitude,
                             double& x, double& y, double& z);


    double _baselat;
    double _baselon;
    double _degreeNorthInMeters;

};

#endif // COORDINATECONVERTER
