


//
//  RIMatrixMath.hpp
//  RIMatrixMath
//
//  Created by David Dvergsten on 2/5/19.
//  Copyright © 2019 Rapid Imaging Technologies. All rights reserved.
//

#ifndef RIMatrixMath_hpp
#define RIMatrixMath_hpp

#include <stdio.h>
class RIVector4;
class RIVector3;
class RIMatrix3x3
{
public:
    RIVector3 MultiplyVector3(RIVector3 vector3);//return a resultant vector4 from multiplying a vector times current matrix.  _matrix unchanged
    RIMatrix3x3();
    void SetRotateAboutAxisMatrix(RIVector3 axis, float degrees);
    float _matrix[9];//3x3
};
class RIMatrix4x4
{
public:
    void SetToViewport(float left, float bottom, float width, float height, float nearPlane, float farPlane);
    void SetToOrthographicMatrix(float left, float right, float bottom, float top, float near, float far);
    void SetToPerspectiveMatrix(float vfovDegrees, float aspect, float near, float far);
    //void SetToPerspectiveMatrix(float vfov Degrees, float aspect, float near, float far);
    RIVector3 RotateVectorAboutAxis(RIVector3 axis, RIVector3 vectorToRotate, float degrees);
    void SetToLookAtMatrix(float camx, float camy, float camz, float losPitch, float losYaw, float losRoll);//overwrit matrix to be lookatmatrix
    void SetToIdentity();//overwrite matrix to be identity matrix
    RIMatrix4x4();//set to identity matrix?
    RIMatrix4x4(float data[16]);
    RIMatrix4x4 operator*(const RIMatrix4x4&);

    RIMatrix4x4 PerformScale(float scalex, float scaley, float scalez);//create scalematrix and multiply times "this" matrix and return the result
    RIMatrix4x4 PerformTranslate(float x, float y, float z);//create translatematrix and multiply times "this" matrix and return the result
    RIMatrix4x4 PerformRotateAboutX(float degrees);//create xrotatematrix and multiply times "this" matrix and return the result
    RIMatrix4x4 PerformRotateAboutY(float degrees);//create yrotatematrix and multiply times "this" matrix and return the result
    RIMatrix4x4 PerformRotateAboutZ(float degrees);//create zrotatematrix and multiply times "this" matrix and return the result
    RIVector4 MultiplyVector4(RIVector4 vector4);//return a resultant vector4 from multiplying a vector times current matrix.  _matrix unchanged
    float* get();
private:
    RIMatrix4x4 PerformMatrixMultiply(const RIMatrix4x4& otherMat);
    float _matrix[16];//column major, ie 0 - 4 is the first column of matrix
};

class RIVector3
{
public:
    RIVector3(float x, float y, float z);
    RIVector3 operator+ (const RIVector3&);
    RIVector3 operator- (const RIVector3&);
    void GetValues(float& x, float& y, float& z);
private:
    float _vector[3];
    //float _x;
    //float _y;
    //float _z;
};

class RIVector4
{
public:
    RIVector3 ToVector3DAffine();
    RIVector4(float x, float y, float z, float w);
    RIVector4 operator+ (const RIVector4&);
    RIVector4 operator- (const RIVector4&);
    void GetValues(float& x, float& y, float& z, float& w);
    float* get(){return &_vector[0];};
private:
    float _vector[4];
};

class RIRotationTK
{
public:
    RIRotationTK(int windowWidth, int windowHeight){
        sx = windowWidth;
        sy = windowHeight;
    };
    void LoadMatrices() ;
    void ResizeWindow(int nx, int ny);
    void SetFOVs(double hFov, double vFov);
    void SetCamera(double *camcart,double pitch, double heading, double roll);
    RIVector4 GetPixels4Cart(double cart[]);
private:
    RIMatrix4x4 mvpM;
    RIMatrix4x4 viewpM;
    RIMatrix4x4 viewM;
    RIMatrix4x4 persM;
    int sx,sy;
    double hFOV, vFOV, iFOV,aspectRatio;

};

RIVector3 CrossProduct(RIVector3 vector1, RIVector3 vector2);
float DotProductVec3(const RIVector3& vector1, const RIVector3& vector2);
RIVector4 GetPixels4Cart(double cart[]);
//void LoadMatrices() ;
//void ResizeWindow(int nx, int ny);
//void SetFOVs(double hFov, double vFov);
//void SetCamera(double *camcart,double pitch, double heading, double roll);

//float DotProductVec4(const RIVector4& vector1, const RIVector4& vector2);
#endif /* RIMatrixMath_hpp */
