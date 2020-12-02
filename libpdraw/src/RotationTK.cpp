// RotationTK.cpp: implementation of the RotationTK class.
//
//////////////////////////////////////////////////////////////////////
// Copyright 2015 Michael F. Abernathy all rights reserved
//////////////////////////////////////////////////////////////////////

#include "RotationTK.h"
#include <math.h>
#include <stdio.h>
//#if defined(__APPLE__)
#include "rimatrixmath.hpp"
//#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define nullptr 0

void RotationTK::SetUserSettings()
{

}

RotationTK::RotationTK(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger)
{
    LoggableConfigurableObject::Init(usettingsData, logger, "RotationTK");
    viewM.SetToIdentity();   // world to camera transformation matrix
    persM.SetToIdentity();   // perspective transformation matrix
    mvpM.SetToIdentity();    // combined perspective * camera > matrix
    imvpM.SetToIdentity();    // INVERSE perspective * camera > matrix
    viewpM.SetToIdentity();  // viewport matrix

    sx = 1280;
    sy = 720;
    vFOV = 30.0;
    hFOV = 16.0 * vFOV / 9.0;
    
}

RotationTK::~RotationTK()
{
    
}

void RotationTK::CopyMatrix(double *f, double *m)
{
    if (m == nullptr) return;
    if (f == nullptr) return;
    m[0] = f[0];        m[4] = f[4];    m[8] = f[8];        m[12] = f[12];
    m[1] = f[1];        m[5] = f[5];    m[9] = f[9];        m[13] = f[13];
    m[2] = f[2];        m[6] = f[6];    m[10] = f[10];        m[14] = f[14];
    m[3] = f[3];        m[7] = f[7];    m[11] = f[11];        m[15] = f[15];
}

void RotationTK::MakeIdentity(double *m)
{
    if (m == NULL) return;
    m[0] = 1.0;        m[4] = 0.0;    m[8] = 0.0;        m[12] = 0.0;
    m[1] = 0.0;        m[5] = 1.0;    m[9] = 0.0;        m[13] = 0.0;
    m[2] = 0.0;        m[6] = 0.0;    m[10] = 1.0;    m[14] = 0.0;
    m[3] = 0.0;        m[7] = 0.0;    m[11] = 0.0;    m[15] = 1.0;
}

void RotationTK::RotateX(double *m, double angle) // Pitch (1,0,0)
{
    if (m == NULL) return;
    angle *= PI180;
    double si = sin(angle);
    double co = cos(angle);
    
    r[0] = 1.0;        r[4] = 0.0;    r[8] = 0.0;        r[12] = 0.0;
    r[1] = 0.0;        r[5] = co;    r[9] = -si;        r[13] = 0.0;
    r[2] = 0.0;        r[6] = si;    r[10] = co;        r[14] = 0.0;
    r[3] = 0.0;        r[7] = 0.0;    r[11] = 0.0;    r[15] = 1.0;
    MatrixMultiply(r,m,rr,4,4,4,4);
    CopyMatrix(rr,m);
}

void RotationTK::RotateY(double *m, double angle) // Heading (0,1,0)
{
    if (m == NULL) return;
    angle *= PI180;
    double si = sin(angle);
    double co = cos(angle);
    
    r[0] = co;        r[4] = 0.0;    r[8] = si;        r[12] = 0.0;
    r[1] = 0.0;        r[5] = 1.0;    r[9] = 0.0;        r[13] = 0.0;
    r[2] = -si;        r[6] = 0.0;    r[10] = co;        r[14] = 0.0;
    r[3] = 0.0;        r[7] = 0.0;    r[11] = 0.0;    r[15] = 1.0;
    
    MatrixMultiply(r,m,rr,4,4,4,4);
    CopyMatrix(rr,m);
}

void RotationTK::RotateZ(double *m, double angle) // Roll (0,0,1)
{
    if (m == NULL) return;
    angle *= PI180;
    double si = sin(angle);
    double co = cos(angle);
    
    r[0] = co;        r[4] = -si;    r[8] = 0.0;        r[12] = 0.0;
    r[1] = si;        r[5] = co;    r[9] = 0.0;        r[13] = 0.0;
    r[2] = 0.0;        r[6] = 0.0;    r[10] = 1.0;    r[14] = 0.0;
    r[3] = 0.0;        r[7] = 0.0;    r[11] = 0.0;    r[15] = 1.0;
    MatrixMultiply(r,m,rr,4,4,4,4);
    CopyMatrix(rr,m);
}

