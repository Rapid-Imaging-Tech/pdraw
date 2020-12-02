//test git submodule
#pragma once
#ifndef __GL_COMPONENT
#define __GL_COMPONENT
//#define MAXTILESTORENDER 9
#define NUMOFZOOMLEVELSRASTER 21
#define NUMOFZOOMLEVELSVECTILE 15
#define CACHESIZELIMIT 50 //prune the tilecache, only allow the latest 20 tiles from each zoom level to exist in cache
#include <boost/asio.hpp>
#include "LFClient.h"
#include "UserSettings.h"
//#include "elevationclient.h"
#include "elevationmanagerlocal.h"
#include "openglfont.h"
#if defined(__APPLE__)
#else
#include <QtGui/QOpenGLShaderProgram>
#endif
#include "boost/thread.hpp"
#include <rafootprint.h>
#include <vectortile.h>
#include "coordinateconverter.h"
#include "rastertile.h"
#include "StatusLogger.h"
#include "pathway.h"
enum TILETYPE{TTVECTOR, TTRASTER};
class LFClientEngine: public LFClient, public LoggableConfigurableObject
{
private:
    //this is a flag to state that we have constructed this object and we need to check for previous kml loaded
    //ie, user clicked 2d map so this was destroyed and restarted, need to reload previous kml
    bool _regenKml;
    int _labelDensity;
    unsigned int _autoScaleOffsetSize;
    unsigned int _autoScaleOffsetStVecs;
    unsigned int _autoScaleOffsetIndices;
    
    unsigned int _autoScaleStVecStride;
    unsigned int _autoScaleIndicesStride;
    
    GLuint _ublockindex;
    GLint _maxBindPoints;
    int _indexUniformBufferBindList;
    std::vector<boost::shared_ptr<tile> > _uniformBlockBindings;
    //std::vector<float> _FBOQuadVertices;
    //GLuint _FBOQuad;
    //void InitQuadShader();
    void InitAutoscaleShader();
    unsigned int _textureColorbuffer;
    unsigned int _framebuffer;
    std::vector<std::string> _kmzFilesLoaded;
    bool _initialized;
    bool _recordingFlightData;
    double _pixelSize;
    std::string _kmlFilename;
    bool _newKmlFile;
    std::vector<boost::shared_ptr<pathway> > _pathways;
    void NotifyCustomSymbolListeners();
    boost::shared_ptr<std::vector<boost::shared_ptr<symbol> > > _customSymbolList;
    boost::shared_ptr<iconInfo> CreateIconInfo(POIENUM classification, std::string iconFilename);
    void FillIconMap();
    boost::shared_ptr<std::map<std::string, boost::shared_ptr<iconInfo> > > _poiIconMap;//set icon hrefs from names that are used in vector tiles
    boost::shared_ptr<std::map<std::string, boost::shared_ptr<iconInfo> > > _placeIconMap;
#if defined(__APPLE__)
    GLuint findTextureBasedOnHref(std::string href);
    boost::shared_ptr<std::map<std::string, GLuint > > _symbolTextures;
    GLuint loadSymbolTextureWithFilename(std::string filename, std::string fileType, std::string folder);
    GLuint _risShaderProgram;
    GLuint _risShaderProgramAutoScale;
    GLuint _apositionLocation;//location in shader where position vertices stored
    GLuint _atextureLocation;
    
    GLuint _apositionLocationFBO;//location in shader where position vertices stored
    GLuint _atextureLocationFBO;
    bool _overlaysOn;
    bool _streetLinesOn;
    bool _streetLabelsOn;
    bool _streetShieldsOn;
#else
    boost::shared_ptr<QOpenGLTexture> findTextureBasedOnHref(std::string href);
    boost::shared_ptr<std::map<std::string, boost::shared_ptr<QOpenGLTexture> > > _symbolTextures;
    boost::shared_ptr<QOpenGLTexture> loadSymbolTextureWithFilename(std::string filename, std::string fileType, std::string folder);
#endif
    boost::shared_ptr<std::vector<std::string> > _listCustomSymbols;
    void SetSymbolElevations(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > > points);
    double _alphaStreetVector;
    double _alphaPOI;
    double _alphaStreetLabel;
    
