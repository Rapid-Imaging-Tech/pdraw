
#pragma once
#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>
#include "SViewData.h"
typedef unsigned int U32;
enum DICTTYPE{DICT2, DICT3, DICTUNKNOWN};
enum EntryName { TIME, MissionID, PlatformTailNumber, HEADING, PITCH, ROLL, PlatformTrueAirspeed, PlatformIndicatedAirspeed, PlatformDesignation, 
					ImageSourceSensor, ImageCoordinateSystem, LATITUDE, LONGITUDE, ALTITUDE, SensorHorizontalFoV, VFOV,CAM_BEARING, CAM_DEPRESSION,
					CAM_TWIST, SlantRange, TargetWidth, FrameCenterLatitude, FrameCenterLongitude, FrameCenterElevation, CornerLatitudePoint1,
					CornerLongitudePoint1 ,CornerLatitudePoint2 ,CornerLongitudePoint2, CornerLatitudePoint3, CornerLongitudePoint3, 
					CornerLatitudePoint4 ,CornerLongitudePoint4 ,IcingDetected ,WindDirection ,WindSpeed ,StaticPressure, DensityAltitude,
					OutsideAirTemperature };
enum DataType{Uint32, Int32, Uint16, Int16, Uint8, Int8, Uint64, ISO7};
class DictionaryEntry
{
 public:
	EntryName _name;
	unsigned char _key;
	double _minRange;
	double _maxRange;
	DataType _type;
};

class KLVItem
{
public:
	unsigned char _key;
	unsigned char _length;
	unsigned char _value[128];
};
class KLVCollection
{
public:
	//_mapDictionary
	void ExpandValues()
	{
		//go through the vector, 

	};
	std::vector<boost::shared_ptr<KLVItem> > _klvItems;
};

//class SViewData
//{
//public:
//	double _latitude;
//	double _longitude;
//	double _altitude;
//};
class Metadata
{
public:
	void Init();
	void ParsePacket(const unsigned char* buffer, int length);
	void GetData(boost::shared_ptr<SViewData> sviewdata);
	Metadata();
	~Metadata();
private:
	DICTTYPE _dictType;
	void ParseDict2(const unsigned char* buffer, int length);
	void ParseDict3(const unsigned char* buffer, int length);

	DICTTYPE FindDictType(const unsigned char* buffer);
	KLVCollection _klvCollection;
	std::map<unsigned char, boost::shared_ptr<DictionaryEntry> > _mapDictionary2;
	std::map<unsigned char, boost::shared_ptr<DictionaryEntry> > _mapDictionary3;
	void SetupDictionary2();
	void SetupDictionary3();
};
