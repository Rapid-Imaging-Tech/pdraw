 //
//  StreetLineManager.cpp
//  StreetLineMath
//
//  Created by David Dvergsten on 4/15/19.
//  Copyright © 2019 David Dvergsten. All rights reserved.
//
#include <iostream>
#include "streetlinetriangulation.hpp"
#include <math.h>
//std::string StreetLineManager::getName()
//{
//    return "Dave";
//}
//REMOVE LATER
inline double Det(double a, double b, double c, double d)
{
    return a*d - b*c;
}

//REMOVE LATER
bool LineLineIntersect(double x1, double y1, //Line 1 start
    double x2, double y2, //Line 1 end
    double x3, double y3, //Line 2 start
    double x4, double y4, //Line 2 end
    double &ixOut, double &iyOut) //Output
{
    double detL1 = Det(x1, y1, x2, y2);
    double detL2 = Det(x3, y3, x4, y4);
    double x1mx2 = x1 - x2;
    double x3mx4 = x3 - x4;
    double y1my2 = y1 - y2;
    double y3my4 = y3 - y4;

    double xnom = Det(detL1, x1mx2, detL2, x3mx4);
    double ynom = Det(detL1, y1my2, detL2, y3my4);
    double denom = Det(x1mx2, y1my2, x3mx4, y3my4);
    if(denom == 0.0)//Lines don't seem to cross
    {
        ixOut = NAN;
        iyOut = NAN;
        return false;
    }

    ixOut = xnom / denom;
    iyOut = ynom / denom;
    if(!isfinite(ixOut) || !isfinite(iyOut)) //Probably a numerical issue
        return false;

    return true; //All OK
}
bool TriangulationLineString::GetSlopeAndIntercept(std::shared_ptr<TriangulationPoint> startPoint,
                                                   std::shared_ptr<TriangulationPoint> endPoint,
                                                   double& slope, double& intercept)
{
    double deltax = startPoint->_x - endPoint->_x;
    double deltay = startPoint->_y - endPoint->_y;

    slope = deltay / deltax;

    //(h,k) pick any point on the line
    double h = startPoint->_x;
    double k = startPoint->_y;
    //point slope format of line y  = slope * (x - h) + k;

    intercept = ((-1) * h * slope) + k;
    return true;
}

bool TriangulationLineString::FindIntersectWith2Lines2(std::shared_ptr<TriangulationPoint> startPointL1, std::shared_ptr<TriangulationPoint> endPointL1,
                                                        std::shared_ptr<TriangulationPoint> startPointL2, std::shared_ptr<TriangulationPoint> endPointL2,
                                                     double& xintercept, double& yintercept)
{
    double deltax = endPointL1->_x - startPointL1->_x;
    double deltay = endPointL1->_y - startPointL1->_y;
    if(startPointL1->_x > endPointL1->_x)
    {
        //cout<<"swap sign delta"<<endl;
        deltax = (-1.0) * deltax;
        deltay = (-1.0) * deltay;
    }
    double slope = deltay / deltax;// rise/run
    double h = startPointL1->_x;
    double k = startPointL1->_y;
    //point slope format of line y  = slope * (x - h) + k;

    double intercept = ((-1) * h * slope) + k;
    //now formula is y = slope*x + intercept

    double deltax2 = endPointL2->_x - startPointL2->_x;
    double deltay2 = endPointL2->_y - startPointL2->_y;
    if(startPointL2->_x > endPointL2->_x)
    {
        //cout<<"swap sign delta"<<endl;
        deltax2 = (-1.0) * deltax2;
        deltay2 = (-1.0) * deltay2;
    }
    double slope2 = deltay2 / deltax2;// rise/run
    double h2 = startPointL2->_x;
    double k2 = startPointL2->_y;
    //point slope format of line y2  = slope2 * (x2 - h2) + k2;
    //intercept is -h2*slope2 + k2;
    double intercept2 = ((-1) * h2 * slope2) + k2;
    //now formula is y2 = slope2 * x2 + intercept2
    //subtract the x coefficient from the right side to both sides
    xintercept = (intercept2 - intercept)/(slope - slope2);
    yintercept = slope * xintercept + intercept;

    if(yintercept < endPointL1->_y - 50.0 || yintercept > endPointL1->_y + 50.0 ||
            xintercept < endPointL1->_x - 50.0 || xintercept > endPointL1->_x + 50.0)
    {
        using namespace std;
        //std::cout<<"intersectx="<<xintercept<<" intersecty="<<yintercept<<endl;
        //std::cout<<"endfirstlinex="<<endPointL1->_x<<"startsecondlinex="<<startPointL2->_x<<endl;
        //std::cout<<"endfirstliney="<<endPointL1->_y<<"startsecondliney="<<startPointL2->_y<<endl;
        //std::cout<<""<<endl;
    }

    return true;
}
#include "utilities.h"
double TriangulationLineString::GetAngleBetweenLines(std::shared_ptr<TriangulationPoint> startFirstLine, std::shared_ptr<TriangulationPoint> endFirstLine,
                                                      std::shared_ptr<TriangulationPoint> beginSecondLine ,
                                                    std::shared_ptr<TriangulationPoint> endSecondLine)
{
    //use getslope intercept function, then use formula
    double m1 = 0.00;//slope
    double intercept1 = 0.00;
    double m2 = 0.00;//slope
    double intercept2 = 0.00;

    GetSlopeAndIntercept(startFirstLine, endFirstLine, m1, intercept1);
    GetSlopeAndIntercept(beginSecondLine, endSecondLine, m2, intercept2);
    //tan theta = abs((m2 - m1)/1 + m2m1))
    //theta = taninv (abs((m2 - m1)/1 + m2m1)))
    double numDenom = (m2 - m1)/(1 + m2 * m1);
    if(numDenom < 0)
        numDenom *= -1.0;
    double angle = atan(numDenom);
    double degrees = radiansToDegrees(numDenom);
    return degrees;
}