// compute the composite angles with h and v offset angles from the centerline of the sensor
void RotationTK::GetCompositeAngles(sixdof *sdf, sixdof *cdf, sixdof *composite)
{
    double hyp,mvmat[16];
    double befo[4],after[4],dv;
    befo[0] = 0.0;
    befo[1] = 0.0;
    befo[2] = 1.0;
    befo[3] = 1.0;
    MakeIdentity(mvmat);
    RotateY(mvmat,sdf->heading);    //, 0.0, 1.0, 0.0);
    RotateX(mvmat, -(sdf->pitch));  //, 1.0, 0.0, 0.0);
    RotateZ(mvmat, - sdf->roll);    //, 0.0, 0.0, 1.0);
    // Correct for boresight
    RotateY(mvmat, cdf->heading);    //, 0.0, 1.0, 0.0);
    RotateX(mvmat, -(cdf->pitch));    //, 1.0, 0.0, 0.0);
    RotateZ(mvmat, cdf->roll);        //, 0.0, 0.0, 1.0);
    
    MatrixVectorMult(befo,mvmat,after);
    
    hyp = after[0] * after[0] + after[1] * after[1] + after[2] * after[2];
    if (hyp > 0.0) {
        hyp = sqrt(hyp);
        after[0] /= hyp;
        after[1] /= hyp;
        after[2] /= hyp;
    }
    dv = sqrt(after[0]*after[0] + after[2]*after[2]);
    composite->AGL = sdf->AGL;
    composite->latitude = sdf->latitude;
    composite->longitude = sdf->longitude;
    composite->altitude = sdf->altitude;
    composite->vectorIncrement[0] = after[0];
    composite->vectorIncrement[1] = after[1];
    composite->vectorIncrement[2] = after[2];
    composite->pitch = atan2(after[1],dv)/PI180;
    composite->heading = atan2(after[0],after[2])/PI180;
    // Here we add the part to compute the roll angle
    befo[0] = 1.0;  // make a vector out the side of the camera
    befo[1] = 0.0;
    befo[2] = 0.0;
    befo[3] = 1.0;
    MatrixVectorMult(befo,mvmat,after);
    hyp = after[0] * after[0] + after[1] * after[1] + after[2] * after[2];
    if (hyp > 0.0) {
        hyp = sqrt(hyp);
        after[0] /= hyp;
        after[1] /= hyp;
        after[2] /= hyp;
    }
    dv = sqrt(after[0]*after[0] + after[2]*after[2]);
    composite->roll = atan2(after[1],dv)/PI180;
    
}
void RotationTK::Uncomposite(sixdof& sdf, sixdof& los, sixdof&  azEl)
{
    double hyp,mvmat[16],mlos[16], result[16];
    double befo[4],after[4],dv;
    befo[0] = 0.0;
    befo[1] = 0.0;
    befo[2] = 1.0;
    befo[3] = 1.0;
    MakeIdentity(mvmat);
    RotateY(mvmat,sdf.heading);    //, 0.0, 1.0, 0.0);
    RotateX(mvmat, -(sdf.pitch));  //, 1.0, 0.0, 0.0);
    RotateZ(mvmat, - (sdf.roll));    //, 0.0, 0.0, 1.0);
    
    double mvmatT[16];
    Transpose(mvmat,mvmatT);
    
    MakeIdentity(mlos);
    RotateY(mlos,los.heading);
    RotateX(mlos,-(los.pitch));
    RotateZ(mlos,-(los.roll));
    
    MatrixMultiply(mlos,mvmatT,result,4,4,4,4);
    
    
    MatrixVectorMult(befo,result,after);
    
    hyp = after[0] * after[0] + after[1] * after[1] + after[2] * after[2];
    if (hyp > 0.0) {
        hyp = sqrt(hyp);
        after[0] /= hyp;
        after[1] /= hyp;
        after[2] /= hyp;
    }
    dv = sqrt(after[0]*after[0] + after[2]*after[2]);
    azEl.heading = atan2(after[0],after[2])/PI180;
    azEl.pitch = atan2(after[1],dv)/PI180;
}


