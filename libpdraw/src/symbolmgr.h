//
//  SymbolMgrRA.h
//  LFClientEngine
//
//  Created by David Dvergsten on 11/30/16.
//  Copyright © 2016 Rapid Imaging Software Inc. All rights reserved.
//
#pragma once
#include <ElevationManager.h>
#include "ROIMan.h"
#include "SViewData.h"
#include <QMatrix4x4>
#include <KMLEntities.h>
#include <placemarkdatatypes.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <risopengltext.h>
#include <boost/thread/condition.hpp>
#include <roilistener.h>
#include <map>
#include <vector>
#include <string>
class SymbolGroup{
public:

    SymbolGroup():_roiRefreshable(false),
        _uid(-1),
        _hide(false)
    {
        _symbolGroup.reset(new std::vector<boost::shared_ptr<SymbolData> > );
    }
    ~SymbolGroup(){}
    bool _roiRefreshable;
    int _uid;
    boost::shared_ptr<std::vector<boost::shared_ptr<SymbolData> > > _symbolGroup;
    //std::vector<boost::shared_ptr<SymbolData> >  _symbolGroup;
    bool _hide;
private:
};

class SymbolMgr : public LoggableConfigurableObject, public RoiListener {

public:
    //int enableStreetVectors(bool enable);
    //int enableStreetLabels(bool enable);
    int enablePlacemarks(bool enable);
    void CreateCustomIcons(std::vector<std::string> newIconFileNames);
    void HideKMLSymbols(boost::shared_ptr<KmlPathDetails> path);
    void ShowKMLSymbols(boost::shared_ptr<KmlPathDetails> path);
    void DeleteKMLSymbols(boost::shared_ptr<KmlPathDetails> path);
    virtual void OnPendingRoiChange();
    virtual void OnRoiChangeSwap();
    virtual void OnRoiChange(double lowLat, double lowLon, double upperLat, double upperLon) ;
    void Shutdown();
    //void addSymbolWithLatitude(double lat, double lon, double alt, std::string lbl, std::string icon,  bool mapSymbol);
    void renderWithCameraX(double camX, double camY, double camZ, boost::shared_ptr<SViewData> viewData, double frameWidth,
        double frameHeight,  double displayWidth, double displayHeight, QOpenGLShaderProgram* program,  QMatrix4x4 projection);
    void clearSymbols();
    int getMapSymbolCount();
    SymbolMgr();

    int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID,
        boost::shared_ptr<ROIMan> roiman, int openglVersionMaj);

    //void ReplacePlacemarks(boost::shared_ptr<RISPlacemarkCollection> placemarkCollection, int uid, bool roiRefreshable);
    int AddPlacemarks(boost::shared_ptr<RISPlacemarkCollection> placemarkCollection,  bool roiRefreshable);
    //void SetElTiff(boost::shared_ptr<ElevationManager> elTiff);
    void LoadSymbolTextures();
protected:
    virtual void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
