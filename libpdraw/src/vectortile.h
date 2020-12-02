#ifndef RAVECTORTILE_H
#define RAVECTORTILE_H
#if !defined(__APPLE__)
#include <QOpenGLBuffer>
#include <qopenglshaderprogram.h>
#include <QOpenGLTexture>
#endif
#include <LFDefinitionsRA.h>
#include <utilities.h>

#include <boost/shared_ptr.hpp>
#include <KMLEntities.h>

#include <boost/thread.hpp>
#include <UserSettingsData.h>
#include <string>
#include "elevationmanagerlocal.h"
#include "symbol.h"
#include "boost/shared_array.hpp"
#include "coordinateconverter.h"
#include "vector_tile.pb.h"
#include "openglfont.h"
#include "tile.h"
#include "LFClient.h"
#include "LoggableObject.h"
#include "pathway.h"
#include "streetlinemanager.hpp"
class VectorTile : public tile,  public LoggableConfigurableObject
{
private:
    void AssignBillboardSymbolLevel(boost::shared_ptr<symbolCartesianPoint>& cartesianPoint, boost::shared_ptr<Kml::Vectors> vecs);
    boost::shared_ptr<symbol> Assign1xBillboardSymbol(boost::shared_ptr<Kml::Vectors> vecs, std::string name, POIENUM billboardLevel);
    //void AssignBillboardSymbols(boost::shared_ptr<Kml::Vectors> vecStreets, std::string name, POIENUM billboardLevel);
    boost::shared_ptr<iconInfo> addStreetShield(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry);
    void SetUniformBufferBindingPoint(GLuint bindingPoint);
    int _oldBindingPoint;
    GLuint _uniformBuffer;
    int _currentUBOffsetIndices;
    bool _dumpStreetVecData;
    ofstream _vectorFile;
    int _currentUBStIndices;
    int _currentUBIndicesIndices;
    int _currentUBOffsetStVec;
    void AssignElevationsAndXYZ(boost::shared_ptr<Kml::Vectors> vecs);
    unsigned char* _uniformBufferData;
    unsigned int _autoScaleOffsetSize;
    unsigned int _autoScaleOffsetStVecs;
    unsigned int _autoScaleOffsetIndices;
    
    unsigned int _autoScaleStVecStride;
    unsigned int _autoScaleIndicesStride;
    
    void addTransNameBillboard(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry);
    StreetLineManager _streetLineManager;
    StreetLineManager _streetLineManagerCollar;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _vecStreetLinesMinor;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _vecStreetLinesService;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _vecStreetLinesSecondary;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _vecStreetLinesTertiary;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _vecStreetLinesRail;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _vecStreetLinesTrunk;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _vecStreetLinesPrimary;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _vecStreetLinesMotorway;

    void addStreetLine(vector_tile::Tile_Layer const& layer,
                                              vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry);

    double _streetLabelCutoff1;
    double _streetLabelCutoff2;
    double _streetLabelCutoff3;
    double _streetLabelCutoff4;
    double _currentStreetCutoff;

    double _streetLabelScale1;
    double _streetLabelScale2;
    double _streetLabelScale3;
    double _streetLabelScale4;
    double _streetLabelScale5;
#if defined(__APPLE__)
    GLuint _indexBufStreetLabels;
    GLuint _arrayBufStreetLabels;
#else
    QOpenGLBuffer _indexBufStreetLabels;
    QOpenGLBuffer _arrayBufStreetLabels;
#endif
    boost::shared_ptr<std::vector<float> > _streetLabelVertices;
    boost::shared_ptr<std::vector<unsigned int> > _streetLabelIndices;
    std::vector<float> _streetLabelVertices2;
    void AssignElevations(boost::shared_ptr<Kml::Vectors> vecs);
    boost::shared_ptr<SViewData> _viewData;
    bool _debugTransName;
    void addTransNameLineString(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry);
    void OutputTileData(const vector_tile::Tile& newtile, std::string filename);
    bool _vectorDataDump;//if true dump all data to output file
    boost::shared_ptr<std::vector<boost::shared_ptr<symbol> > > _glbCustomSymbolList;//global list
    boost::shared_ptr<std::vector<boost::shared_ptr<symbol> > > _localCustomSymbolList;
    boost::shared_ptr<iconInfo> CreateIconInfo(POIENUM classification, std::string iconFilename);
    boost::shared_ptr<placemarkCollection> _placemarkCollection;
    boost::shared_ptr<openglfont> _openglFont;
    bool isCity(std::string classification);
    boost::shared_ptr<iconInfo> addPlace(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry);
    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > _points;
    boost::shared_ptr<std::map<std::string, boost::shared_ptr<iconInfo> > > _poiIconMap;//set icon hrefs from names that are used in vector tiles
    boost::shared_ptr<std::map<std::string, boost::shared_ptr<iconInfo> > > _placeIconMap;
    void fillIconMap();
    boost::shared_ptr<iconInfo>  addPoi(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry);
    std::string findStringForKey(std::string keyString, vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat);
#if defined(__APPLE__)
    boost::shared_ptr<std::map<std::string, GLuint > > _symbolTextures;
    GLuint findTextureBasedOnHref(std::string href);
#else
    boost::shared_ptr<std::map<std::string, boost::shared_ptr<QOpenGLTexture> > > _symbolTextures;
    boost::shared_ptr<QOpenGLTexture> findTextureBasedOnHref(std::string href);
#endif
    std::string _tileserverIP;
    std::string _tileserverPort;
    std::string _pngFolder;
     std::vector<boost::shared_ptr<symbol> > _symbols;
    
    
     std::vector<boost::shared_ptr<pathway> > _pathways;//use for transportation_name
    std::vector<boost::shared_ptr<pathway> > _pathwaysOutsideN;
    std::vector<boost::shared_ptr<pathway> > _pathwaysOutsideS;
    std::vector<boost::shared_ptr<pathway> > _pathwaysOutsideE;
    std::vector<boost::shared_ptr<pathway> > _pathwaysOutsideW;
     boost::shared_ptr<CoordinateConverter> _coordConverter;
     virtual void SetUserSettings() ;
public:
    //std::vector<boost::shared_ptr<pathway> > _pathways;//use for transportation_name
    