void TriangulationLineString::ConnectSingleOffsetLine(std::shared_ptr<TriangulationPoint> startFirstLine, std::shared_ptr<TriangulationPoint> endFirstLine,
                                                      std::shared_ptr<TriangulationPoint> beginSecondLine ,
                                                    std::shared_ptr<TriangulationPoint> endSecondLine)
{

    double intersectX = 0.00;
    double intersectY = 0.00;
    bool result = FindIntersectWith2Lines2(startFirstLine, endFirstLine, beginSecondLine, endSecondLine, intersectX, intersectY);



    //bool result = FindIntersectWith2Lines(startFirstLine->_x, startFirstLine->_y, endFirstLine->_x, endFirstLine->_y, beginSecondLine->_x, beginSecondLine->_y,
      //                                     endSecondLine->_x, endSecondLine->_y, intersectX, intersectY);
   // LineLineIntersect(startFirstLine->_x, startFirstLine->_y, endFirstLine->_x, endFirstLine->_y, beginSecondLine->_x, beginSecondLine->_y, endSecondLine->_x, endSecondLine->_y, intersectX, intersectY);
//    if(intersectY < endFirstLine->_y - 20.0 || intersectY > endFirstLine->_y + 20.0 ||
//            intersectX < endFirstLine->_x - 20.0 || intersectX > endFirstLine->_x + 20.0)
//    {
//        using namespace std;
//        std::cout<<"intersectx="<<intersectX<<" intersecty="<<intersectY<<endl;
//        std::cout<<"endfirstlinex="<<endFirstLine->_x<<"startsecondlinex="<<beginSecondLine->_x<<endl;
//        std::cout<<"endfirstliney="<<endFirstLine->_y<<"startsecondliney="<<beginSecondLine->_y<<endl;

//    }

    double angleBetweenLines = GetAngleBetweenLines(startFirstLine, endFirstLine, beginSecondLine, endSecondLine);

    if((angleBetweenLines < 30.0 && angleBetweenLines > -30.0) || (angleBetweenLines < 200.0 && angleBetweenLines > 160.0))
    {
        //lines are pretty much parallel
       // endFirstLine->_x = intersectX;// beginSecondLine->_x;
       // endFirstLine->_y = intersectY;//beginSecondLine->_y;
        beginSecondLine->_x = endFirstLine->_x;//intersectX;
        beginSecondLine->_y = endFirstLine->_y;//intersectY;
    }

   // if(result)
    else
    {
        endFirstLine->_x = intersectX;// beginSecondLine->_x;
        endFirstLine->_y = intersectY;//beginSecondLine->_y;
        beginSecondLine->_x = endFirstLine->_x;//intersectX;
        beginSecondLine->_y = endFirstLine->_y;//intersectY;
    }

}

