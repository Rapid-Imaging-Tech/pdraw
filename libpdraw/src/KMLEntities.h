#pragma once
#include <algorithm>
#include <iostream>
#include <string>
//#include "utilities.h"
#include "kml/dom.h"

#include "kml/engine.h"
#include "kml/base/file.h"
#include <vector>
#include "StatusLogger.h"
#include <cmath>
#if defined(__APPLE__)
#include "boost/shared_ptr.hpp"
#else
#include </usr/local/include/boost/shared_ptr.hpp>
#endif
#include "LoggableObject.h"
//#include <utilities.h>
// Declare the types and functions defined in this file.

using kmldom::ContainerPtr;
using kmldom::ElementPtr;
using kmldom::FeaturePtr;
using kmldom::KmlPtr;
using kmldom::PlacemarkPtr;
using kmlengine::KmlFile;
using kmlengine::KmlFilePtr;
using kmlengine::KmzFile;
using kmlengine::KmzFilePtr;
using std::cout;
using std::endl;
//todo: make this a struct which attaches placemark with it's folder path from kml file
#include "kmlpathdetails.h"

typedef std::vector<PlacemarkPtr> placemark_vector_t;
extern std::string AppDataDir();
struct coordinate
{
	
	double latitude;
	double longitude;
	std::string name;
};
FeaturePtr GetKmlFileRootFeature(const char* kmlfile, int uid, std::vector<std::string>& newIcons);


void CreateDirectoryTree(std::string path);



namespace Kml
{
	enum TYPE{LATITUDE, LONGITUDE, ELEVATION};
	class Point
	{
	public:
         boost::shared_ptr<KmlPathDetails> _kmlPathDetails;
		Point():
		  _latitude(0.00),
		  _longitude(0.00),
		  _elevation(0.00),
		  _x(0.00),
		  _y(0.00),
		  _z(0.00),
		  _name("")
        {
            _kmlPathDetails.reset(new KmlPathDetails);
        }
        void SetCustomHref(std::string customName){_customHref = customName;};
        std::string GetCustomHref(){return _customHref;};
		~Point(){};
		double GetLat(){return _latitude;};
		double GetLon(){return _longitude;};
		double GetAlt(){return _elevation;};
		std::string GetName(){return _name;};
		void SetLat(double val){ _latitude = val ; } ;
		void SetLon(double val){ _longitude = val ; } ;
		void SetAlt(double val){ _elevation = val ; } ;
		void SetName(std::string val){_name = val;};

		void SetX(double x){_x = x;};
		void SetY(double y){_y = y;};
		void SetZ(double z){_z = z;};
		double GetX(){return _x;};
		double GetY(){return _y;};
		double GetZ(){return _z;};

        void SetStyle(kmldom::StylePtr style)
        {
            _style = style;
        };
		//void SetStyle(unsigned int abgr){_abgrcolor = abgr; } ;
		std::string GetIconHref()
		{
			if(_style->has_iconstyle())
			{
				kmldom::IconStylePtr p = _style->get_iconstyle();
				kmldom::IconStyleIconPtr p2 = p->get_icon();
				std::string strHref = p2->get_href();
				return strHref;
			}
			else
			{
				std::string str("");
				return str;
			}
		};

		kmlbase::Color32 GetColor()
		{
			if(_style && _style->has_labelstyle())
				return _style->get_labelstyle()->get_color();
			else
			{
				kmlbase::Color32 clr;
				clr.set_color_abgr(0xffffffff);
				return clr;
			} 
		};
	private:
        std::string _customHref;
		kmldom::StylePtr _style;

		
		std::string _name;
		double _latitude;
		double _longitude;
		double _elevation;
		double _x;
		double _y;
		double _z;
	};