     virtual void OnNotifyCustomSymbolChange() ;
     virtual void AddSymbol() ;
     virtual void SortEntities(double vehLat, double vehLon);
     virtual void SetFont(boost::shared_ptr<openglfont> openglFont)
     {
         _openglFont = openglFont;
     };
    virtual void DataLoadFunction();
    bool SetupOpenglForEntities(GLuint program);
    void StartDataLoadThread();
    boost::shared_ptr<boost::thread> _dataLoadThread;
    bool _readyToSetupOpengl;
    bool _readyToRender;
#if defined(__APPLE__)
    GLuint _textureToBind;//todo: make a shared_ptr
    virtual bool RenderPerspectiveEntities(GLuint program, bool limitMet, boost::shared_ptr<SViewData> viewData, RIMatrix4x4 projection,
                                           double pixelSize, OVERLAYTYPE overlayType, bool billboardStyle, double textScale, int labelDensity);
    virtual bool RenderOrthographicEntities(GLuint program, bool limitMet, boost::shared_ptr<SViewData> _viewData, double windowWidth, double windowHeight, RIMatrix4x4 projection, std::vector<double>& alreadyDrawnIcons, bool billboardStyle, bool streetShieldsOn, int labelDensity);
    VectorTile(int x, int y, int zoomLevel, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
               boost::shared_ptr<CoordinateConverter> coordConverter, boost::shared_ptr<std::map<std::string,
               GLuint > > symbolTextures, boost::shared_ptr<placemarkCollection> pmCollection,
               boost::shared_ptr<std::map<std::string, boost::shared_ptr<iconInfo> > > poiMap, boost::shared_ptr<std::map<std::string,
               boost::shared_ptr<iconInfo> > > placeMap, boost::shared_ptr<std::vector<boost::shared_ptr<symbol> > > customSymbolList, double streetLineWidth,
               int autoScaleOffsetStVecs, int autoScaleOffsetIndices, int autoScaleOffsetSize, int autoScaleStVecStride, int autoScaleIndicesStride);
    GLuint _arrayBuf;
#else
    boost::shared_ptr<QOpenGLTexture> _textureToBind;//todo: make a shared_ptr
    virtual bool RenderPerspectiveEntities(QOpenGLShaderProgram* program, bool limitMet, boost::shared_ptr<SViewData> viewData, QMatrix4x4 projection,
                                           double pixelSize);
    virtual bool RenderOrthographicEntities(QOpenGLShaderProgram* program, bool limitMet, boost::shared_ptr<SViewData> _viewData, double windowWidth, double windowHeight, QMatrix4x4 projection);
    VectorTile(int x, int y, int zoomLevel, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
               boost::shared_ptr<CoordinateConverter> coordConverter, boost::shared_ptr<std::map<std::string,
               boost::shared_ptr<QOpenGLTexture> > > symbolTextures, boost::shared_ptr<placemarkCollection> pmCollection,
               boost::shared_ptr<std::map<std::string, boost::shared_ptr<iconInfo> > > poiMap, boost::shared_ptr<std::map<std::string,
               boost::shared_ptr<iconInfo> > > placeMap, boost::shared_ptr<std::vector<boost::shared_ptr<symbol> > > customSymbolList);
    QOpenGLBuffer _arrayBuf;
#endif
    
    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > _streets;
    
    ~VectorTile();
    
    void LoadVertexBuffers();
    
    virtual int SetEML(boost::shared_ptr<ElevationManagerLocal> eml);
    boost::shared_ptr<ElevationManagerLocal> _EML;
};

//_missingIcons.reset(new std::vector<std::string>);
#endif // RAVECTORTILE_H