void RotationTK::Uncomposite(double p, double h, double r, double losP, double losH, double losR, double& az, double& el)
{
    SIXDOF sdf;
    SIXDOF los;
    SIXDOF azEl;
    
    sdf.pitch = p;
    sdf.heading = h;
    sdf.roll = r;
    
    los.pitch = losP;
    los.heading = losH;
    los.roll = losR;
    
    Uncomposite(sdf,los,azEl);
    az = azEl.heading;
    el = azEl.pitch;
}


void RotationTK::GetTurretAngles(sixdof *sdf, sixdof *cdf, sixdof *turret)
{
    double hyp,mvmat[16];
    double after[4],dv;
    //vcomp;
    double befo[4];
    hyp =      cos(cdf->pitch * PI180);
    befo[0] = hyp * sin(cdf->heading * PI180);
    befo[1] = sin(cdf->pitch * PI180);
    befo[2] = hyp * cos(cdf->heading * PI180);
    befo[3] = 1.0;
    MakeIdentity(mvmat);
    RotateZ(mvmat,  sdf->roll);    //, 0.0, 0.0, 1.0);
    RotateX(mvmat, (sdf->pitch));  //, 1.0, 0.0, 0.0);
    RotateY(mvmat,-sdf->heading);    //, 0.0, 1.0, 0.0);
    
    MatrixVectorMult(befo,mvmat,after);
    
    hyp = after[0] * after[0] + after[1] * after[1] + after[2] * after[2];
    if (hyp > 0.0) {
        hyp = sqrt(hyp);
        after[0] /= hyp;
        after[1] /= hyp;
        after[2] /= hyp;
    }
    dv = sqrt(after[0]*after[0] + after[2]*after[2]);
    turret->AGL = sdf->AGL;
    turret->latitude = sdf->latitude;
    turret->longitude = sdf->longitude;
    turret->altitude = sdf->altitude;
    turret->vectorIncrement[0] = after[0];
    turret->vectorIncrement[1] = after[1];
    turret->vectorIncrement[2] = after[2];
    turret->pitch = atan2(after[1],dv)/PI180;
    turret->heading = atan2(after[0],after[2])/PI180;
}


/////////////////////////////
// Routine to multiply two matrices
//
// Note:  inner dimension of product must match
//        that is, can multiply 4X3 matrix times 3X1 or 3X2 or 3X3 or ...
//        resulting matrix  has outer values as dimensions
//        e.g. 4X3 times 3X1 will be 4X1
////////////////////////////////
int RotationTK::MatrixMultiply(double *First, double *Second, double *Result,
                               int nRowsFirst, int mColsFirst,
                               int nRowsSecond, int mColsSecond)
{
    int Status = -2;
    // Check for valid product dimensions.
    if( nRowsFirst < 1 || mColsFirst < 1 || nRowsSecond < 1 || mColsSecond < 1 )
        return Status;
    Status = -1;
    if( mColsFirst != nRowsSecond )
        return Status;
    // Got valid dimensions, so proceed.
    double *pA = First;
    double *pB;
    double *p_B;
    double *pC = Result;
    int i,j,k;
    int nrows = nRowsFirst;
    int ncols = mColsFirst;
    int mcols = mColsSecond;
    
    for (i = 0; i < nrows; First += ncols, i++)
        for (p_B = Second, j = 0; j < mcols; Result++, p_B++, j++){
            pB = p_B;
            pA = First;
            *Result = 0.0;
            for (k = 0; k < ncols; pA++, pB += mcols, k++)
                *Result += *pA * *pB;
        }
    
    Status = 0;
    return Status;
}

int RotationTK::MatrixVectorMult(double *v, double *m, double *r)
{
    r[0] = v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + v[3] * m[12];
    r[1] = v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + v[3] * m[13];
    r[2] = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + v[3] * m[14];
    r[3] = v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + v[3] * m[15];
    return 1;
    
}

