#ifndef GEOVECTORTILE_H
#define GEOVECTORTILE_H
#include <geotile.h>
#include <boost/asio.hpp>
#include <geotileservergl.h>
//#include <vector_tile.pb.h>
#include <map>
class geovectortile : public GeoTile
{
public:
    std::string _pbfFileName;
    std::string _tempPath;
    void fillIconMap();
    std::map<std::string, std::string> _poiIconMap;//set icon hrefs from names that are used in vector tiles
    bool isCity(std::string classification);
    //int _vecTileX;
    //int _vecTileY;
    //int _vecTileZoomLevel;
    void parseVectorTile();

    //std::string findStringForKey(std::string keyString, vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat);
    //void addPlace(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry);
    //void addPoi(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry);
    geovectortile(int tilex, int tiley, int zoomlevel, int column, int row,
                                 int lod);
        ~geovectortile();
        int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);
        virtual bool LoadData();

        //for kml only
        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > GetPolygons();
        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > GetStreets();
        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > GetStreetLabels();
        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > GetPoints();
        //void GrabDataForStarePoint(double starePointLat, double starePointLon, std::vector<boost::shared_ptr<Kml::Point> >& points,
          //                    std::vector<boost::shared_ptr<Kml::Vectors> >& streets, std::vector<boost::shared_ptr<Kml::StreetLabel> >& streetLabels );

        void GrabDataForROI(double maxLat, double maxLon, double minLat, double minLon, std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > >& points,
                              std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > >& streets,
                            std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > >& streetLabels );

         boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > _points;
         boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > _points9;
         boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > _streets;//vec/ptr/point
         //std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Point> > >
         boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > _streetLabels;

        int _lod;
        int _numEntities;
        int _numEntities9;//this is only needed for lod12, which uses lod9 for placemarks otherwise way too many and slowing down cpu

        //std::string _kml;//todo: remove later?
        //std::string _kml9;

        std::string _vectilePort;
        std::string _vectileURL;
        void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
private:
        //double _lowerLat;
        //double _lowerLon;
        //double _upperLat;
        //double _upperLon;
        //double _latCenter;
        //double _lonCenter;
};

#endif // GEOVECTORTILE_H
