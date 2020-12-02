// RotationTK.h: interface for the RotationTK class.
//
//////////////////////////////////////////////////////////////////////
// Copyright 1996-2015 MICHAEL F. ABERNATHY all rights reserved
///////////////////////////////////////////////////////////////////////

#pragma once
#ifndef SIXDOF_TYPE

typedef struct sixdof {
    double latitude, longitude; // position in degrees of graticule
    double pitch;    // angle of nose w/r/t horizon measured upward positive
    double heading; // angle of nose w/r/t geographic north
    double roll;    // angle of rotation about the line of sight, clockwise positive from the vertical
    double altitude; // altitude in meters
    int AGL;        //  if AGL is 0, altitude is Above Mean Sea Level,
    // else it is with respect to local land height.
    double velocity; // meters per second along the heading vector
    double time;        // ultimately will hold time, for now a place holder
    double vectorIncrement[3]; // change in position in meters for short increments
} SIXDOF;

#define SIXDOF_TYPE
#endif

#include "RotationToolKit.h"
#include "consts.h"
//#include "boost/shared_ptr.hpp"
//#include <QMatrix4x4>
#include "LoggableObject.h"
#include "RIMatrixMath.hpp"

class RotationTK : RotationToolKit, public LoggableConfigurableObject
{
private:
    virtual void SetUserSettings();
public:
    int MatrixVectorMult(double *v, double *m, double *r);
    int MatrixMultiply(double * First, double * Second, double * Result, int nRowsFirst, int mColsFirst, int nRowsSecond, int mColsSecond);
    void RotateZ(double *m, double angle);
    void RotateY(double *m, double angle);
    void CopyMatrix(double *from, double *to);
    void RotateX(double *m, double angle);
    void MakeIdentity(double *m);
    void Transpose(double * in, double * out);
    // compute the composite angles with h and v offset angles from the centerline of the sensor
    virtual void GetCompositeAngles(sixdof *sdf, sixdof *cdf, sixdof *composite);
    virtual void Uncomposite(sixdof& sdf, sixdof& los, sixdof& azel);
    virtual void Uncomposite(double p, double h, double r, double losP, double losH, double losR, double& az, double& el);
    virtual void GetTurretAngles(sixdof *sdf, sixdof *cdf, sixdof *turret);
    //
    RotationTK(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger);
    virtual ~RotationTK();
    
    double r[16],rr[16];
    
    virtual void GetCompositeAngles(sixdof * sdf, sixdof * cdf, sixdof * composite, double vang, double hang);
    
    virtual void GetLOSAngles(double vp, double vh, double vr, // vehicle angles
                              double cp, double ch,double cr,  // camera angles
                              double *p, double *h, double *r); // LOS angles
    int GetScreenAngles4Geo(double px, double py, double pz, // point position vector relative to camera
                            double vp, double vh, double vr, // vehicle angles
                            double cp, double ch,double cr,  // camera angles
                            double *p, double *h,
                            double *distance); // camera screen angles)
    int GetPixels4Geo(double px, double py, double pz, // point position vector relative to camera
                      double vp, double vh, double vr, // vehicle angles
                      double cp, double ch,double cr,  // camera angles
                      double ifov, int nx, int ny,
                      double *sx, double *sy,
                      double *distance); // camera screen angles)
    int Pixels4Geo(double c[], double p[], // camera cartesian and point cartesian
                   double cp, double ch,double cr,  // composite camera angles
                   double ifov, int nx, int ny, // instantaneous fov, screen pixel counts
                   double *sx, double *sy,       // pixel indices of the point on the screen
                   double *distance); // camera screen angles)
    int GetPixelFromAngles(int nx, int ny, double hfov, double vfov ,
                           double ha, double va, double *x, double *y);
    int GetAnglesFromPixels(int nx, int ny, double hfov, double vfov ,
                            double x, double y, double *ha, double *va);
    
    // set the camera location
    void SetCamera(double camcart[],double pitch, double heading, double roll);
    // set the fields of view
    void SetFOVs(double hFov, double vFov);
    // set the fovs using instantaneous fov - assumes square pixels
    void SetIFOV(double ifov);
    // set the size of the window
    void ResizeWindow(int nx, int ny);
    // combine matrices to make efficient single transformation matrix
    // call this after all the Set and Resize functions have been called with the right values
    // and you are ready to start transforming points.
    void LoadMatrices();
    // get the pixel location from cartesian world point
    RIVector4 GetPixels4Cart(double cart[]);
    
