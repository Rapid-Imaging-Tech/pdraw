//
//  streetlabelline.cpp
//  SmartCam
//
//  Created by David Dvergsten on 4/23/20.
//  Copyright © 2020 Rapid Imaging Technologies. All rights reserved.
//

#include "streetlabelline.hpp"
//#include "streetlabelline.h"
//
//  StreetLabel.cpp
//  TextScaleCollisionDetection
//
//  Created by David Dvergsten on 1/27/20.
//  Copyright © 2020 Rapid Imaging Tech. All rights reserved.
//

//#include "StreetLabel.hpp"
#include <cmath>
#include <iostream>
using namespace std;

//TODO PUT IN UTILITY FILE?
bool IsIntersecting(SLLine& l1,  SLLine& l2){//put inside line class?? this->IsIntersecting
    SLPoint a(l1._point1);
    SLPoint b(l1._point2);
    SLPoint c(l2._point1);
    SLPoint d(l2._point2);
    return IsIntersecting(a, b, c, d);
}
bool IsIntersecting(SLPoint a, SLPoint b, SLPoint c, SLPoint d)
{

    float denominator = ((b.X - a.X) * (d.Y - c.Y)) - ((b.Y - a.Y) * (d.X - c.X));
    float numerator1 = ((a.Y - c.Y) * (d.X - c.X)) - ((a.X - c.X) * (d.Y - c.Y));
    float numerator2 = ((a.Y - c.Y) * (b.X - a.X)) - ((a.X - c.X) * (b.Y - a.Y));

    // Detect coincident lines (has a problem, read below)
    if (denominator == 0) return numerator1 == 0 && numerator2 == 0;

    float r = numerator1 / denominator;
    float s = numerator2 / denominator;

    return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
}

//distance between 2 points
float SLPoint::operator - (const SLPoint &obj){
    //use a^2 + b^2 = c^2
    float a = obj.X - this->X;
    float b = obj.Y - this->Y;
    float c = sqrt(a*a + b*b);
    //Point pResult(0.0f, 0.0f);
    return c;
}

void StreetLabelLine::CopyCLPoints(vector<SLPoint> centerLinePoints){
    for(int i = 0 ; i < centerLinePoints.size() ; i++){
        SLPoint p = centerLinePoints[i];
        _vecCLPoints.push_back(p);
    }
}
StreetLabelLine::StreetLabelLine(std::vector<SLPoint> centerLinePoints, float textLength, float textHeight):
_textLengthAtUnity(textLength),
_baseScaleTextHeight(textHeight)
{
    CopyCLPoints(centerLinePoints);
}

void StreetLabelLine::Init(){
    //calculate the max scale before text runs over street line length
    _maxScale = CalculateMaxScale();
    CalculateOffsets(_maxScale);
}

//calc centerline of streetlabel line ie the line that osm gives us
float StreetLabelLine::CalculateStreetLength()
{
    //TODO WILL HAVE TO ACCOUNT FOR MANY STREET VECTORS EVENTUALLY, RIGHT NOW JUST 1 LINE
    float totalDistance = 0.0f;
    for(int i = 0 ; i < _vecCLPoints.size() - 1 ; i++)
    {
        SLPoint p0 = _vecCLPoints[i];
        SLPoint p1 = _vecCLPoints[i + 1];
        totalDistance += p1 - p0;// always should be positive since we square everything in the diff function
    }
    return totalDistance;
}

//compare base length of text string with length of streetlabel centerline(as given by osm), and come up
//with the max scale we could make the text before running off the end points of centerline
float StreetLabelLine::CalculateMaxScale()
{
    _streetLength = CalculateStreetLength();
    _maxScale = _streetLength / _textLengthAtUnity;
    return _maxScale;
}