	class StreetLabel
	{
	public:
		StreetLabel():
		_elevation(0.00),
			_isAlternateLabel(false)
		{};
		~StreetLabel(){};
		kmlbase::Color32 GetColor()
		{
			if(_style && _style->has_labelstyle())
				return _style->get_labelstyle()->get_color();
			else
			{
				kmlbase::Color32 clr;
				clr.set_color_abgr(0xffffffff);
				return clr;
			}
		};
		void SetIsAlternativeName(bool val){_isAlternateLabel = true;};
		bool IsAlternativeName(){return _isAlternateLabel; } ;
		void SetStyle(kmldom::StylePtr style){_style = style;};
		void SetLabel(std::string val){_label = val;};
		void SetDirection(std::vector<boost::shared_ptr<Kml::Point> > val){_labelDirection = val;};
		void SetLabelPoint(boost::shared_ptr<Point> val){_labelPoint = val; } ;
		void SetLabelElevation(double el){_elevation = el;};
		std::string GetLabel(){return _label;};
		std::vector<boost::shared_ptr<Kml::Point> > GetDirection(){return _labelDirection;};
		boost::shared_ptr<Point> GetLabelPoint(){return _labelPoint; } ;
		double GetLabelElevation(){return _elevation; } ;
	private:
		bool _isAlternateLabel;//is this a secondary name for the road? ie is there a shield too?
		kmldom::StylePtr _style;
		//StreetVectors _streetVectors;
		std::vector<boost::shared_ptr<Point> > _labelDirection;
		boost::shared_ptr<Point> _labelPoint;
		std::string _label;
		double _elevation;
	};

//#include <math.h>
class Vector2D{
public:
    Vector2D(float deltax, float deltay):
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
	class Vectors//can be either street vectors or polygon vectors
	{
	 public:
        bool _hide;
        bool _delete;
        boost::shared_ptr<KmlPathDetails> _kmlPathDetails;
        bool GetIsPolygon(){return _isPolygon;};
        void SetIsPolygon(bool val){_isPolygon = val;};
		 int GetSize(){return _streetVectors.size(); } ;
		 kmlbase::Color32 GetColor()
		 {
			 if(_style && _style->has_linestyle())
				return _style->get_linestyle()->get_color();
			 else
			{
				kmlbase::Color32 clr;
				clr.set_color_abgr(0xffffffff);
				return clr;
			} 
		 };
		double GetLineWidth()
		{
			if(_style && _style->has_linestyle())
				return _style->get_linestyle()->get_width();
			else
			{
				return 2.0;
			}
		};
		void SetStyle(kmldom::StylePtr style){_style = style;};