    // these matrices are retained for re-use
    //QMatrix4x4 viewM;   // world to camera transformation matrix
     RIMatrix4x4 viewM;   // world to camera transformation matrix
    //QMatrix4x4 persM;   // perspective transformation matrix
    RIMatrix4x4 persM;
    //QMatrix4x4 mvpM;    // combined perspective * camera > matrix
    RIMatrix4x4 mvpM;
    //QMatrix4x4 imvpM;    // INVERSE perspective * camera > matrix
    RIMatrix4x4 imvpM;
    //QMatrix4x4 viewpM;  // viewport matrix
    RIMatrix4x4 viewpM;
    double hFOV, vFOV, iFOV,aspectRatio;
    int sx,sy;
    double location[3];
    double angles[3]; // pitch, heading, roll
    
    
};





//// RotationTK.h: interface for the RotationTK class.
////
////////////////////////////////////////////////////////////////////////
//// Copyright 1996-2015 MICHAEL F. ABERNATHY all rights reserved
/////////////////////////////////////////////////////////////////////////

//#pragma once
//#ifndef SIXDOF_TYPE

//typedef struct sixdof {
//    double latitude, longitude; // position in degrees of graticule
//    double pitch;    // angle of nose w/r/t horizon measured upward positive
//    double heading; // angle of nose w/r/t geographic north
//    double roll;    // angle of rotation about the line of sight, clockwise positive from the vertical
//    double altitude; // altitude in meters
//    int AGL;        //  if AGL is 0, altitude is Above Mean Sea Level,
//                    // else it is with respect to local land height.
//    double velocity; // meters per second along the heading vector
//    double time;        // ultimately will hold time, for now a place holder
//    double vectorIncrement[3]; // change in position in meters for short increments
//} SIXDOF;

//#define SIXDOF_TYPE
//#endif

//#include "RotationToolKit.h"
//#include "consts.h"
////#include "boost/shared_ptr.hpp"

//class RotationTK : RotationToolKit
//{
//public:
//    int MatrixVectorMult(double *v, double *m, double *r);
//    int MatrixMultiply(double * First, double * Second, double * Result, int nRowsFirst, int mColsFirst, int nRowsSecond, int mColsSecond);
//    void RotateZ(double *m, double angle);
//    void RotateY(double *m, double angle);
//    void CopyMatrix(double *from, double *to);
//    void RotateX(double *m, double angle);
//    void MakeIdentity(double *m);
//    void Transpose(double * in, double * out);
//    // compute the composite angles with h and v offset angles from the centerline of the sensor
//    virtual void GetCompositeAngles(sixdof *sdf, sixdof *cdf, sixdof *composite);
//    virtual void Uncomposite(sixdof *sdf, sixdof * los, sixdof * azel);
//    virtual void Uncomposite(double p, double h, double r, double losP, double losH, double losR, double * az, double * el);
//    virtual void GetTurretAngles(sixdof *sdf, sixdof *cdf, sixdof *turret);
//    //
//    RotationTK();
//    virtual ~RotationTK();

//    double r[16],rr[16];

//    virtual void GetCompositeAngles(sixdof * sdf, sixdof * cdf, sixdof * composite, double vang, double hang);

//    virtual void GetLOSAngles(double vp, double vh, double vr, // vehicle angles
//                                    double cp, double ch,double cr,  // camera angles
//                                    double *p, double *h, double *r); // LOS angles
//    int GetScreenAngles4Geo(double px, double py, double pz, // point position vector relative to camera
//                                        double vp, double vh, double vr, // vehicle angles
//                                        double cp, double ch,double cr,  // camera angles
//                                        double *p, double *h,
//                                        double *distance); // camera screen angles)
//    int GetPixels4Geo(double px, double py, double pz, // point position vector relative to camera
//                                        double vp, double vh, double vr, // vehicle angles
//                                        double cp, double ch,double cr,  // camera angles
//                                        double ifov, int nx, int ny,
//                                        double *sx, double *sy,
//                                        double *distance); // camera screen angles)
//    int Pixels4Geo(double c[], double p[], // camera cartesian and point cartesian
//                      double cp, double ch,double cr,  // composite camera angles
//                      double ifov, int nx, int ny, // instantaneous fov, screen pixel counts
//                      double *sx, double *sy,       // pixel indices of the point on the screen
//                      double *distance); // camera screen angles)
//    int GetPixelFromAngles(int nx, int ny, double hfov, double vfov ,
//                      double ha, double va, double *x, double *y);
//    int GetAnglesFromPixels(int nx, int ny, double hfov, double vfov ,
//                      double x, double y, double *ha, double *va);

//};