void RotationTK::Transpose(double * in, double * out)
{
    //Diagonal is unchanged
    out[0] = in[0];
    out[5] = in[5];
    out[10] = in[10];
    out[15] = in[15];
    
    out[1] = in[4];
    out[4] = in[1];
    
    out[2] = in[8];
    out[8] = in[2];
    
    out[3] = in[12];
    out[12] = in[3];
    
    out[6] = in[9];
    out[9] = in[6];
    
    out[7]=in[13];
    out[13]=in[7];
    
    out[11] = in[14];
    out[14] = in[11];
    
}
void RotationTK::GetCompositeAngles(sixdof * sdf, sixdof * cdf, sixdof * composite, double vang, double hang)
{
    double hyp,mvmat[16];
    double befo[4],after[4],dv;
    befo[0] = 0.0;
    befo[1] = 0.0;
    befo[2] = 1.0;
    befo[3] = 1.0;
    MakeIdentity(mvmat);
    RotateY(mvmat,sdf->heading);    //, 0.0, 1.0, 0.0);
    RotateX(mvmat, -(sdf->pitch));  //, 1.0, 0.0, 0.0);
    RotateZ(mvmat, - sdf->roll);    //, 0.0, 0.0, 1.0);
    // Correct for boresight
    RotateY(mvmat, cdf->heading);    //, 0.0, 1.0, 0.0);
    RotateX(mvmat, -(cdf->pitch+vang));    //, 1.0, 0.0, 0.0);
    // this is our extra rotation for horizontal angle offset
    RotateY(mvmat, hang);    //, 1.0, 0.0, 0.0);
    
    RotateZ(mvmat, cdf->roll);        //, 0.0, 0.0, 1.0);
    
    MatrixVectorMult(befo,mvmat,after);
    
    hyp = after[0] * after[0] + after[1] * after[1] + after[2] * after[2];
    if (hyp > 0.0) {
        hyp = sqrt(hyp);
        after[0] /= hyp;
        after[1] /= hyp;
        after[2] /= hyp;
    }
    dv = sqrt(after[0]*after[0] + after[2]*after[2]);
    composite->AGL = sdf->AGL;
    composite->latitude = sdf->latitude;
    composite->longitude = sdf->longitude;
    composite->altitude = sdf->altitude;
    composite->vectorIncrement[0] = after[0];
    composite->vectorIncrement[1] = after[1];
    composite->vectorIncrement[2] = after[2];
    composite->pitch = atan2(after[1],dv)/PI180;
    composite->heading = atan2(after[0],after[2])/PI180;
    // Here we add the part to compute the roll angle
    befo[0] = 1.0;  // make a vector out the side of the camera
    befo[1] = 0.0;
    befo[2] = 0.0;
    befo[3] = 1.0;
    MatrixVectorMult(befo,mvmat,after);
    hyp = after[0] * after[0] + after[1] * after[1] + after[2] * after[2];
    if (hyp > 0.0) {
        hyp = sqrt(hyp);
        after[0] /= hyp;
        after[1] /= hyp;
        after[2] /= hyp;
    }
    dv = sqrt(after[0]*after[0] + after[2]*after[2]);
    composite->roll = atan2(after[1],dv)/PI180;
}

void RotationTK::GetLOSAngles(double vp, double vh, double vr, // vehicle angles
                              double cp, double ch,double cr,  // camera angles
                              double *p, double *h, double *r) // LOS angles
{//(sixdof *sdf, sixdof *cdf, sixdof *composite)
    double hyp,mvmat[16];
    double befo[4],after[4],dv;
    befo[0] = 0.0;
    befo[1] = 0.0;
    befo[2] = 1.0;
    befo[3] = 1.0;
    MakeIdentity(mvmat);
    RotateY(mvmat,vh);    //, 0.0, 1.0, 0.0);
    RotateX(mvmat, - vp);  //, 1.0, 0.0, 0.0);
    RotateZ(mvmat, - vr);    //, 0.0, 0.0, 1.0);
    // Correct for boresight
    RotateY(mvmat, ch);    //, 0.0, 1.0, 0.0);
    RotateX(mvmat, -(cp));    //, 1.0, 0.0, 0.0);
    RotateZ(mvmat, cr);        //, 0.0, 0.0, 1.0);
    
    MatrixVectorMult(befo,mvmat,after);
    
    hyp = after[0] * after[0] + after[1] * after[1] + after[2] * after[2];
    if (hyp > 0.0) {
        hyp = sqrt(hyp);
        after[0] /= hyp;
        after[1] /= hyp;
        after[2] /= hyp;
    }
    dv = sqrt(after[0]*after[0] + after[2]*after[2]);
    
    *p = atan2(after[1],dv)/PI180;
    *h = atan2(after[0],after[2])/PI180;
    // Here we add the part to compute the roll angle
    befo[0] = 1.0;  // make a vector out the side of the camera
    befo[1] = 0.0;
    befo[2] = 0.0;
    befo[3] = 1.0;
    MatrixVectorMult(befo,mvmat,after);
    hyp = after[0] * after[0] + after[1] * after[1] + after[2] * after[2];
    if (hyp > 0.0) {
        hyp = sqrt(hyp);
        after[0] /= hyp;
        after[1] /= hyp;
        after[2] /= hyp;
    }
    dv = sqrt(after[0]*after[0] + after[2]*after[2]);
    *r = atan2(after[1],dv)/PI180;
    
}