		void SetLabel(std::string val){_label = val;};
        void SetColor(kmlbase::Color32 val)
        {
            kmldom::LineStylePtr lstyle;
            lstyle->set_color(val);

            _style->set_linestyle(lstyle);
        };
		std::string GetLabel(){return _label;};
		Vectors():
        _hasPoint(false),
          _isPolygon(false),
          _hide(false),
          _delete(false)
        {
            _kmlPathDetails.reset(new KmlPathDetails);
        };
		~Vectors(){};
		std::vector<boost::shared_ptr<Point> > GetStreetVectors(){return _streetVectors; };
		void SetStreetVectors(std::vector<boost::shared_ptr<Point> > val){_streetVectors = val;};
        float DistanceBetweenPoints(boost::shared_ptr<Point> p1, boost::shared_ptr<Point> p2)
        {
            //close enough approx to get midpoint of vector points
            double x1 = p1->GetX();
            double y1 = p1->GetY();
            double x2 = p2->GetX();
            double y2 = p2->GetY();
            
            float a = x1 - x2;
            float b = y1 - y2;
            float c = sqrt(a*a + b*b);
            //Point pResult(0.0f, 0.0f);
            return c;
        }
        float CalculateStreetLength()
        {
            //TODO WILL HAVE TO ACCOUNT FOR MANY STREET VECTORS EVENTUALLY, RIGHT NOW JUST 1 LINE
            float totalDistance = 0.0f;
            for(int i = 0 ; i < _streetVectors.size() - 1 ; i++)
            {
                boost::shared_ptr<Point> p0 = _streetVectors[i];
                boost::shared_ptr<Point> p1 = _streetVectors[i + 1];
                totalDistance += DistanceBetweenPoints(p0, p1);// always should be positive since we square everything in the diff function
            }
            return totalDistance;
        }
        double _midPointx = 0.0;
        double _midPointy = 0.0;
        double _midPointz = 0.0;
        void  FindMidPoint(double& x, double& y, double& z){
//            _midPointx = 0.0f;
//            _midPointy = 0.0f;
//            _midPointz = 0.0f;
            //iterate through all StreetVectors points, find midpoint
            float streetLength = CalculateStreetLength();
            float midPoint = streetLength / 2.0f;
            float currentPosition = midPoint;
            bool calculationComplete = false;
            //TODO ALSO PRORATE OUT ELEVATION BETWEEN THE 2 POINTS USING Z'S ON BOTH ENDS
            for(int i = 0 ; i < _streetVectors.size() - 1 && calculationComplete == false; i++){
                boost::shared_ptr<Point> p0 = _streetVectors[i];
                boost::shared_ptr<Point> p1 = _streetVectors[i+1];
                float dist = DistanceBetweenPoints(p0, p1);
                currentPosition -= dist;
                if(currentPosition < 0){
                    //the midpoint is somewhere in this segment
                    //create unit vector pointing from p0 to p1
                    //multiply by length to go
                    //then add deltax from p0.x and deltay to p0.y to
                    //get midpoint
                    float distFromP0 = dist + currentPosition;
                    float deltax = p1->GetX() - p0->GetX();
                    float deltay = p1->GetY() - p0->GetY();
                    float deltaz = p1->GetZ() - p0->GetZ();
                    float percent = distFromP0 / dist;
                    _midPointz = p0->GetZ() + percent * deltaz;
                    Vector2D vectorBetweenPoints(deltax, deltay);
                    vectorBetweenPoints.Normalize();
                    vectorBetweenPoints.Multiply(distFromP0);
                    _midPointx = p0->GetX() + vectorBetweenPoints.GetDeltaX();
                    _midPointy = p0->GetY() + vectorBetweenPoints.GetDeltaY();
                    calculationComplete = true;
                    
                }
            }
            x = _midPointx;
            y = _midPointy;
            z = _midPointz;
            //Point midPoint(midPointx, midPointy);
            //return p;
        }
	 private:
        bool _isPolygon;
		kmldom::StylePtr _style;
		std::string _label;
		std::vector<boost::shared_ptr<Point> > _streetVectors;
		bool _hasPoint;
	};
}


//class PointWithPath{
//public:
//    PointWithPath()
//    {
//        _point.reset(new Kml::Point);
//        _filterItems.reset(new KmlFilterItems);
//    }
//    boost::shared_ptr<Kml::Point> _point;
//    boost::shared_ptr<KmlFilterItems> _filterItems;
//};

class PlacemarkWithPath{
public:
    PlacemarkWithPath()
    {
        _kmlPathDetails.reset(new KmlPathDetails);
    }
    ~PlacemarkWithPath(){}
    boost::shared_ptr<KmlPathDetails> _kmlPathDetails;
    PlacemarkPtr _placeMarkPtr;
};

void SavePlacemarks(const FeaturePtr& feature,
                          // placemark_vector_t* placemarks, std::string filename);
                     std::vector<PlacemarkWithPath>& placemarks, std::string filename, std::vector<std::string> path);
// This function object is used by STL sort() to alphabetize Placemarks
// by <name>.
struct ComparePlacemarks
  : public
      //std::binary_function<const PlacemarkPtr&, const PlacemarkPtr&, bool>
        std::binary_function<const PlacemarkWithPath&, const PlacemarkWithPath&, bool>
{
  bool operator()(const PlacemarkWithPath& a, const PlacemarkWithPath& b)
  {
      return a._kmlPathDetails->_element < b._kmlPathDetails->_element;
    //return a->get_name() < b->get_name();
  }
};
