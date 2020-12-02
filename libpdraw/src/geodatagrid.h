
#ifndef GEODATAGRID
#define GEODATAGRID
#include <geodatacachesettings.h>
#include <geokmlrow.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <utilities.h>
#include <SViewData.h>
#include <LoggableObject.h>
#include <boost/thread.hpp>
#include <DemTileCollection.h>
//#include <geotileservergl.h>
class GeoDataGrid : public LoggableConfigurableObject
{
    enum ROWTYPE{KML, ELEV};
public:
    bool TestForGridChange();
    bool _gridLoaded;
   // boost::asio::io_service io_service_;
   // geotileservergl* tileserver_;
    bool GetElevationDataReady(){return _elevationDataReady;};
    GeoDataGrid();
    ~GeoDataGrid();

    int GetTerrainIntercept(double lat, double lon, double alt, double p, double h, double r, double& lt, double& ln, double& al, double& range);
    void GetStarePoint(boost::shared_ptr<SViewData> viewData, double& range);//, double& lat, double& lon, double& elevation);
    void GetGridExtentsLatLon(double& northLat, double& southLat, double& eastLon, double& westLon);
    int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);
    void FillInitialGrid(boost::shared_ptr<SViewData> viewData);
    boost::shared_ptr<GeoTile>  LoadNextElement(int currentLOD);
    //void GrabDataForStarePoint(double starePointLat, double starePointLon, std::vector<boost::shared_ptr<Kml::Point> >& points,
      //                    std::vector<boost::shared_ptr<Kml::Vectors> >& streets, std::vector<boost::shared_ptr<Kml::StreetLabel> >& streetLabels, int lod );

    int GrabDataForROI(double maxLat, double maxLon, double minLat, double minLon, std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > >& points,
                          std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > >& streets,
                        std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > >& streetLabels, int lod);

    bool RequestAreaToLoad(double maxLat, double maxLon, double minLat, double minLon, int lod);

    void UpdateViewAreaBorder(boost::shared_ptr<SViewData> viewData);
    bool CheckToShiftGrid();//if the view area reaches out of gridborder, we need to add more tiles, and delete on the other end
    void RePrioritizeTilesToLoad(boost::shared_ptr<SViewData> viewData, std::vector<boost::shared_ptr<GeoTile> >& vecUnsorted);//prioritize by distance of tile from starepoint
    void PrioritizeTilesToLoad(boost::shared_ptr<SViewData> viewData, double minLat, double minLon, double maxLat, double maxLon);
    //void PrioritizeTilesToLoad(std::vector<boost::shared_ptr<GeoTile> > tilesInRoi);//prioritize by tiles in current roi
    bool GetElevationAtLocation(double lat, double lon, double &elevation);
