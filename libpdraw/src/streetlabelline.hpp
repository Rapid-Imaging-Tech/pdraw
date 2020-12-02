//
//  streetlabelline.hpp
//  SmartCam
//
//  Created by David Dvergsten on 4/23/20.
//  Copyright © 2020 Rapid Imaging Technologies. All rights reserved.
//

//#ifndef streetlabelline_hpp
//#define streetlabelline_hpp
//
//#include <stdio.h>
//
//#endif /* streetlabelline_hpp */
#ifndef STREETLABELLINE_H
#define STREETLABELLINE_H


//class StreetLabelLine
//{
//public:
//    StreetLabelLine();
//};
//
//  StreetLabel.hpp
//  TextScaleCollisionDetection
//
//  Created by David Dvergsten on 1/27/20.
//  Copyright © 2020 Rapid Imaging Tech. All rights reserved.
//

//#ifndef StreetLabel_hpp
//#define StreetLabel_hpp
#include "boost/shared_ptr.hpp"
#include <stdio.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <memory>
using namespace std;
class SLPoint{
public:
    float operator - (const SLPoint &obj);//get distance between 2 points
    SLPoint(){
        X = 0.0f;
        Y = 0.0f;
    };
    SLPoint(float x, float y){X=x;Y=y;};
    SLPoint(const SLPoint& p){
        X = p.X;
        Y = p.Y;
    }

    float X;
    float Y;
};

//just a container for 2 points as a line
class SLLine{
public:
    void PrintCoordinates(){
        cout<<"point1 x, y: "<<_point1.X<<", "<<_point1.Y<<" ";
        cout<<"point2 x, y: "<<_point2.X<<", "<<_point2.Y<<endl;
    };
    SLLine(){

    };
    SLLine(SLPoint p1, SLPoint p2):
    _point1(p1),
    _point2(p2)
    {

    };
    SLPoint _point1;
    SLPoint _point2;
};
bool IsIntersecting(SLPoint a, SLPoint b, SLPoint c, SLPoint d);//todo add to line class??

class SLVector{
public:
    SLVector(float deltax, float deltay):
    _deltaX(deltax),
    _deltaY(deltay)
    {
    }
    void Normalize(){//make unit vector of length 1
        float distance = sqrt(_deltaX*_deltaX + _deltaY * _deltaY);
        _deltaX = _deltaX / distance;
        _deltaY = _deltaY / distance;
    }
    //set length of vector
    void Multiply(float val){
        _deltaX *= val;
        _deltaY *= val;
    }
    float GetDeltaX(){return _deltaX;};
    float GetDeltaY(){return _deltaY;};
private:
    float _deltaX;
    float _deltaY;
};


class StreetLabelLine{
public:
    //return scale of text at which these 2 lines will collide
    //always collide= no matter what feasible scale, these 2 labels will collide and can't be displayed together
    //nevercollide= no matter what scale(within max-so they don't go beyond end) these 2 lines will not collide and can always co-exist
    float CalculateCollisionScale(boost::shared_ptr<StreetLabelLine> otherLabel, bool& alwaysCollide, bool& neverCollide);
    bool IsCollision( boost::shared_ptr<StreetLabelLine> otherLabel);
    //StreetLabel(Point p1, Point p2, float textLength, float textHeight);
    StreetLabelLine(std::vector<SLPoint>, float textLength, float textHeight);
    void Init();
    float _textLengthAtUnity;//base text scale ie scale = 1
    float _maxScale;//max Scale before text runs off end of street line
    float _streetLength;//length of street label line given by osm data
    float _baseScaleTextHeight;//text height at base scale
    float _maxScaleTextHeight;
private:
    void ReCalculateCLPoints(float textScale);
    void CopyCLPoints(vector<SLPoint> centerLinePoints);
    //take all the street lines and find the center point inside the 1 street line
    //ie add them all up, divide by 2 for length.  Then find which segment center belongs in
    void CalculateCenterPoint();
    std::vector<SLPoint> _vecCLPoints;  //add all the points that make up centerline to this

    //TODO WE NEED TO CREATE A VECTOR OF CENTERLINE POINTS, BUT CHOPPED OFF AT WHERE THE LABEL WILL START AND END
    //  AT GIVEN SCALE AND USE THAT FOR OUR OFFSETS.
    //basically _vecCLPoints, but cut off at given scale of text.  we need to compute every time we test a new scale.
    std::vector<SLPoint> _vecCLPointsAtGivenScale;

    //these are the offset lines to left and right of centerline at given scale
    //these will be used to detect collisions between text.
    //add the lines at offset to these vectors, one for the left side one for the right side.

    //every time we detect collisions, we need to clear these 2 and recalculate offsets/endpoints
    std::vector<SLLine> _offsetLinesL;
    std::vector<SLLine> _offsetLinesR;

    //connect ends of the label ie so we box in the label, we have offsets, and lines connecting os 1 to os 2 at endpoint1 etc
    SLLine _textBoxEdgeBegin;
    SLLine _textBoxEdgeEnd;

    //void CalculateMaxOffsets();
    void CalculateOffsets(float textScale);
    float CalculateStreetLength();//length of centerline of street label ie what osm gives us
    float CalculateMaxScale();//the max scale we can make the text without the text running over the endpoints of streetlabel line

    //compare other streetlabel with "this" streetlabel, compare each textboxline of this label with each textbox line of other
    //street label.  if any intersection then we have a collision at said scale
    bool CompareOffsetL(boost::shared_ptr<StreetLabelLine> otherLabel);
    bool CompareOffsetR(boost::shared_ptr<StreetLabelLine> otherLabel);

    bool CompareBeginEdge(boost::shared_ptr<StreetLabelLine> otherLabel);
    bool CompareEndEdge(boost::shared_ptr<StreetLabelLine> otherLabel);

    void PrintOffsets(StreetLabelLine* sl);

};

#endif // STREETLABELLINE_H