int RotationTK::GetScreenAngles4Geo(double px, double py, double pz, // point position vector relative to camera
                                    double vp, double vh, double vr, // vehicle angles
                                    double cp, double ch,double cr,  // camera angles
                                    double *p, double *h,
                                    double *distance) // camera screen angles)
{
    double hyp,mvmat[16];
    double befo[4],after[4],dv,D;
    // compute the distance to the point from the camera
    D = px*px + py*py + pz*pz;
    *distance = D;
    if (D != 0.0)
        *distance = sqrt(D);
    // now we transform the point of interest into camera screen angles
    befo[0] = px;
    befo[1] = py;
    befo[2] = pz;
    befo[3] = 1.0;
    // compose a rotation matrix for all the rotations that impact this transformation
    MakeIdentity(mvmat);
    
    // camera angles to vehicle
    RotateZ(mvmat, -cr);        //, 0.0, 0.0, 1.0); 1
    RotateX(mvmat, (cp));    //, 1.0, 0.0, 0.0);
    RotateY(mvmat, -ch);    //, 0.0, 1.0, 0.0);
    //vehicle angles
    //    RotateY(mvmat, ch);    //, 0.0, 1.0, 0.0);
    //    RotateX(mvmat, - vp);  //, 1.0, 0.0, 0.0);
    //    RotateY(mvmat,vh);    //, 0.0, 1.0, 0.0);
    
    MatrixVectorMult(befo,mvmat,after);
    //printf("after %lf %lf %lf\n",after[0],after[1],after[2]);
    
    hyp = after[0] * after[0] + after[1] * after[1] + after[2] * after[2];
    if (hyp > 0.0) {
        hyp = sqrt(hyp);
        after[0] /= hyp;
        after[1] /= hyp;
        after[2] /= hyp;
    }
    dv = sqrt(after[0]*after[0] + after[2]*after[2]);
    //printf("d %f dv %f   after %lf %lf %lf\n",hyp,dv, after[0],after[1],after[2]);
    
    *p = atan2(after[1],dv)/PI180;
    *h = atan2(after[0],after[2])/PI180;
    
    return 1;
}

int RotationTK::GetPixels4Geo(double px, double py, double pz, // point position vector relative to camera
                              double vp, double vh, double vr, // vehicle angles
                              double cp, double ch,double cr,  // camera angles
                              double ifov, int nx, int ny, // instaneous field of view, x and y pixel counts
                              double *sx, double *sy,
                              double *distance) // camera screen angles)
{
    // accepts a relative position vector in cartesian coordinates, (subtract camera position[3] from point of interest[3] to get this vector[3])
    // uses all camera and vehicle angles and
    // the horizontal and vertical fovs of the screen, as well as the number of x and y pixels
    // if point is in view of camera returns one, and fills the screen coordinates (sx, and sy) with the screen coordinates
    // distance contains the distance to the pixel.
    // returns zero if point is not visible to the camera
    
    // local pitch and heading in screen space
    double p,h;
    GetScreenAngles4Geo(px, py, pz, // point position vector relative to camera
                        vp, vh, vr, // vehicle angles
                        cp, ch,cr,  // camera angles
                        &p, &h,
                        distance);
    /*
     double hv,hh;
     hv = ((double) ny) * 0.5;
     hh = ((double) nx) * 0.5;
     *sx = h / ifov + hh;
     *sy = p / ifov + hv;
     */
    GetPixelFromAngles(nx,ny,nx*ifov,ny*ifov,h,p,sx,sy);
    if (*sx < 0) return 0;
    if (*sy < 0) return 0;
    if (*sx > nx) return 0;
    if (*sy > ny) return 0;
    return 1;
}

