//
//  RIMatrixMath.cpp
//  RIMatrixMath
//
//  Created by David Dvergsten on 2/5/19.
//  Copyright © 2019 Rapid Imaging Technologies. All rights reserved.
//
#define SQUARE(x) (x*(x))
#define PI 3.14159265
#include "RIMatrixMath.hpp"
#include <math.h>
#include <iostream>

using namespace std;
void RIRotationTK::LoadMatrices() {
    mvpM = persM * viewM;

    bool inv;


    //imvpM = mvpM.inverted(&inv);
    //if (!inv)
    //    printf("Non invertable view matrix\n");
}
void RIRotationTK::ResizeWindow(int nx, int ny){
    viewpM.SetToIdentity();
    viewpM.SetToViewport(0.0, 0.0, nx, ny, 1000000.0, 1.0);
    sx = nx;
    sy = ny;

    //viewpM.setToIdentity();
    //viewpM.viewport(0, 0, nx, ny, 1, 1000000.0);
    //sx = nx;
    //sy = ny;
}
void RIRotationTK::SetFOVs(double hFov, double vFov){
    persM.SetToIdentity();
    hFOV = hFov;
    vFOV = vFov;
    iFOV = hFov / (double)sx;
    aspectRatio = hFOV / vFOV;
    aspectRatio = static_cast<double>(sx) / static_cast<double>(sy);
    persM.SetToPerspectiveMatrix(vFOV, aspectRatio, 1, 1000000.0);
    LoadMatrices();

    //persM.setToIdentity();
    //hFOV = hFov;
    //vFOV = vFov;
    //iFOV = hFov / sx;
    //aspectRatio = hFOV / vFOV;
    //persM.perspective(vFOV, aspectRatio,  1, 1000000.0);
    //LoadMatrices();
}
void RIRotationTK::SetCamera(double *camcart,double pitch, double heading, double roll){
    //RIMatrix4x4 loc_viewM = viewM;
    viewM.SetToIdentity();
    viewM = viewM.PerformRotateAboutZ(roll);
    viewM = viewM.PerformRotateAboutX(pitch);
    viewM = viewM.PerformRotateAboutY(heading);
    viewM = viewM.PerformRotateAboutX(-90.0);
    viewM = viewM.PerformTranslate(-camcart[0],-camcart[1],-camcart[2]);
    viewM = viewM;
    LoadMatrices();

    //    viewM.setToIdentity();

    //    viewM.rotate(roll, 0.0, 0.0, 1.0);
    //    viewM.rotate(pitch, 1.0, 0.0, 0.0);
    //    viewM.rotate(heading, 0.0, 1.0, 0.0);

    //    viewM.rotate(-90.0, 1.0, 0.0, 0.0);
    //    viewM.translate(-camcart[0],-camcart[1],-camcart[2]);

    //    LoadMatrices();
}

void SetToViewport(float left, float bottom, float width, float height, float nearPlane, float farPlane)

{

}

RIVector4 RIRotationTK::GetPixels4Cart(double cart[])
{


    //QVector4D vertex;
    RIVector4 vertex(cart[0], cart[1], cart[2], 1.0);
    //    vertex.setW(1.0);
    //    vertex.setX(cart[0]);
    //    vertex.setY(cart[1]);
    //    vertex.setZ(cart[2]);
    // transform the point (vertex) into the camera coordiante frame including perspective

    RIVector4 xyzw = mvpM.MultiplyVector4(vertex);
    //QVector4D xyzw = mvpM * vertex;


    // Convert those to normalized device coords
    RIVector3 xyzwNTC = xyzw.ToVector3DAffine();
    //QVector3D xyzwNTC = xyzw.toVector3DAffine();

    float x, y, z, w;
    xyzw.GetValues(x, y, z, w);

    if (w < 0) {
        // if the w value in mvpM coordinates is less than zero, it means the object is behind us,
        // and therefore, not useful.
        RIVector4 dud(-1.0, -1.0, 1.0, 1.0);
        //QVector4D dud;
        //printf("screen values indicating point not visible in front.\n");
        //dud[0] = -1;
        //dud[1] = -1;
        return dud;
    }

    //  apply the viewport matrix to get the x and y pxel location
    // QVector4D xy = viewpM * xyzwNTC;
    float x4,y4,z4;
    xyzwNTC.GetValues(x4,y4,z4);
//    if(x4 > -1 && x4 < 1.0 && y4 > -1 && y4 < 1.0)
//    {
//        std::cout<<"stop";
//    }
    RIVector4 xyzwNTCvec4(x4, y4, z4, 1.0);
    RIVector4 xy = viewpM.MultiplyVector4(xyzwNTCvec4);

    // back check

    return xy;


    //RIVector4 vec(1.0, 1.0, 1.0, 1.0);
    //return vec;
}