void TriangulationLineString::ConnectOffsetLines()
{
    //go through vector of linesegments, connect up endpoints of left and right offsets.
    if(_lineSegments.size() > 1)
    {
        for(int i = 0 ; i < _lineSegments.size() - 1; i++)
        {
            std::shared_ptr<TriangulationLineSegment> lsfirst = _lineSegments[i];
            std::shared_ptr<TriangulationLineSegment> lssecond = _lineSegments[i + 1];
            {
                std::shared_ptr<TriangulationPoint> startFirstLineLt = lsfirst->_startPointLeftOffset;
                std::shared_ptr<TriangulationPoint> endFirstLineLt = lsfirst->_endPointLeftOffset;

                std::shared_ptr<TriangulationPoint> beginSecondLineLt = lssecond->_startPointLeftOffset;
                std::shared_ptr<TriangulationPoint> endSecondLineLt = lssecond->_endPointLeftOffset;

                ConnectSingleOffsetLine(startFirstLineLt, endFirstLineLt, beginSecondLineLt, endSecondLineLt);
            }
            {
                std::shared_ptr<TriangulationPoint> startFirstLineRt = lsfirst->_startPointRightOffset;
                std::shared_ptr<TriangulationPoint> endFirstLineRt = lsfirst->_endPointRightOffset;

                std::shared_ptr<TriangulationPoint> beginSecondLineRt = lssecond->_startPointRightOffset;
                std::shared_ptr<TriangulationPoint> endSecondLineRt = lssecond->_endPointRightOffset;

               ConnectSingleOffsetLine(startFirstLineRt, endFirstLineRt, beginSecondLineRt, endSecondLineRt);
            }
        }
    }
    else
    {
        //cout<<"linesegment size=1"<<endl;
    }
}
bool TriangulationLineSegment::AddOffsets(double distance)
{
    double osxstartrt;
    double osystartrt;
    double osxendrt;
    double osyendrt;
    OffsetLine(true, distance, _startPoint->_x, _startPoint->_y, _endPoint->_x, _endPoint->_y, osxstartrt, osystartrt, osxendrt, osyendrt);

    double osxstartlt;
    double osystartlt;
    double osxendlt;
    double osyendlt;
    OffsetLine(false, distance, _startPoint->_x, _startPoint->_y, _endPoint->_x, _endPoint->_y, osxstartlt, osystartlt, osxendlt, osyendlt);

    //right offsets
    _startPointRightOffset.reset(new TriangulationPoint(osxstartrt, osystartrt, _startPoint->_z));
    _endPointRightOffset.reset(new TriangulationPoint(osxendrt, osyendrt, _endPoint->_z));

    //left offsets
    _startPointLeftOffset.reset(new TriangulationPoint(osxstartlt, osystartlt, _startPoint->_z));
    _endPointLeftOffset.reset(new TriangulationPoint(osxendlt, osyendlt, _endPoint->_z));
    //ConnectOffsetLines();
    return true;
}