int RotationTK::Pixels4Geo(double c[], double p[], // camera cartesian and point cartesian
                           double cp, double ch,double cr,  // composite camera angles
                           double ifov, int nx, int ny, // instantaneous fov, screen pixel counts
                           double *sx, double *sy,       // pixel indices of the point on the screen
                           double *distance) // camera screen angles)
{
    double pp[3];
    pp[0] = p[0] - c[0];
    pp[1] = p[1] - c[1];
    pp[2] = p[2] - c[2];
    
    return GetPixels4Geo(pp[0],pp[2],pp[1],
                         0,0,0,
                         cp,ch,cr,
                         ifov,nx,ny,
                         sx,sy,distance);
}

int RotationTK::GetPixelFromAngles(int nx, int ny, double hfov, double vfov ,
                                   double ha, double va, double *x, double *y) {
    // nx, ny are pixel dimensions of screen
    // hfov, vfov are the fields of view in degrees
    // ha, va are the horizontal and vertical angles to be converted to pixesl
    // x and y are the pixels calculated
    double hs = cos(0.5*fabs(ha)*PI180);
    double vs = cos(0.5*fabs(ha)*PI180);
    hs *= cos(0.5*fabs(va)*PI180);
    vs *= cos(0.5*fabs(va)*PI180);
    //hs = 1.0;
    //vs = 1.0;
    // the fields of view are a little larger porportional to how far off axis they are
    vfov *= vs;
    hfov *= hs;
    
    //    vfov *= 0.99;
    //    hfov *= 0.99;
    double hh = 0.5 * nx;
    double hv = 0.5 * ny;
    //
    // convert from angles to pixels
    *x = (ha/(0.5 * hfov)) * hh + hh;
    *y = (va/(0.5 * vfov)) * hv + hv;
    return 1;
}

int RotationTK::GetAnglesFromPixels(int nx, int ny, double hfov, double vfov ,
                                    double x, double y, double *ha, double *va) {
    double H,V;
    double hh = 0.5 * nx;
    double hv = 0.5 * ny;
    // Calculate the simple angles first
    H = (x - hh) / hh;
    V = (y - hv) / hv;
    H *= hfov;
    V *= vfov;
    double hs = cos(0.5*fabs(V)*PI180);
    double vs = cos(0.5*fabs(H)*PI180);
    *ha = H * hs;
    *va = V * vs;
    return 1;
}

//TODO: DIFFERS FROM SCJET IN QT MATRICES--START POINT
// (re) set the camera location
void RotationTK::SetCamera(double *camcart,double pitch, double heading, double roll){
//    viewM.setToIdentity();
//
//    viewM.rotate(roll, 0.0, 0.0, 1.0);
//    viewM.rotate(pitch, 1.0, 0.0, 0.0);
//    viewM.rotate(heading, 0.0, 1.0, 0.0);
//
//    viewM.rotate(-90.0, 1.0, 0.0, 0.0);
//    viewM.translate(-camcart[0],-camcart[1],-camcart[2]);
    viewM = viewM.PerformRotateAboutZ((float)roll);// vmath::rotate((float)roll, (float)0.0, (float)0.0, (float)1.0) *
    viewM = viewM.PerformRotateAboutX((float)pitch); //vmath::rotate((float)pitch, (float)1.0, (float)0.0, (float)0.0) *
    viewM = viewM.PerformRotateAboutY((float)heading);// vmath::rotate((float)heading, (float)0.0, (float)1.0, (float)0.0) *
    viewM = viewM.PerformRotateAboutX((float)-90.0);// vmath::rotate((float)-90.0, (float)1.0, (float)0.0, (float)0.0) *
        viewM = viewM.PerformTranslate((float)-camcart[0],(float)-camcart[1],(float)-camcart[2]);// vmath::translate((float)-camcart[0],(float)-camcart[1],(float)-camcart[2]);
    
    LoadMatrices();
}