private:
    //double _tileSize;
    void GiveRoiTilesTopPriority(std::vector<boost::shared_ptr<GeoTile> >& priorityQueue, std::vector<std::vector<boost::shared_ptr<GeoTile> > > tileRowsInRoi);
    void SwapDemtiles();
    boost::shared_ptr<DemTileCollection> GetCurrentDemTileCollection();
    bool _elevationDataReady;
    void RefreshDemTiles();
    boost::shared_ptr<DemTileCollection> _demTileCollection;
    //fill this one up while we're grabbing data from _demTileCollection
    boost::shared_ptr<DemTileCollection> _demTileCollectionBackBuff;

    void GetViewAreaExtentsLatLon(double& northLat, double& southLat, double& eastLon, double& westLon);
    void SetViewAreaExtents(double northLat, double southLat, double eastLon, double westLon);
    void SetGridExtents(int northTileRowy, int southTileRowy, int eastTileColumnx, int westTileColumnx);

    //MEMBERS
    boost::shared_ptr<SViewData> _latestViewData;


    //TODO: THESE SHOULD BE THREAD SAFE
    std::vector<boost::shared_ptr<GeoTile> > _tilesNotLoadedOrPrioritizedQueueHIGHRES;
    std::vector<boost::shared_ptr<GeoTile> > _tilesSetToLoadPriorityQueueHIGHRES;

    //std::vector<boost::shared_ptr<GeoTile> > _tilesNotLoadedOrPrioritizedQueueLOWRES;
    //std::vector<boost::shared_ptr<GeoTile> > _tilesSetToLoadPriorityQueueLOWRES;

    //std::vector<boost::shared_ptr<GeoTile> > _tilesNotLoadedOrPrioritizedQueueMEDRES;
    //std::vector<boost::shared_ptr<GeoTile> > _tilesSetToLoadPriorityQueueMEDRES;


    boost::mutex _mutexGridBorder;
    boost::mutex _mutexViewAreaBorder;
    boost::mutex _demTileSwapMutex;
    boost::mutex _mutexPrioritizer;
    //north and south borders are latitude east and west are in longitude
    double _northBorderViewAreaLat;
    double _southBorderViewAreaLat;
    double _eastBorderViewAreaLon;
    double _westBorderViewAreaLon;

    int _northGridRowY;
    //the y of the tile row consisiting of south row, actual grid line is Y + 1 converted to lat,
    //since y is the NorthWest corner of tile
    int _southGridRowY;

    //the x of the tile row consisiting of east column, actual grid line is x + 1 converted to lon,
    //since x,y is the NorthWest corner of tile
    int _eastGridColumnX;
    int _westGridColumnX;

    int _gridDimensionNS;
    int _gridDimensionEW;

    std::vector<boost::shared_ptr<GeoRow> > _vecGridRowsLOD9;
    std::vector<boost::shared_ptr<GeoRow> > _vecGridRowsLOD8;
    std::vector<boost::shared_ptr<GeoRow> > _vecGridRowsLOD12;
    //std::vector<boost::shared_ptr<GeoRow> > _vecGridRowsElev;

    //PRIVATE METHODS
    void LogStateOfGrid();
    void PrioritizeTilesToLoad(boost::shared_ptr<SViewData> viewData, std::vector<boost::shared_ptr<GeoTile> >& vecUnsorted,
                                            std::vector<boost::shared_ptr<GeoTile> >& vecToSort);

   // virtual void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
     virtual void SetUserSettings();


    void AddRowNorth(int lod, std::vector<boost::shared_ptr<GeoTile> >& tilesNotLoadedQueueOrPrioiritized,
                     std::vector<boost::shared_ptr<GeoRow> >& vecRow, ROWTYPE type);//todo: later make type an enum?

    void AddRowSouth(int lod, std::vector<boost::shared_ptr<GeoTile> >& tilesNotLoadedQueueOrPrioiritized,
                                                   std::vector<boost::shared_ptr<GeoRow> >& vecRow, ROWTYPE type);


    bool AddColumnEast(int lod, std::vector<boost::shared_ptr<GeoTile> >& tilesNotLoadedQueueOrPrioiritized,
                       const std::vector<boost::shared_ptr<GeoRow> >& vecRow);
    bool AddColumnWest(int lod, std::vector<boost::shared_ptr<GeoTile> >& tilesNotLoadedQueueOrPrioiritized,
                       const std::vector<boost::shared_ptr<GeoRow> >& vecRow);

    void RemoveEastColumn(std::vector<boost::shared_ptr<GeoRow> >& vecRow);
    void RemoveWestColumn(std::vector<boost::shared_ptr<GeoRow> >& vecRow);
    void RemoveSouthRow(std::vector<boost::shared_ptr<GeoRow> >& vecRow);

    void RemoveNorthRow(std::vector<boost::shared_ptr<GeoRow> >& vecRow);

    std::vector<boost::shared_ptr<GeoRow> > GatherRowsForRoi( const std::vector<boost::shared_ptr<GeoRow> >& vecRows,
                                                                                   double maxLat, double maxLon, double minLat, double minLon, bool fullGrid);
};
#endif // GEODATAGRID