void RIMatrix4x4::SetToViewport(float left, float bottom, float width, float height, float nearPlane, float farPlane)
{
    float deltaPlane = farPlane - nearPlane;
    float sumPlane = farPlane + nearPlane;
    _matrix[0] = width/2.0f;         _matrix[4] = 0.0;          _matrix[8] = 0.0;                   _matrix[12] = left + (width/2.0f);
    _matrix[1] = 0.0;               _matrix[5] = height/2.0f;   _matrix[9] = 0.0;                   _matrix[13] = bottom + (height/2.0f);
    _matrix[2] = 0.0;               _matrix[6] = 0.0;           _matrix[10] = (deltaPlane/2.0f);    _matrix[14] = sumPlane/2.0f;
    _matrix[3] = 0.0;               _matrix[7] = 0.0;           _matrix[11] = 0.0;                  _matrix[15] = 1.0;
}

RIVector3 RIVector4::ToVector3DAffine()
{
    //take xyz and divide by w
    //create a vec3 from that and return it
    float x3;
    float y3;
    float z3;
    x3 = _vector[0] / _vector[3];
    y3 = _vector[1] / _vector[3];
    z3 = _vector[2] / _vector[3];
    RIVector3 retVector(x3, y3, z3);
    return retVector;
}

RIVector3 RIMatrix3x3::MultiplyVector3(RIVector3 vector3)
{
    float x = 0.00;
    float y = 0.00;
    float z = 0.00;
    vector3.GetValues(x, y, z);

    float resultx = _matrix[0] * x + _matrix[3] * y + _matrix[6] * z ;
    float resulty = _matrix[1] * x + _matrix[4] * y + _matrix[7] * z ;
    float resultz = _matrix[2] * x + _matrix[5] * y + _matrix[8] * z ;

    RIVector3 retVec3(resultx, resulty, resultz);//, resultw) ;
    return retVec3;
}
RIMatrix3x3::RIMatrix3x3()
{
    _matrix[0] = 1.0; _matrix[3] = 0.0; _matrix[6] = 0.0;
    _matrix[1] = 0.0; _matrix[4] = 1.0; _matrix[7] = 0.0;
    _matrix[2] = 0.0; _matrix[5] = 0.0; _matrix[8] = 1.0;
}