// set the fields of view
void RotationTK::SetFOVs(double hFov, double vFov){
    //persM.setToIdentity();
    persM.SetToIdentity();
    hFOV = hFov;
    vFOV = vFov;
    //iFOV = hFov / sx;
    aspectRatio = hFOV / vFOV;
    //TODO: DIFFERS FROM SCJET IN QT MATRICES
    //persM = vmath::perspective(vFOV, aspectRatio, 1, 1000000.0);
    persM.SetToPerspectiveMatrix(vFOV, aspectRatio, 1, 1000000.0);
    //persM.perspective(vFOV, aspectRatio,  1, 1000000.0);
    LoadMatrices();
}

// set the fovs using instantaneous fov - assumes square pixels
void RotationTK::SetIFOV(double ifov){
    //persM.setToIdentity();
    persM.SetToIdentity();
    iFOV = ifov;
    hFOV = ifov * sx;
    vFOV = ifov * sy;
    aspectRatio = hFOV / vFOV;
    //TODO: DIFFERS FROM SCJET IN QT MATRICES
    persM.SetToPerspectiveMatrix(vFOV, aspectRatio, 1, 10000000.0);
    //persM = vmath::perspective(vFOV, aspectRatio, 1, 10000000.0);
    //persM.perspective(vFOV, aspectRatio,  1, 1000000.0);
    LoadMatrices();
}

// set the size of the window
void RotationTK::ResizeWindow(int nx, int ny){
    //viewpM.setToIdentity();
    viewpM.SetToIdentity();
    //TODO: DIFFERS FROM SCJET IN QT MATRICES
    viewpM.SetToOrthographicMatrix(0.0, nx, 0.0, ny, 1, 1000000.0);
    //viewpM = vmath::ortho(0.0, 0.0, nx, ny, 1, 1000000.0);//is this the same??
    //viewpM.viewport(0, 0, nx, ny, 1, 1000000.0);//original only line
    
    sx = nx;
    sy = ny;
}

void RotationTK::LoadMatrices() {
    mvpM = persM * viewM;
    bool inv=true;//don't think we need this or any inverse matrix
    //imvpM = mvpM.inverted(&inv);
    //TODO: DIFFERS FROM SCJET IN QT MATRICES
    //imvpM = mvpM.inverted(&inv);
    if (!inv)
        printf("Non invertable view matrix\n");
}

//TODO: DIFFERS FROM SCJET IN QT MATRICES
RIVector4 RotationTK::GetPixels4Cart(double cart[]){
    RIVector4 vertex(cart[0], cart[1], cart[2], 1.0);
//    QVector4D vertex;
//    vertex.setW(1.0);
//    vertex.setX(cart[0]);
//    vertex.setY(cart[1]);
//    vertex.setZ(cart[2]);
    // transform the point (vertex) into the camera coordiante frame including perspective
    //QVector4D xyzw = mvpM * vertex;
    
    RIVector4 xyzw = mvpM.MultiplyVector4(vertex);
    
    // Convert those to normalized device coords
    //QVector3D xyzwNTC = xyzw.toVector3DAffine();
    float x, y, z, w;
    xyzw.GetValues(x, y, z, w);
    RIVector3 xyzwNTC(x/w, y/w, z/w);//to vector 3d affine
    if (w < 0) {
        // if the w value in mvpM coordinates is less than zero, it means the object is behind us,
        // and therefore, not useful.
        //QVector4D dud;
        RIVector4 dud(-1, -1, 0.0, 0.0);
        //printf("screen values indicating point not visible in front.\n");
        //dud[0] = -1;
        //dud[1] = -1;
        return dud;
    }
    
    //  apply the viewport matrix to get the x and y pxel location
    //QVector4D xy = viewpM * xyzwNTC;
    xyzwNTC.GetValues(x, y, z);
    RIVector4 xyzwNTCx4(x, y, z, 1.0);
    RIVector4 xy = viewpM.MultiplyVector4(xyzwNTCx4);
    // back check
    
    return xy;
}