//bool StreetLineManager::ProcessStreetLines(LineString segments)
//{
//    return true;
//}
//double StreetLineManager::DegreesToRadians(double degrees)
//{
//    return degrees * 0.01745329252;
//}
//double StreetLineManager::RadiansToDegrees(double radians)
//{
//    return radians * 57.2958;
//}
bool TriangulationLineSegment::OffsetLine(bool rightSide, double offset, double line1x1, double line1y1, double line1x2, double line1y2, double& osx1, double& osy1, double& osx2, double& osy2)
{
    if(line1x1 == line1x2 && line1y1 == line1y2)
        return false;//same point, can't offset

    //right/left is based on a line traveling in the +x direction ie if 1st point has x< 2nd point
    //meaning right offset you would - offset in y direction
    double deltay = line1y2 - line1y1;
    double deltax = line1x2 - line1x1;
    if(deltay == 0.0)//handle divide by zero case
    {
        osx1 = line1x1;
        osx2 = line1x2;
        //offset straight up or down with no x element, horizontal line
        if((rightSide && line1x2 > line1x1) || (!rightSide && line1x2 < line1x1))
        {
            osy1 = line1y1 - offset;
            osy2 = line1y2 - offset;
        }
        else{
            osy1 = line1y1 + offset;
            osy2 = line1y2 + offset;
        }
        return true;
    }
    double alpha = atan(deltax / deltay);
    double xoffset = offset * cos(alpha);
    double yoffset = offset * sin(alpha);

    if((rightSide && line1x2 > line1x1) || (!rightSide && line1x2 < line1x1)
       || (rightSide && line1x2 == line1x1 && line1y2 > line1y1))
    {
        osx1 = line1x1 + xoffset;
        osx2 = line1x2 + xoffset;

        osy1 = line1y1 - yoffset;
        osy2 = line1y2 - yoffset;
    }
    else{
        osx1 = line1x1 - xoffset;
        osx2 = line1x2 - xoffset;

        osy1 = line1y1 + yoffset;
        osy2 = line1y2 + yoffset;
    }

    //double result = atan(30.0);
    //double 0.01745
    return true;//success
}
#include <iostream>
bool TriangulationLineString::FindIntersectWith2Lines(double line1x1, double line1y1, double line1x2, double line1y2, double line2x3, double line2y3, double line2x4, double line2y4,
                                            double& intersectX, double& intersectY)
{
    double xnumerator = (line1x1 * line1y2 - line1y1 * line1x2)*(line2x3 - line2x4) - (line1x1 - line1x2)*(line2x3 * line2y4 - line2y3 * line2x4);
    double xdenominator = (line1x1 - line1x2)*(line2y3 - line2y4) - (line1y1 - line1y2)*(line2x3 - line2x4);
    if(xdenominator != 0)
        intersectX = xnumerator / xdenominator;
    else
        return false;//lines are parallel

    double ynumerator = (line1x1 * line1y2 - line1y1 * line1x2) * (line2y3 - line2y4) - (line1y1 - line1y2) * (line2x3 * line2y4 - line2y3 * line2x4);
    double ydenominator = xdenominator;
    if(xdenominator != 0)
        intersectY = ynumerator / ydenominator;
    else
        std::cout<<"divide by zero";

    //if intersectx is between line1x1 and line1x2, and between line2x3 line2x4 and intersecty is between line1y1 line1y2 and between line2y3 line2y4 then the 2 line segments intersect
    //otherwise the intersect point is outside the line segment/s
    int lineIntersects = 0;
    if(((line1x1 <= intersectX && intersectX <= line1x2) || (line1x2 <= intersectX && intersectX <= line1x1)) && ((line1y1 <= intersectY && intersectY <= line1y2) || (line1y2 <= intersectY && intersectY <= line1y1)))
    {
        // this coordinate lies on the first line
        lineIntersects++;
    }

    if(((line2x3 <= intersectX && intersectX <= line2x4) || (line2x4 <= intersectX && intersectX <= line2x3)) && ((line2y3 <= intersectY && intersectY <= line2y4) || (line2y4 <= intersectY && intersectY <= line2y3)))
    {
        // this coordinate lies on the second line
        lineIntersects++;
    }

    if(lineIntersects >= 2)
        return true;
    else
        return false;
}

//double StreetLineManager::DistancePointToLine(double line1x1, double line1y1, double line1x2, double line1y2, double pointx, double pointy)
//{
//    double deltay = line1y2 - line1y1;
//    double deltax = line1x2 - line1x1;
//    double numerator = (deltay * pointx) - (deltax * pointy) + line1x2*line1y1 - line1y2*line1x1;
//    double absnumerator = abs(numerator);

//    double denominator = sqrt(deltay*deltay + deltax*deltax);
//    //double absval = abs(-2.3);
//    return absnumerator / denominator;
//}

//bool StreetLineManager::isLeft(double line1x1, double line1y1, double line1x2, double line1y2, double pointx, double pointy)
//{

//    //return ((b.X - a.X)*(c.Y - a.Y) - (b.Y - a.Y)*(c.X - a.X)) > 0;
//    return ((line1x2 - line1x1)*(pointy - line1y1) - (line1y2 - line1y1)*(pointx - line1x1)) > 0;
//}
