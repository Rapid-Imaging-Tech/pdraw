#ifndef RAFOOTPRINT_H
#define RAFOOTPRINT_H
#include "RotationToolKit.h"
#include "RotationTK.h"
#include "elevationmanagerlocal.h"
#include <boost/shared_ptr.hpp>
#include "LFDefinitionsRA.h"
#include "boost/shared_array.hpp"
#include "utilities.h"
#include "coordinateconverter.h"
#if defined(__APPLE__)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#else
#include <QOpenGLBuffer>
#include <qopenglshaderprogram.h>
#endif
#include "LoggableObject.h"
struct FootPrint {
public:
    double fpLat[4],fpLon[4],fpAlt[4];
    int fpValid; // 0 invalid, 1, partially valid, 4 all valid
    int MouseX,MouseY;
    double MouseLat,MouseLon, MouseAlt, SPLat, SPLon;
};
class RAFootprint : public LoggableConfigurableObject
{
private:
    double ComputePOIPitchAboveSP(double vfov, double windowheight, int y);
    double ComputePOIHorzDeltaRightOfSP(double vfov, double windowwidth, double windowheight, int x, int vpwidth, int vpheight);
    virtual void SetUserSettings();
    //QOpenGLBuffer _arrayBuf;
    boost::shared_ptr<CoordinateConverter> _coordConverter;
    //BoundingBox _boundingBox;
    boost::shared_array<VertexDataLW> _tileGridVertices;
public:
    int GetLatLon4Pixels(boost::shared_ptr<SViewData> svd, double x, double y, double *lat, double  *lon, double  *elev, int windowWidth, int windowHeight, int vpwidht, int vpheight);
//    int GetLatLon4Pixels(boost::shared_ptr<SViewData> svd, double x, double y, double *lat, double  *lon, double  *elev,
//                         int windowWidth, int windowHeight);
    void SetCoordConverter(boost::shared_ptr<CoordinateConverter> coordConv)
    {
        _coordConverter = coordConv;
    }
    
    ;
    double GetElevationAtLocation(double lat, double lon, double& z);
    int GetTerrainIntercept(double lat, double lon, double alt, double p, double h, double r, double *lt, double *ln, double *al, double *range);
    int SetEML(boost::shared_ptr<ElevationManagerLocal> eml);
    RotationTK _rtk;
#if defined(__APPLE__)
    void Render(GLuint program);
#else
    void Render(QOpenGLShaderProgram* program);
#endif
    // this is the elevationmanagelocal storing elevation cells
    boost::shared_ptr<ElevationManagerLocal> _EML;
    
    
    FootPrint _FP;
    RAFootprint(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger);
    int GetLatLon4Pixels(double lat, double lon, double alt,
                         double x, double y,
                         double p, double h, double r,
                         double *lt, double  *ln, double  *al);
    int GetPixels4LatLon(double clat, double clon, double calt,
                         double lat, double lon, double alt,
                         double  p, double h, double r,
                         double  ifov, int nx, int ny,
                         double *x, double*y, double *d);
    FootPrint ComputePrecisionFootprint(double lat, double lon, double alt, double pitch, double heading, double roll,
                                        double camPitch, double camHeading, double camRoll, double hFOV, double FOV, double windowWidth, double windowHeight);
};

#endif // RAFOOTPRINT_H



//#ifndef RAFOOTPRINT_H
//#define RAFOOTPRINT_H
//#include <RotationToolKit.h>
//#include "RotationTK.h"
//#include "elevationmanagerlocal.h"
//#include <boost/shared_ptr.hpp>
//#include "LFDefinitionsRA.h"
//#include "boost/shared_array.hpp"
//#include "utilities.h"
//#include "coordinateconverter.h"
//#include <QOpenGLBuffer>
//#include <qopenglshaderprogram.h>
//#include "LoggableObject.h"
//struct FootPrint {
//public:
//    double fpLat[4],fpLon[4],fpAlt[4];
//    int fpValid; // 0 invalid, 1, partially valid, 4 all valid
//    int MouseX,MouseY;
//    double MouseLat,MouseLon, MouseAlt, SPLat, SPLon;
//};
//class RAFootprint : public LoggableConfigurableObject
//{
//private:
//    virtual void SetUserSettings();
//    QOpenGLBuffer _arrayBuf;
//    boost::shared_ptr<CoordinateConverter> _coordConverter;
//    //BoundingBox _boundingBox;
//    boost::shared_array<VertexDataLW> _tileGridVertices;
//public:
//    void SetCoordConverter(boost::shared_ptr<CoordinateConverter> coordConv)
//    {
//        _coordConverter = coordConv;
//    }

//    ;
//    double GetElevationAtLocation(double lat, double lon, double& z);
//    int GetTerrainIntercept(double lat, double lon, double alt, double p, double h, double r, double *lt, double *ln, double *al, double *range);
//    int SetEML(boost::shared_ptr<ElevationManagerLocal> eml);
//    RotationTK _rtk;
//    void Render(QOpenGLShaderProgram* program);
//// this is the elevationmanagelocal storing elevation cells
//    boost::shared_ptr<ElevationManagerLocal> _EML;


//    FootPrint _FP;
//    RAFootprint(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger);
//    int GetLatLon4Pixels(double lat, double lon, double alt,
//                         double x, double y,
//                         double p, double h, double r,
//                         double *lt, double  *ln, double  *al);
//    int GetPixels4LatLon(double clat, double clon, double calt,
//                         double lat, double lon, double alt,
//                         double  p, double h, double r,
//                          double  ifov, int nx, int ny,
//                         double *x, double*y, double *d);
//    FootPrint ComputePrecisionFootprint(double lat, double lon, double alt, double pitch, double heading, double roll,
//        double camPitch, double camHeading, double camRoll, double hFOV, double FOV, double windowWidth, double windowHeight);
//};

//#endif // RAFOOTPRINT_H