void StreetLabelLine::ReCalculateCLPoints(float textScale)
{
    //take _vecCLPoints, and given scale.  multiply given scale * text length which is our length of text at that scale
    //recompute cl points where start point and endpoint denote start/end of text at given scale along centerline.
    _vecCLPointsAtGivenScale.clear();//we're starting fresh with a new scale
    //length of label at given scale = _textLength at base scale * textScale
    //take full length of centerline - new length at scale.  divide by 2.  This is the empty space at the beginning and end
    //of the full centerline.  now we need to calculate a new centerline, which is encompassed inside the full centerline but at given scale
    float textLengthAtScale = _textLengthAtUnity * textScale;
    float startPointDist = (_streetLength - textLengthAtScale)/2.0f;//this is where start is distance wise along full centerline
    float endPointDist = startPointDist + textLengthAtScale;//this is where the endpoint is distance wise along full centerline

    bool startPointAdded = false;
    bool endPointAdded = false;
    for(int i = 0 ; i < _vecCLPoints.size() - 1 && !endPointAdded; i++)
    {
        SLPoint p1 = _vecCLPoints[i];
        SLPoint p2 = _vecCLPoints[i+1];
        float currentSegmentDist = p2 - p1;//dist between points
        startPointDist -= currentSegmentDist ;
        if(startPointDist < 0 && startPointAdded == false)
        {
            //we're inside the segment where the new start point is
            //so we're -startPointDist units from endpoint2 in this line segment

            //TODO AS LONG AS WE DON'T HIT THE ENDPOINT, KEEP ADDING POINTS TO THE NEW CENTERLINE
            //  ONCE WE HIT THE ENDPOINT, WE CHOP OFF THE END PIECE AND MAKE THAT THE NEW ENDPOINT.
            //  AND ADD THAT LINE SEGMENT TO THE LIST, THEN WE'RE DONE

            //THE LINE SEGMENT IS NOW the calculated start point and p2.

            SLVector startVector(p2.X - p1.X, p2.Y - p1.Y);
            startVector.Normalize();
            startVector.Multiply(startPointDist + currentSegmentDist);//pointing from p1 to p2, startPointDist will be dist from p2 so take dist from p1

            SLPoint startPoint(p1.X + startVector.GetDeltaX(), p1.Y + startVector.GetDeltaY());
            _vecCLPointsAtGivenScale.push_back(startPoint);
            startPointAdded = true;
        }
        endPointDist -= currentSegmentDist ;//keep track of where we are in relation to
        if(endPointDist < 0 && !endPointAdded){
            SLVector endVector(p2.X - p1.X, p2.Y - p1.Y);
            endVector.Normalize();
            endVector.Multiply(endPointDist + currentSegmentDist);//end point is on this line

            SLPoint endPoint(p1.X + endVector.GetDeltaX(), p1.Y + endVector.GetDeltaY());
            _vecCLPointsAtGivenScale.push_back(endPoint);
            //we're done now
            endPointAdded = true;
        }
        if(startPointAdded && !endPointAdded){
            _vecCLPointsAtGivenScale.push_back(p2);
        }
    }
}