private:
    std::vector<std::string> _missingIcons;
    //int _placemarkCollectionUID;

    double _starePointX;
    double _starePointY;
    double _starePointZ;
    boost::mutex _risPlacemarkSwapMutex;//ensure placemarks sent in and set that we use don't overlap in the copy
    void NewSymbolsThread();

    boost::mutex _newSymbolsMutex;
    bool _newSymbolsReady;
    boost::condition_variable _newSymbolsCondition;
    boost::mutex _newSymbolsSwapLock;//lock around the time symmgr gets a new set of placemarks and when we read them in

    //lock around the time we are ready with new matrices for symbols, and when we use them to render
    boost::mutex _symbolMatricesMutex;
    bool _symbolMatricesReady;
    boost::condition_variable _symbolMatricesCondition;
    bool _shutdown;

    void SymbolMonitorThread();
    boost::shared_ptr<boost::thread> _symbolMonitorThread;
    boost::shared_ptr<boost::thread> _newSymbolsThread;

    double calculateDistToSymbol(boost::shared_ptr<SymbolData> sym, double vehlat, double vehlon);
    //double calculateDistToSymbol(boost::shared_ptr<SymbolData> sym);
    std::string _iconFolder;
    boost::shared_ptr<UserSettingsData> _usettingsdata;
    bool _initComplete;
    int _openglVersionMaj;
    RISOpenglText _openglText;
    double radiansToDegreesWithRadian(double radians);
    void initSymbolGeometry();
    QOpenGLBuffer _arrayBuf;
    QOpenGLBuffer _indexBuf;

    QMatrix4x4 _projectionMatrix;
    QOpenGLShaderProgram* _program;
    QOpenGLTexture* findTextureBasedOnHref(std::string href);
    bool prepareRenderSymbolWithTexture(std::string href);
    void renderSymbols(boost::shared_ptr<std::vector<boost::shared_ptr<SymbolData> > > symbols) ;
    void calculateSymbolMatrices(boost::shared_ptr<std::vector<boost::shared_ptr<SymbolData> > > symbols);
    double calculateSymbolScaleCamx(double camx, double camy, double camz, double symx, double symy,
                                    double symz, double fov, double pixHeight);
    double degreesToRadiansWithDegree(double degrees);
    double CalculateSymbolXRotDeltaX(double& deltax, double& deltay, double& deltaz);
    std::vector<std::string> split(const char*phrase, std::string delimiter);
    int PointInsidePolyNumVertices(int nvert, double* vertx, double* verty, float testx, float testy);
    void AddSymbolsFromPlacemarkCollection(boost::shared_ptr<RISPlacemarkCollection> placemarkCollection) ;
    void createSymbolList();
    bool GenerateSymbolTextTextureWithSymbol(boost::shared_ptr<SymbolData> symData);
    QOpenGLTexture* loadSymbolTextureWithFilename(std::string filename, std::string fileType, std::string folder);
    bool _symbolTexturesLoaded;
    boost::shared_ptr<ROIMan> _roiMan;
    boost::shared_ptr<ElevationManager> _elevMgr;
    std::vector<boost::shared_ptr<RISPlacemarkCollection> > _newPlacemarkCollection;
    boost::mutex _queueLock;
    boost::mutex _symbolSwapLock;
    boost::mutex _placemarkRequestMutex;
    bool _generateSymbolLists;
    boost::shared_ptr<SViewData> _viewData;
    bool _symbolsLoaded;
    double _lblOffset;
    double _frameWidth;
    double _frameHeight;
    double _displayWidth;
    double _displayHeight;
    double _camX;
    double _camY;
    double _camZ;
    bool _symbolsNeedSorting;
    std::vector<boost::shared_ptr<SymbolData> > _streetLabelSymbols;
    std::vector<boost::shared_ptr<SymbolData> > _mapSymbols;
    std::vector<boost::shared_ptr<SymbolGroup> > _symbolsVec;
    boost::shared_ptr<SymbolGroup> _roiRefreshableSymbols;
    //std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<SymbolData> > > > _backBuffSymbols;
    //std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<SymbolData> > > > _vecNewSymbolsBackBuffer;
   // boost::shared_ptr<std::vector<boost::shared_ptr<SymbolData> > > _newSymbolsBackBuffer2;
    void sortSymbols(boost::shared_ptr<std::vector<boost::shared_ptr<SymbolData> > >& unsortedSymbols);
    std::vector<boost::shared_ptr<SymbolData> > _possibleLabelLocInsideFP;
    std::vector<std::string> _labelSearch;
    std::vector<boost::shared_ptr<SymbolData> > _possibleLabelLocNonRepeat;
    //std::vector<boost::shared_ptr<SymbolData> > _symDataQueue;

    //textures for symbols
    std::map<std::string, QOpenGLTexture*> _symbolTextures;
//    QOpenGLTexture* _airportTexture;
//    QOpenGLTexture* _pinredTextureRA;
//    QOpenGLTexture* _accidentTexture;
//    QOpenGLTexture* _administrationTexture;
//    QOpenGLTexture* _agricultureTexture;
//    QOpenGLTexture* _barTexture;
//    QOpenGLTexture* _baseballTexture;
//    QOpenGLTexture* _blackrectangleTexture;
//    QOpenGLTexture* _bluerectangleTexture;
//    QOpenGLTexture* _campTexture;
//    QOpenGLTexture* _capitalTexture;
//    QOpenGLTexture* _churchTexture;
//    QOpenGLTexture* _cityTexture;
//    QOpenGLTexture* _congressTexture;
//    QOpenGLTexture* _firemenTexture;
//    QOpenGLTexture* _flagTexture;
//    QOpenGLTexture* _golfTexture;
//    QOpenGLTexture* _greenrectangleTexture;
//    QOpenGLTexture* _highwayTexture;
//    QOpenGLTexture* _hikerTexture;
//    QOpenGLTexture* _hillsTexture;
//    QOpenGLTexture* _hospitalTexture;
//    QOpenGLTexture* _houseTexture;
//    QOpenGLTexture* _libraryTexture;
//    QOpenGLTexture* _mosqueTexture;
//    QOpenGLTexture* _parkTexture;
//    QOpenGLTexture* _peakTexture;
//    QOpenGLTexture* _poiTextureRA;
//    QOpenGLTexture* _policeTexture;
//    QOpenGLTexture* _postalTexture;
//    QOpenGLTexture* _powerTexture;
//    QOpenGLTexture* _railyardTexture;
//    QOpenGLTexture* _riverTexture;
//    QOpenGLTexture* _roadTexture;
//    QOpenGLTexture* _schoolTexture;
//    QOpenGLTexture* _shoppingTexture;
//    QOpenGLTexture* _targetTexture;
//    QOpenGLTexture* _treesTexture;
//    QOpenGLTexture* _universityTexture;
//    QOpenGLTexture* _wavesTexture;
//    QOpenGLTexture* _whitecircle2Texture;
//    QOpenGLTexture* _whitecircleTexture;
//    QOpenGLTexture* _whiterectangleTexture;
};
