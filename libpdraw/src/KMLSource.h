#include <string>
class KMLSource
{
public:
	virtual std::string GetKML(double minLat, double minLon, double maxLat, double maxLon) = 0;
	virtual void Init() = 0;
};