//create 2 vectors 1 for left side of center and 1 for right.  Add line segments to these lists which are the offset lines to centerline
//denoting the area where the text will take up this space.
//void StreetLabel::CalculateMaxOffsets()
void StreetLabelLine::CalculateOffsets(float textScale)
{
    bool useFullCenterline = false;//if we're using max text scale, don't try recalculate centerline at given scale, just use full centerline given by osm
    if(textScale == _maxScale){
        useFullCenterline = true;
    }
    else{
        ReCalculateCLPoints(textScale);
    }
    _offsetLinesL.clear();
    _offsetLinesR.clear();
    //max scale of text before the text disappears from being too long, * 1/2 of the height of the text * baseTextheight, which is text height at scale = 1
    //float offsetDistFromCenterline = _maxScale * 0.5f * _baseScaleTextHeight;
    float offsetDistFromCenterline = textScale * 0.5f * _baseScaleTextHeight;

    //the points at the end of the text box, we will create 2 lines with to seal up the ends
    SLPoint startPointLeft;
    SLPoint startPointRight;
    SLPoint endPointLeft;
    SLPoint endPointRight;
    unsigned long pointCount = 0;
    if(useFullCenterline){
        pointCount = _vecCLPoints.size();
    }
    else{
        pointCount = _vecCLPointsAtGivenScale.size();
    }
    for(int i = 0 ; i < pointCount - 1 ; i++)
    {
        SLPoint endPoint1;
        SLPoint endPoint2;
        if(useFullCenterline){
             endPoint1 = _vecCLPoints[i];
             endPoint2 = _vecCLPoints[i + 1];
        }
        else{
             endPoint1 = _vecCLPointsAtGivenScale[i];
             endPoint2 = _vecCLPointsAtGivenScale[i + 1];
        }
        //take the max scale and calculate offsets from line based on that
        //using max scale means we use the whole length of the street so just offset the whole thing
        //take vector of ep1 to ep2, and normalize it
        SLVector streetLineVector(endPoint1.X - endPoint2.X, endPoint1.Y - endPoint2.Y);
        streetLineVector.Normalize();

        //then reverse delta x/y from normalized vector, alternate signs for each side of the offset,
        // multiply times offset distance and add to ep1 then ep2 to get L/R offsets

        //calculate left offset of this street cl segment and add to Left Offset vector
        SLVector offsetL(-1.0 * streetLineVector.GetDeltaY(), streetLineVector.GetDeltaX());
        offsetL.Multiply(offsetDistFromCenterline);

        SLVector offsetR(streetLineVector.GetDeltaY(), -1.0f * streetLineVector.GetDeltaX());
        offsetR.Multiply(offsetDistFromCenterline);

        //now add offset1 to endpoint1  and endpoint2, these lines calculate the endps of 1 offset line
        SLPoint endp0LeftOffset(endPoint1.X + offsetL.GetDeltaX(), endPoint1.Y + offsetL.GetDeltaY());
        SLPoint endp1LeftOffset(endPoint2.X + offsetL.GetDeltaX(), endPoint2.Y + offsetL.GetDeltaY());
        if(i == 0){
            //then endp0Right is the corner of the textbox
            startPointLeft = endp0LeftOffset;
        }
        if(i == pointCount - 2){
            //then this is the last line segment, so endpoint1 is the right corner of the textbox
            endPointLeft = endp1LeftOffset;
        }

        //now encapsulate into a line
        SLLine lineLeft(endp0LeftOffset, endp1LeftOffset);
        _offsetLinesL.push_back(lineLeft);


        //calculate right offset of this street cl segment and add to Right Offset vector
        //now add offset1 to endpoint1  and endpoint2, these 4 lines calculate the endps of 1 offset line
        SLPoint endp0RightOffset(endPoint1.X + offsetR.GetDeltaX(), endPoint1.Y + offsetR.GetDeltaY());
        SLPoint endp1RightOffset(endPoint2.X + offsetR.GetDeltaX(), endPoint2.Y + offsetR.GetDeltaY());
        if(i == 0){
            //then endp0Right is the corner of the textbox
            startPointRight = endp0RightOffset;
        }
        if(i == pointCount - 2){
            //then this is the last line segment, so endpoint1 is the right corner of the textbox
            endPointRight = endp1RightOffset;
        }
        //now encapsulate into a line
        SLLine lineRight(endp0RightOffset, endp1RightOffset);
        _offsetLinesR.push_back(lineRight);
    }

    //the line the seals up the ends at beginning of textbox
    _textBoxEdgeBegin._point1 = startPointRight;
    _textBoxEdgeBegin._point2 = startPointLeft;

    //the line the seals up the ends at end of textbox
    _textBoxEdgeEnd._point1 = endPointRight;
    _textBoxEdgeEnd._point2 = endPointLeft;
}

//TODO MAKE SURE WE DON'T DOUBLE CHECK streetlabels IE DON'T RECHECK STREET LABEL 1 WITH STREET LABEL 2