///We use this matrix to account for roll of camera, so we can set up 3 orhtogonal
///vectors located at the camera, aiming at starepoint, sideways vector perpendicular
///to los, upcamera perpendicular to camera in the up +z direction, but skewed to account
///for camera roll.
void RIMatrix3x3::SetRotateAboutAxisMatrix(RIVector3 axis, float degrees)
{
    float radians = degrees * PI / 180.0;
    RIMatrix4x4 rotMat;
    float C = cos(radians);
    float S = sin(radians);
    float Ax = 0.0;
    float Ay = 0.0;
    float Az = 0.0;
    axis.GetValues(Ax, Ay, Az);

    float m0 = C + SQUARE(Ax) * ( 1 - C);
    float m1 = Ax * Ay * (1 - C) + Az * S;
    float m2 = Ax * Az * (1 - C) - Ay * S;

    float m3 = Ax * Ay * (1 - C) - Az * S;
    float m4 = C + SQUARE(Ay) * (1 - C);
    float m5 = Ay * Az * (1 - C) + Ax * S;

    float m6 = Ax * Az * (1 - C) + Ay * S;
    float m7 = Ay * Az * (1 - C) - Ax * S;
    float m8 = C + SQUARE(Az) * (1 - C);

    _matrix[0] = m0;    _matrix[3] = m3;    _matrix[6] = m6;
    _matrix[1] = m1;    _matrix[4] = m4;    _matrix[7] = m7;
    _matrix[2] = m2;    _matrix[5] = m5;    _matrix[8] = m8;
}
void RIMatrix4x4::SetToOrthographicMatrix(float left, float right, float bottom, float top, float near, float far)
{
    _matrix[0] = 2.0/(right-left);  _matrix[4] = 0.0;               _matrix[8] = 0.0;               _matrix[12] = (left+right)/(left-right);
    _matrix[1] = 0.0;               _matrix[5] = 2.0/(top-bottom);  _matrix[9] = 0.0;               _matrix[13] = (bottom+top)/(bottom-top);
    _matrix[2] = 0.0;               _matrix[6] = 0.0;               _matrix[10] = 2.0/(near-far);   _matrix[14] = (near+far)/(far-near);
    _matrix[3] = 0.0;               _matrix[7] = 0.0;               _matrix[11] = 0.0;              _matrix[15] = 1.0;

}
void RIMatrix4x4::SetToPerspectiveMatrix(float vfovDegrees, float aspect, float near, float far)
{
    float radians = (vfovDegrees/2.0) * PI / 180.0 ;

    float sine = sin(radians);
    float cotan = cos(radians) / sine;

    float q = 1.0f / tan(0.5f * vfovDegrees);
    float A = q / aspect;
    float B = (near + far) / (near - far);
    float C = (2.0f * near * far) / (near - far);

    float clip = far - near;
    _matrix[0] = cotan / aspect;     _matrix[4] = 0.0;  _matrix[8] = 0.0;                   _matrix[12] = 0.0;
    _matrix[1] = 0.0;               _matrix[5] = cotan; _matrix[9] = 0.0;                   _matrix[13] = 0.0;
    _matrix[2] = 0.0;               _matrix[6] = 0.0;   _matrix[10] = -(near+far)/clip;     _matrix[14] = (-1)*(2.0f*near*far)/clip;
    _matrix[3] = 0.0;               _matrix[7] = 0.0;   _matrix[11] = -1.0;                 _matrix[15] = 0.0;

}
void RIMatrix4x4::SetToLookAtMatrix(float camx, float camy, float camz, float losPitch, float losYaw, float losRoll)
{
    //construct vector f = forward looking vector, use lospitch/heading using +z as up
    //x/y components can be calculated from yaw and pitch
    RIVector3 forwardVector(sin(losYaw), cos(losYaw), -1*(sin(losPitch)));//vector pointing in direction of line of sight (los)

    //construct the "up" vector which should be +z
    RIVector3 upVector(0.0, 0.0, 1.0);

    //take cross product of forward vector and up vector to get vector perpendicular to both "sideways vector"
    RIVector3 sidewaysVector = CrossProduct(forwardVector, upVector);

    //I think we need to now rotate by camera roll, so when we take cross product of sideways vector and forward vector
    //  we have vector pointing up perpendicular to camera.  This was not described in OpenglSuperBible but they
    //  didn't take roll into account
    RIMatrix3x3 mat3x3;
    mat3x3.SetRotateAboutAxisMatrix(forwardVector, losRoll);//todo: not sure if this is -roll or +roll
    RIVector3 sidewaysVecRollRotated = mat3x3.MultiplyVector3(sidewaysVector);//align perpendicular to camera after adding los roll
    /////end extra losroll calculation


    //construct new up vector that is perpendicular to the camera "ucam"
    RIVector3 upCamVectorRollRotated = CrossProduct(sidewaysVecRollRotated, forwardVector);
    float sx = 0.0;
    float sy = 0.0;
    float sz = 0.0;
    sidewaysVecRollRotated.GetValues(sx, sy, sz);

    float ucamx = 0.0;
    float ucamy = 0.0;
    float ucamz = 0.0;
    upCamVectorRollRotated.GetValues(ucamx, ucamy, ucamz);

    float fx = 0.00;
    float fy = 0.00;
    float fz = 0.00;
    forwardVector.GetValues(fx, fy, fz);

    _matrix[0] = sx;    _matrix[4] = ucamx;     _matrix[8] = fx;    _matrix[12] = -1*(camx);
    _matrix[1] = sy;    _matrix[5] = ucamy;     _matrix[9] = fy;    _matrix[13] = -1*(camy);
    _matrix[2] = sz;    _matrix[6] = ucamz;     _matrix[10] = fz;   _matrix[14] = -1*(camz);
    _matrix[3] = 0.0;   _matrix[7] = 0.0;       _matrix[11] = 0.0;  _matrix[15] = 1.0;
}
RIMatrix4x4 RIMatrix4x4::PerformTranslate(float x, float y, float z)
{
    RIMatrix4x4 translateMatrix;
    translateMatrix._matrix[0] = 1.0;   translateMatrix._matrix[4] = 0.0;   translateMatrix._matrix[8] = 0.0;   translateMatrix._matrix[12] = x;
    translateMatrix._matrix[1] = 0.0;   translateMatrix._matrix[5] = 1.0;   translateMatrix._matrix[9] = 0.0;   translateMatrix._matrix[13] = y;
    translateMatrix._matrix[2] = 0.0;   translateMatrix._matrix[6] = 0.0;   translateMatrix._matrix[10] = 1.0;  translateMatrix._matrix[14] = z;
    translateMatrix._matrix[3] = 0.0;   translateMatrix._matrix[7] = 0.0;   translateMatrix._matrix[11] = 0.0;  translateMatrix._matrix[15] = 1.0;

    RIMatrix4x4 resultantMatrix = PerformMatrixMultiply(translateMatrix);
    return resultantMatrix;
}
RIMatrix4x4 RIMatrix4x4::PerformScale(float scalex, float scaley, float scalez)
{
    //float radians = degrees * PI / 180.0;
    RIMatrix4x4 scaleMatrix;
    scaleMatrix._matrix[0] = scalex;    scaleMatrix._matrix[4] = 0.0;       scaleMatrix._matrix[8] = 0.0;       scaleMatrix._matrix[12] = 0.0;
    scaleMatrix._matrix[1] = 0.0;       scaleMatrix._matrix[5] = scaley;    scaleMatrix._matrix[9] = 0.0;       scaleMatrix._matrix[13] = 0.0;
    scaleMatrix._matrix[2] = 0.0;       scaleMatrix._matrix[6] = 0.00;      scaleMatrix._matrix[10] = scalez;   scaleMatrix._matrix[14] = 0.0;
    scaleMatrix._matrix[3] = 0.0;       scaleMatrix._matrix[7] = 0.0;       scaleMatrix._matrix[11] = 0.0;      scaleMatrix._matrix[15] = 1.0;

    RIMatrix4x4 resultantMatrix = PerformMatrixMultiply(scaleMatrix);
    return resultantMatrix;
}
RIVector4 RIMatrix4x4::MultiplyVector4(RIVector4 vector4)
{
    float x = 0.00;
    float y = 0.00;
    float z = 0.00;
    float w = 0.00;
    vector4.GetValues(x, y, z, w);

    float resultx = _matrix[0] * x + _matrix[4] * y + _matrix[8] * z + _matrix[12] * w;
    float resulty = _matrix[1] * x + _matrix[5] * y + _matrix[9] * z + _matrix[13] * w;
    float resultz = _matrix[2] * x + _matrix[6] * y + _matrix[10] * z + _matrix[14] * w;
    float resultw = _matrix[3] * x + _matrix[7] * y + _matrix[11] * z + _matrix[15] * w;

    RIVector4 retVec4(resultx, resulty, resultz, resultw) ;
    return retVec4;
}
RIMatrix4x4::RIMatrix4x4(float data[16])
{
    _matrix[0] = data[0];
    _matrix[1] = data[1];
    _matrix[2] = data[2];
    _matrix[3] = data[3];
}

