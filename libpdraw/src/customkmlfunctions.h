#ifndef CUSTOMKMLFUNCTIONS
#define CUSTOMKMLFUNCTIONS
#include "KMLEntities.h"
using namespace Kml;
namespace Kml
{
    class CustomKmlFunctions
    {
    public:
        CustomKmlFunctions();
        ~CustomKmlFunctions();
        void TestKmlOutput(std::vector<coordinate> coords);
        //take file and parse into placemark_vector_t
        //return the number of placemarks found, or size of vector
        int ParseFile(const char* filename, int uid, std::vector<std::string>& newIcons);//std::string filename);
        int ParseXMLData(std::string xmlData);//std::string filename);
        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > GetPoints(std::string filename);
        boost::shared_ptr<std::vector<boost::shared_ptr<Vectors> > > GetStreets(std::string filename);
        boost::shared_ptr<std::vector<boost::shared_ptr<StreetLabel> > > GetStreetLabels();
        boost::shared_ptr<std::vector<boost::shared_ptr<Vectors> > > GetPolygons();
    private:
        std::string GetKmlFromFile(std::string filename);
        std::string _kmlFile;
        double GetValueDouble(Kml::TYPE type, int index);
        //placemark_vector_t _placemark_vector;
        std::vector<PlacemarkWithPath> _placemarksWithPath;
    };
}
#endif // CUSTOMKMLFUNCTIONS

//#ifndef CUSTOMKMLFUNCTIONS
//#define CUSTOMKMLFUNCTIONS
//#include <KMLEntities.h>
//namespace Kml
//{
//    class CustomKmlFunctions
//    {
//      public:
//        CustomKmlFunctions();
//        ~CustomKmlFunctions();
//        void TestKmlOutput(std::vector<coordinate> coords);
//        //take file and parse into placemark_vector_t
//        //return the number of placemarks found, or size of vector
//        int ParseFile(const char* filename, int uid, std::vector<std::string>& newIcons);//std::string filename);
//        int ParseXMLData(std::string xmlData);//std::string filename);
//        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > GetPoints(std::string filename);
//        boost::shared_ptr<std::vector<boost::shared_ptr<Vectors> > > GetStreets(std::string filename);
//        boost::shared_ptr<std::vector<boost::shared_ptr<StreetLabel> > > GetStreetLabels();
//        boost::shared_ptr<std::vector<boost::shared_ptr<Vectors> > > GetPolygons();
//      private:
//        std::string GetKmlFromFile(std::string filename);
//        std::string _kmlFile;
//        double GetValueDouble(Kml::TYPE type, int index);
//        //placemark_vector_t _placemark_vector;
//        std::vector<PlacemarkWithPath> _placemarksWithPath;
//    };
//}
//#endif // CUSTOMKMLFUNCTIONS
