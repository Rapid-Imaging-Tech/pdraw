#ifndef STREETLINETRIANGULATION
#define STREETLINETRIANGULATION
#include <memory>
#include <vector>
#include <boost/shared_ptr.hpp>
using namespace std;
class TriangulationPoint{
public:
    TriangulationPoint(double x, double y, double z){_x = x; _y = y; _z = z;};
    double _x;
    double _y;
    double _z;
};

class TriangulationLineSegment
{
  //2 points
public:
    TriangulationLineSegment(TriangulationPoint start, TriangulationPoint end):
        _startPoint(new TriangulationPoint(start._x, start._y, start._z)),
        _endPoint(new TriangulationPoint(end._x, end._y, end._z))
    {

    }
    bool AddOffsets(double distance);//add the left and right offset to the centerline
    std::shared_ptr<TriangulationPoint> _startPoint;
    std::shared_ptr<TriangulationPoint> _endPoint;
    std::shared_ptr<TriangulationPoint> _startPointRightOffset;
    std::shared_ptr<TriangulationPoint> _endPointRightOffset;
    std::shared_ptr<TriangulationPoint> _startPointLeftOffset;
    std::shared_ptr<TriangulationPoint> _endPointLeftOffset;
    bool OffsetLine(bool rightSide, double offset, double line1x1, double line1y1, double line1x2, double line1y2, double& osx1, double& osy1, double& osx2, double& osy2);

};

class TriangulationLineString
{
  //vector of line segments signifying a street line
public:
    double GetAngleBetweenLines(std::shared_ptr<TriangulationPoint> startFirstLine, std::shared_ptr<TriangulationPoint> endFirstLine,
                                                          std::shared_ptr<TriangulationPoint> beginSecondLine ,
                                                        std::shared_ptr<TriangulationPoint> endSecondLine);

    bool GetSlopeAndIntercept(std::shared_ptr<TriangulationPoint> startPoint,
                                                       std::shared_ptr<TriangulationPoint> endPoint,
                                                       double& slope, double& intercept);
    //bool GetSlopeAndIntercept(std::shared_ptr<TriangulationLineSegment> seg1, double& slope, double& intercept);
    bool FindIntersectWith2Lines2(std::shared_ptr<TriangulationPoint> startPointL1, std::shared_ptr<TriangulationPoint> endPointL1,
                                                            std::shared_ptr<TriangulationPoint> startPointL2, std::shared_ptr<TriangulationPoint> endPointL2,
                                double& xintercept, double& yintercept);
    void ConnectSingleOffsetLine(std::shared_ptr<TriangulationPoint> startFirstLineLt, std::shared_ptr<TriangulationPoint> endFirstLineLt,
                                 std::shared_ptr<TriangulationPoint> beginSecondLineLt,
                                 std::shared_ptr<TriangulationPoint> endSecondLineLt);
    vector<std::shared_ptr<TriangulationLineSegment> > _lineSegments;//all LInesegments should be in order ie the _endpoint of 0 should be the _startPOint of 1
    void ConnectOffsetLines();
    bool FindIntersectWith2Lines(double line1x1, double line1y1, double line1x2, double line1y2, double line2x3, double line2y3, double line2x4, double line2y4,
                                 double& intersectX, double& intersectY);
};
#endif // STREETLINETRIANGULATION