//check the left offsets of "this" label with l/r of other label and endlines(outside lines of the text box, begin/end)
bool StreetLabelLine::CompareOffsetL(boost::shared_ptr<StreetLabelLine> otherLabel)
{
    //compare "this" L offsets with each offset(l/r) of other street label for a collision
    //also begin and end lines
    for(int i = 0 ; i < _offsetLinesL.size() ; i++)
    {
        SLLine thisLine = _offsetLinesL[i];
        //check left side
        for(int j = 0 ; j < otherLabel->_offsetLinesL.size() ; j++)
        {
            if(IsIntersecting(thisLine, otherLabel->_offsetLinesL[j]))
            {
                return true;
            }
        }
        //check right side
        for(int j = 0 ; j < otherLabel->_offsetLinesR.size() ; j++)
        {
            if(IsIntersecting(thisLine, otherLabel->_offsetLinesR[j]))
            {
                return true;
            }
        }
        if(IsIntersecting(thisLine, otherLabel->_textBoxEdgeBegin))
            return true;

        if(IsIntersecting(thisLine, otherLabel->_textBoxEdgeEnd))
            return true;
    }
    //compare end lines of other
    //IsIntersecting(otherLabel._textBoxEdgeBeginMaxScale
    return false;
}

bool StreetLabelLine::CompareOffsetR(boost::shared_ptr<StreetLabelLine> otherLabel)
{
    //compare "this" L offsets with each offset(l/r) of other street label for a collision
    //also begin and end lines
    for(int i = 0 ; i < _offsetLinesR.size() ; i++)
    {
        SLLine thisLine = _offsetLinesR[i];
        //check left side
        for(int j = 0 ; j < otherLabel->_offsetLinesL.size() ; j++)
        {
            if(IsIntersecting(thisLine, otherLabel->_offsetLinesL[j]))
            {
                return true;
            }
        }
        //check right side
        for(int j = 0 ; j < otherLabel->_offsetLinesR.size() ; j++)
        {
            if(IsIntersecting(thisLine, otherLabel->_offsetLinesR[j]))
            {
                return true;
            }
        }
        if(IsIntersecting(thisLine, otherLabel->_textBoxEdgeBegin))
            return true;

        if(IsIntersecting(thisLine, otherLabel->_textBoxEdgeEnd))
            return true;
    }
    //compare end lines of other
    //IsIntersecting(otherLabel._textBoxEdgeBeginMaxScale
    return false;
}

//compare each

bool StreetLabelLine::CompareBeginEdge(boost::shared_ptr<StreetLabelLine> otherLabel)
{
    SLLine thisLine = _textBoxEdgeBegin;
    //check left side
    for(int j = 0 ; j < otherLabel->_offsetLinesL.size() ; j++)
    {
        if(IsIntersecting(thisLine, otherLabel->_offsetLinesL[j]))
        {
            return true;
        }
    }
    //check right side
    for(int j = 0 ; j < otherLabel->_offsetLinesR.size() ; j++)
    {
        if(IsIntersecting(thisLine, otherLabel->_offsetLinesR[j]))
        {
            return true;
        }
    }
    if(IsIntersecting(thisLine, otherLabel->_textBoxEdgeBegin))
        return true;

    if(IsIntersecting(thisLine, otherLabel->_textBoxEdgeEnd))
        return true;

    return false;
}
bool StreetLabelLine::CompareEndEdge(boost::shared_ptr<StreetLabelLine> otherLabel)
{
    SLLine thisLine = _textBoxEdgeEnd;
    //check left side
    for(int j = 0 ; j < otherLabel->_offsetLinesL.size() ; j++)
    {
        if(IsIntersecting(thisLine, otherLabel->_offsetLinesL[j]))
        {
            return true;
        }
    }
    //check right side
    for(int j = 0 ; j < otherLabel->_offsetLinesR.size() ; j++)
    {
        if(IsIntersecting(thisLine, otherLabel->_offsetLinesR[j]))
        {
            return true;
        }
    }
    if(IsIntersecting(thisLine, otherLabel->_textBoxEdgeBegin))
        return true;

    if(IsIntersecting(thisLine, otherLabel->_textBoxEdgeEnd))
        return true;

    return false;
}
bool StreetLabelLine::IsCollision( boost::shared_ptr<StreetLabelLine> otherLabel)
{
    bool isIntersection = false;
    //todo now we need to cycle through all offset lines and compare with all offset lines of other StreetLabel
    // if any line collides from "this" streetlabel with line from otherlabel, then there is a collision.
    isIntersection = CompareOffsetL(otherLabel);
    if(isIntersection)
        return true;
    isIntersection = CompareOffsetR(otherLabel);
    if(isIntersection)
        return true;
    isIntersection = CompareBeginEdge(otherLabel);
    if(isIntersection)
        return true;
    isIntersection = CompareEndEdge(otherLabel);
    if(isIntersection)
        return true;

    return false;

}