    int _minRasterZoomLevel;
    std::string _fontPngFile;
    std::string _fontJsonFile;
    virtual void SetUserSettings();
    int _maxTilesToRender;
    boost::shared_ptr<UserSettings> _userSettings;
    boost::shared_ptr<UserSettingsData> _usettingsdata;
    boost::shared_ptr<StatusLogger> _statusLogger;
    boost::shared_ptr<placemarkCollection> _placemarkCollection;
    void RefreshRenderOrderSubEntities(boost::shared_ptr<vector<boost::shared_ptr<tile> > > newRenderList);
    void RefreshTileRenderOrder(boost::shared_ptr<vector<boost::shared_ptr<tile> > > newRenderList);
    void SetupRenderList(int minTileY, int maxTileY, int minTileX, int maxTileX, bool& listChange,
                         boost::shared_ptr<vector<boost::shared_ptr<tile> > > newRenderList,  TILETYPE type, int zoomLevel );
    boost::shared_ptr<openglfont> _openglFont;
    int _windowWidth ;
    int _windowHeight ;
    int _rasterTileCacheLimits[NUMOFZOOMLEVELSRASTER];
    int _vectorTileCacheLimits[NUMOFZOOMLEVELSVECTILE];
    
    void LoadSymbolTextures();
    bool _symbolTexturesLoaded;
    
    double _startingLat;
    double _startingLon;
    double _startingAlt;
    double _manualDeltaLat;
    double _manualDeltaLon;
    
    bool _settingsFileFlight;
    boost::shared_ptr<CoordinateConverter> _coordConverter;
    bool _playVideo;
    std::string _videoFileName;
    
    boost::shared_ptr<SViewData> _viewData;
    void LFGeodeticToCartesian(double lat, double lon, double altitude,
                               double& x, double& y, double& z);
    boost::mutex _kmlFileReadMutex;
    std::vector<boost::shared_ptr<KmlPathDetails> > AddKMLEntitiesFromFile(std::string kmlFileName, bool listOnly);
    bool _billboardStyle;
public:
    void SetBillboardStyle(bool val);
    std::vector<KmlPathDetails> GetKMLEntitiesFromAlreadyLoadedFile(std::string kmlFileName);
    std::vector<std::string> GetLoadedKMZFilenames();

    void GetLatLon(int screenX, int screenY, int screenWidth, int screenHeight, double* lat, double* lon, int vpwidth, int vpheight);
    int AddUserPoint(int screenX, int screenY, double* lat, double* lon, double* elev, int screenWidth, int screenHeight, std::string name, int vpwidth, int vpheight);
    virtual void SetPOISettings(boost::shared_ptr<placemarkCollection> pmCollection);
    int tileDataLoadTask(int seconds, boost::shared_ptr<tile> newTile) ;
    void push_job(int seconds, boost::asio::io_service& io_service, std::vector<boost::shared_future<int> >& pending_data, boost::shared_ptr<tile> newTile) ;
    void DataLoadFunction();
    int _currentRasterZoomLevel;
    int _currentVecTileZoomLevel;
    //each frame we calc which tiles should show up, we first add them, then we need to remvoe from render list because they are out of view
    //this function does that
    int GetZoomLevel(int& minTileX, int& minTileY, int& maxTileX, int& maxTileY, TILETYPE tiletype);
    boost::shared_ptr<tile> SearchForTileOnList(boost::shared_ptr<vector<boost::shared_ptr<tile> > > listToSearch, int x, int y);
    //this vector saves every vectile, so if we already have it we don't have to go to server to get it.
    boost::shared_ptr<vector<boost::shared_ptr<tile> > >  _rasterTileCache[NUMOFZOOMLEVELSRASTER];//create vector of vector tiles for each 0-14 zoomlevels
    boost::shared_ptr<vector<boost::shared_ptr<tile> > >  _vectorTileCache[NUMOFZOOMLEVELSVECTILE];
    boost::shared_ptr<ElevationManagerLocal> _EML;
    
