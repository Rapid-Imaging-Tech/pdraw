#include <string>
#include <vector>
class PlaceMark
{
public:
	PlaceMark();
	double GetLatitude(){
		return _latitude;
	};
	double GetLongitude(){
		return _longitude;
	};

	void SetLatitude(double val){
		_latitude = val;
	};
	void SetLongitude(double val){
		_longitude =  val;
	};

private:
	double _latitude;
	double _longitude;
};

class KMLParser
{
public:
	std::vector<PlaceMark> ParseKML(const std::string& filename);
};