float StreetLabelLine::CalculateCollisionScale(boost::shared_ptr<StreetLabelLine> otherLabel, bool& alwaysCollide, bool& neverCollide)
{
    //we want to return a scale where just higher than that scale we get a collision.  As close as possible without going over
    //_maxScale = maxscale before going off edge

    //start at smallest max scale between the two lines
    float smallestMaxScale = _maxScale;
    if(otherLabel->_maxScale < this->_maxScale)
        smallestMaxScale = otherLabel->_maxScale;
    else
        smallestMaxScale = this->_maxScale;

    CalculateOffsets(smallestMaxScale);
    otherLabel->CalculateOffsets(smallestMaxScale);

    bool collision = IsCollision(otherLabel);
    if(!collision){
        neverCollide = true;//if they don't collide at this scale they never will
    }
    else{
        //They collide at the smaller of their 2 max scales so there could be a collision between the 2
        neverCollide = false;
    }

    float currentScale = smallestMaxScale;
    int count = 0;
    while(collision && count < 10)
    {

        currentScale = currentScale / 2.0f;
        CalculateOffsets(currentScale);
        otherLabel->CalculateOffsets(currentScale);
        collision = IsCollision(otherLabel);

        count++;
    }
    if(count >= 10)
        alwaysCollide = true;
    else{
        //PrintOffsets(this);//just for debugging purposes
        //PrintOffsets(otherLabel.get());
        alwaysCollide = false;
    }

    return currentScale;
    //TODO start at max scale of label with smallest max scale detect if collision
    //, if collision half the scale of each and re-init at the new scale and detect collision
    // get each label max scale, use the calculateoffsets function of each, with the desired scale, then use the isCollision function

}

void StreetLabelLine::PrintOffsets(StreetLabelLine* sl){

    //iterate through offset lines, and endlines and print coordinates to screen so we can check
    std::cout<<"Print Offsets for line**********"<<endl;

    std::cout<<"--Print left offset"<<endl;
    for(int i = 0 ; i < sl->_offsetLinesL.size() ; i++)
    {
        std::cout<<"line "<<i<<": ";
        SLLine currentLine = sl->_offsetLinesL[i];
        currentLine.PrintCoordinates();
    }

    std::cout<<"--Print right offset"<<endl;
    for(int i = 0 ; i < sl->_offsetLinesR.size() ; i++)
    {
        std::cout<<"line "<<i<<": ";
        SLLine currentLine = sl->_offsetLinesR[i];
        currentLine.PrintCoordinates();
    }

    std::cout<<"--Print begin line"<<endl;
    //for(int i = 0 ; i < sl->_offsetLinesR.size() ; i++)
    //{
        //std::cout<<"line "<<i<<": ";
        SLLine currentLineB = sl->_textBoxEdgeBegin;
        currentLineB.PrintCoordinates();

    std::cout<<"--Print end line"<<endl;
    //for(int i = 0 ; i < sl->_offsetLinesR.size() ; i++)
    //{
        //std::cout<<"line "<<i<<": ";
        SLLine currentLineE = sl->_textBoxEdgeEnd;
        currentLineE.PrintCoordinates();
    //}
}