    //this vector represents the vector tiles that symmgr etc are reading and rendering
    boost::shared_ptr<vector<boost::shared_ptr<tile> > >  _rasterTileRendering[NUMOFZOOMLEVELSRASTER];//create vector of vector tiles for each 0-14 zoomlevels
    boost::shared_ptr<vector<boost::shared_ptr<tile> > >  _vectorTileRendering[NUMOFZOOMLEVELSVECTILE];
    
    void PrintStatusOfStack(boost::shared_ptr<vector<boost::shared_ptr<tile> > >  stackToPrint);
    boost::shared_ptr<RAFootprint> _footPrintCalculator;
    FootPrint _currentFootPrint;
    virtual void AddKMLFileRegen();
    virtual std::vector<PlacemarkInfo > AddKMLFile(std::string kmlfile, bool regen = false);
#if defined(__APPLE__)
    void StartRecordingFlightData();
    void StopRecordingFlightData();
    virtual void HideKMLEntities(std::string filename, vector<std::string> path, std::string element) ;
    virtual void DeleteKMLEntities(std::string filename, vector<std::string> path, std::string element) ;
    virtual void ShowKMLEntities(std::string filename, std::vector<std::string> path, std::string element);
    int Init();
    void InitShaders();
    virtual int Render(double lowerLeftX,
                       double lowerLeftY, double windowWidth, double windowHeight, boost::shared_ptr<SViewData> viewData) ;
    void TurnOnOffStreetLines(bool on){_streetLinesOn = on;};
    void TurnOnOffStreetLabels(bool on){_streetLabelsOn = on;};
    void TurnOnOffStreetShields(bool on){_streetShieldsOn = on;};
    void TurnOnOffOverlays(bool on){_overlaysOn = on;};
#else
    virtual void HideKMLEntities(QString filename, QStringList path, QString element) ;
    virtual void ShowKMLEntities(QString filename, QStringList path, QString element) ;
    virtual void DeleteKMLEntities(QString filename, QStringList path, QString element) ;
    virtual int Init(boost::shared_ptr<SViewData> viewData, QOpenGLShaderProgram* symShader,
                     QOpenGLShaderProgram* lineworkShader) ;
    virtual int Render(QOpenGLShaderProgram *program, QMatrix4x4 projection, double lowerLeftX,
                       double lowerLeftY, double windowWidth, double windowHeight) ;
#endif
    virtual void SetLabelDensity(int labelDensity){_labelDensity = labelDensity;};
    virtual void SetAlphaStreetVector(double alpha);
    virtual void SetAlphaStreetLabel(double alpha);
    virtual void SetAlphaPOI(double alpha);
    
    virtual void UpdateROI() {};//old!!
    virtual int enableStreetVectors(bool enable) {return 1;};
    virtual int enableStreetLabels(bool enable) {return 1;};
    virtual int enablePlacemarks(bool enable) {return 1;};
    virtual void SetLODOverride(int lod) {};
    
    virtual int SetServiceLayers(std::vector <LAYERENUM>) {return 0;};
    virtual void SetStreetLineWidth(double lineWidth) {};
    virtual void SetStreetLineColor(double red, double green, double blue) {};
    virtual void SetStreetLabelScale(double scale) {};
    virtual void GetStarePoint(double& latitude, double& longitude, double& elevation, double& range) {};
    virtual void SetVP(boost::shared_ptr<SViewData> viewPointData, bool fromKLV = false) ;
    
    boost::shared_array<unsigned char> _saVideo;
    LFClientEngine();
    ~LFClientEngine();
    
};

#endif
