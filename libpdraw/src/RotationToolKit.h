//////////////////////////////////////////////////////////////////////
// Copyright 1996-2015 MICHAEL F. ABERNATHY all rights reserved
///////////////////////////////////////////////////////////////////////
// this class wraps the RotationTK and exposes just the interface members of interest.
#pragma once

class RotationToolKit {
public:
    // compute the composite angles with h and v offset angles from the centerline of the sensor
    virtual void Uncomposite(double p, double h, double r, double losP, double losH, double losR, double& az, double& el)=0;
    virtual void GetLOSAngles(double vp, double vh, double vr, // vehicle angles
                              double cp, double ch,double cr,  // camera angles
                              double *p, double *h, double *r)=0; // LOS angles
    int Pixels4Geo(double c[], double p[], // camera cartesian and point cartesian
                   double cp, double ch,double cr,  // composite camera angles
                   double ifov, int nx, int ny, // instantaneous fov, screen pixel counts
                   double *sx, double *sy,       // pixel indices of the point on the screen
                   double *distance); // camera screen angles)
    
};