RIMatrix4x4::RIMatrix4x4()
{
    SetToIdentity();//start as identity matrix
}

float* RIMatrix4x4::get()
{
    return &_matrix[0];
}
RIMatrix4x4 RIMatrix4x4::operator*(const RIMatrix4x4& otherMat)
{
    RIMatrix4x4 retMat = PerformMatrixMultiply(otherMat);
    return retMat;
}
RIMatrix4x4 RIMatrix4x4::PerformMatrixMultiply(const RIMatrix4x4& otherMat)
{
    RIMatrix4x4 retMat;
    //column major, ROW 0 OF RESULTANT MATRIX
    retMat._matrix[0] = _matrix[0]*otherMat._matrix[0] + _matrix[4]*otherMat._matrix[1] + _matrix[8]*otherMat._matrix[2] + _matrix[12]*otherMat._matrix[3];
    retMat._matrix[4] = _matrix[0]*otherMat._matrix[4] + _matrix[4]*otherMat._matrix[5] + _matrix[8]*otherMat._matrix[6] + _matrix[12]*otherMat._matrix[7];
    retMat._matrix[8] = _matrix[0]*otherMat._matrix[8] + _matrix[4]*otherMat._matrix[9] + _matrix[8]*otherMat._matrix[10] + _matrix[12]*otherMat._matrix[11];
    retMat._matrix[12] = _matrix[0]*otherMat._matrix[12] + _matrix[4]*otherMat._matrix[13] + _matrix[8]*otherMat._matrix[14] + _matrix[12]*otherMat._matrix[15];

    //ROW 1 OF RESULTANT MATRIX
    retMat._matrix[1] = _matrix[1]*otherMat._matrix[0] + _matrix[5]*otherMat._matrix[1] + _matrix[9]*otherMat._matrix[2] + _matrix[13]*otherMat._matrix[3];
    retMat._matrix[5] = _matrix[1]*otherMat._matrix[4] + _matrix[5]*otherMat._matrix[5] + _matrix[9]*otherMat._matrix[6] + _matrix[13]*otherMat._matrix[7];
    retMat._matrix[9] = _matrix[1]*otherMat._matrix[8] + _matrix[5]*otherMat._matrix[9] + _matrix[9]*otherMat._matrix[10] + _matrix[13]*otherMat._matrix[11];
    retMat._matrix[13] = _matrix[1]*otherMat._matrix[12] + _matrix[5]*otherMat._matrix[13] + _matrix[9]*otherMat._matrix[14] + _matrix[13]*otherMat._matrix[15];

    //ROW 2 OF RESULTANT MATRIX
    retMat._matrix[2] = _matrix[2]*otherMat._matrix[0] + _matrix[6]*otherMat._matrix[1] + _matrix[10]*otherMat._matrix[2] + _matrix[14]*otherMat._matrix[3];
    retMat._matrix[6] = _matrix[2]*otherMat._matrix[4] + _matrix[6]*otherMat._matrix[5] + _matrix[10]*otherMat._matrix[6] + _matrix[14]*otherMat._matrix[7];
    retMat._matrix[10] = _matrix[2]*otherMat._matrix[8] + _matrix[6]*otherMat._matrix[9] + _matrix[10]*otherMat._matrix[10] + _matrix[14]*otherMat._matrix[11];
    retMat._matrix[14] = _matrix[2]*otherMat._matrix[12] + _matrix[6]*otherMat._matrix[13] + _matrix[10]*otherMat._matrix[14] + _matrix[14]*otherMat._matrix[15];

    //ROW 3 OF RESULTANT MATRIX
    retMat._matrix[3] = _matrix[3]*otherMat._matrix[0] + _matrix[7]*otherMat._matrix[1] + _matrix[11]*otherMat._matrix[2] + _matrix[15]*otherMat._matrix[3];
    retMat._matrix[7] = _matrix[3]*otherMat._matrix[4] + _matrix[7]*otherMat._matrix[5] + _matrix[11]*otherMat._matrix[6] + _matrix[15]*otherMat._matrix[7];
    retMat._matrix[11] = _matrix[3]*otherMat._matrix[8] + _matrix[7]*otherMat._matrix[9] + _matrix[11]*otherMat._matrix[10] + _matrix[15]*otherMat._matrix[11];
    retMat._matrix[15] = _matrix[3]*otherMat._matrix[12] + _matrix[7]*otherMat._matrix[13] + _matrix[11]*otherMat._matrix[14] + _matrix[15]*otherMat._matrix[15];
    return retMat;
}
RIMatrix4x4 RIMatrix4x4::PerformRotateAboutX(float degrees)
{
    float radians = degrees * PI / 180.0;
    RIMatrix4x4 xrotMatrix;
    xrotMatrix._matrix[0] = 1.0; xrotMatrix._matrix[4] = 0.0;               xrotMatrix._matrix[8] = 0.0;            xrotMatrix._matrix[12] = 0.0;
    xrotMatrix._matrix[1] = 0.0; xrotMatrix._matrix[5] = cos(radians);      xrotMatrix._matrix[9] = (-1)*sin(radians);   xrotMatrix._matrix[13] = 0.0;
    xrotMatrix._matrix[2] = 0.0; xrotMatrix._matrix[6] = (sin(radians)); xrotMatrix._matrix[10] = cos(radians);  xrotMatrix._matrix[14] = 0.0;
    xrotMatrix._matrix[3] = 0.0; xrotMatrix._matrix[7] = 0.0;               xrotMatrix._matrix[11] = 0.0;           xrotMatrix._matrix[15] = 1.0;

    RIMatrix4x4 resultantMatrix = PerformMatrixMultiply(xrotMatrix);
    return resultantMatrix;
}
RIMatrix4x4 RIMatrix4x4::PerformRotateAboutY(float degrees)
{
    float radians = degrees * PI / 180.0;
    RIMatrix4x4 yrotMatrix;
    yrotMatrix._matrix[0] = cos(radians);   yrotMatrix._matrix[4] = 0.0;    yrotMatrix._matrix[8] = (sin(radians));  yrotMatrix._matrix[12] = 0.0;
    yrotMatrix._matrix[1] = 0.0;            yrotMatrix._matrix[5] = 1.0;    yrotMatrix._matrix[9] = 0.0;                yrotMatrix._matrix[13] = 0.0;
    yrotMatrix._matrix[2] = (-1) *sin(radians);   yrotMatrix._matrix[6] = 0.0;    yrotMatrix._matrix[10] = cos(radians);      yrotMatrix._matrix[14] = 0.0;
    yrotMatrix._matrix[3] = 0.0;            yrotMatrix._matrix[7] = 0.0;    yrotMatrix._matrix[11] = 0.0;               yrotMatrix._matrix[15] = 1.0;

    RIMatrix4x4 resultantMatrix = PerformMatrixMultiply(yrotMatrix);
    return resultantMatrix;
}
RIMatrix4x4 RIMatrix4x4::PerformRotateAboutZ(float degrees)
{
    float radians =  degrees * PI / 180.0;
    RIMatrix4x4 zrotMatrix;
    zrotMatrix._matrix[0] = cos(radians);   zrotMatrix._matrix[4] = -1.0*(sin(radians));    zrotMatrix._matrix[8] = 0.0;    zrotMatrix._matrix[12] = 0.0;
    zrotMatrix._matrix[1] = sin(radians);   zrotMatrix._matrix[5] = cos(radians);           zrotMatrix._matrix[9] = 0.0;    zrotMatrix._matrix[13] = 0.0;
    zrotMatrix._matrix[2] = 0.0;            zrotMatrix._matrix[6] = 0.0;                    zrotMatrix._matrix[10] = 1.0;   zrotMatrix._matrix[14] = 0.0;
    zrotMatrix._matrix[3] = 0.0;            zrotMatrix._matrix[7] = 0.0;                    zrotMatrix._matrix[11] = 0.0;   zrotMatrix._matrix[15] = 1.0;

    RIMatrix4x4 resultantMatrix = PerformMatrixMultiply(zrotMatrix);
    return resultantMatrix;
}
void RIMatrix4x4::SetToIdentity()
{
    _matrix[0] = 1.0; _matrix[4] = 0.0; _matrix[8] = 0.0; _matrix[12] = 0.0;
    _matrix[1] = 0.0; _matrix[5] = 1.0; _matrix[9] = 0.0; _matrix[13] = 0.0;
    _matrix[2] = 0.0; _matrix[6] = 0.0; _matrix[10] = 1.0; _matrix[14] = 0.0;
    _matrix[3] = 0.0; _matrix[7] = 0.0; _matrix[11] = 0.0; _matrix[15] = 1.0;
}
RIVector4::RIVector4(float x, float y, float z, float w)
{
    _vector[0] = x;
    _vector[1] = y;
    _vector[2] = z;
    _vector[3] = w;
}

RIVector3::RIVector3(float x, float y, float z)//, float w)
{
    _vector[0] = x;
    _vector[1] = y;
    _vector[2] = z;
    //_w = w;
}

RIVector4 RIVector4::operator+ (const RIVector4& a)
{
    float x = a._vector[0] + _vector[0];
    float y = a._vector[1] + _vector[1];
    float z = a._vector[2] + _vector[2];
    float w = a._vector[3] + _vector[3];

    RIVector4 retValue(x, y, z, w);
    return retValue;
}

RIVector4 RIVector4::operator- (const RIVector4& a)
{
    float x = a._vector[0] - _vector[0];
    float y = a._vector[1] - _vector[1];
    float z = a._vector[2] - _vector[2];
    float w = a._vector[3] - _vector[3];

    RIVector4 retValue(x, y, z, w);
    return retValue;
}

float DotProductVec3(RIVector3 vector1, RIVector3 vector2)
{
    float x1 = 0.00;
    float y1 = 0.00;
    float z1 = 0.00;
    //float w1 = 0.00;
    vector1.GetValues(x1, y1, z1);//, w1);
    float x2 = 0.00;
    float y2 = 0.00;
    float z2 = 0.00;
    //float w2 = 0.00;
    vector2.GetValues(x2, y2, z2);//, w2);
    return x1 * x2 + y1 * y2 + z1 * z2 ;;
}

RIVector3 CrossProduct(RIVector3 vector1, RIVector3 vector2)
{
    float x1 = 0.00;
    float y1 = 0.00;
    float z1 = 0.00;
    vector1.GetValues(x1, y1, z1);
    float x2 = 0.00;
    float y2 = 0.00;
    float z2 = 0.00;
    vector2.GetValues(x2, y2, z2);

    float x3 = y1 * z2 - z1 * y2;
    float y3 = z1 * x2 - x1 * z2;
    float z3 = x1 * y2 - y1 * x2;
    RIVector3 vector3(x3, y3, z3);
    return vector3;

}
void RIVector3::GetValues(float& x, float& y, float& z)
{
    x = _vector[0];
    y = _vector[1];
    z = _vector[2];
    //w = _w;
}

void RIVector4::GetValues(float& x, float& y, float& z, float& w)
{
    x = _vector[0];
    y = _vector[1];
    z = _vector[2];
    w = _vector[3];
}
