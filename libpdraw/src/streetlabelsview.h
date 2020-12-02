//
//  StreetLabels.h
//  LFClientEngine
//
//  Created by David Dvergsten on 1/3/17.
//  Copyright © 2017 Rapid Imaging Technologies Inc. All rights reserved.
//

#include "ElevationManager.h"
#include "ROIMan.h"
#include "SViewData.h"
#include <QOpenGLTexture>
#include <vector>
#include <risopengltext.h>
#include <placemarkdatatypes.h>
#include </usr/local/include/boost/thread.hpp>
#include <string>
#include <StreetSegment.h>
#include "LoggableObject.h"
#include <roilistener.h>

class StreetLabelsView: public LoggableConfigurableObject, public RoiListener
{
protected:
    bool _hideLabels;
    void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
public:
    CoordinateConverter _oldCoordConverter;
    void triggerNewROI();
    bool PendingSwapFromListener();
    bool _pendingSwap;
    void SignalLabelRegenThread();
    void SetLabelScale(double scale);
    virtual void OnPendingRoiChange();
    virtual void OnRoiChangeSwap();
    virtual void OnRoiChange(double lowLat, double lowLon, double upperLat, double upperLon) ;

    void LoadOpenglTextures();
    void renderWithCameraX(double camX, double camY, double camZ, boost::shared_ptr<SViewData> viewData, double frameWidth,
                            double frameHeight, double displayWidth, double displayHeight,
                            QOpenGLShaderProgram* program, QMatrix4x4 projectionMatrix);
    //void renderWithCameraX(double camX, double camY, double camZ, boost::shared_ptr<SViewData> viewData,
      //                 double frameWidth, double frameHeight, double displayWidth, double displayHeight);
    void AddPlacemarks(boost::shared_ptr<RISPlacemarkCollection> placemarkCollection);
    StreetLabelsView(boost::shared_ptr<ROIMan> roiMan);//, boost::shared_ptr<ElevationManager> elTiff);
    ~StreetLabelsView();
    //void startLabelGenThread();
    void terminateLabelGenThread();
    bool _labelGenStarted;
    int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);

private:
    boost::shared_ptr<boost::thread> _labelGenThread;
    boost::condition_variable _streetLabelRequestCondition;
    bool _streetLabelRegenRequestReady;
    boost::mutex _streetLabelRegenMutex;


    double _userLabelScale;
    double _labelScale;
    void ReversePointList(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices);
    void AlignStreetVectorOrientation(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices);
    bool _skipLabelGenIteration;
    void ResetFilteredLabelsCopy();
    SHIELDTYPES GetShieldType(boost::shared_ptr<Kml::Vectors> pline);
    void CalculatePlineCenterCoord(boost::shared_ptr<Kml::Vectors> pline, double& centerx, double& centery, double& centerz);
    double CalculatePlineLength(boost::shared_ptr<Kml::Vectors> pline);
    double CreateStreetSegmentVector(std::vector<boost::shared_ptr<Kml::Point> > ls,
                                     std::vector<boost::shared_ptr<StreetSegment> >& streetSegments, bool newCoordConv);
    double GetDist2Points(boost::shared_ptr<Kml::Point> coord, boost::shared_ptr<Kml::Point> coord2);
    //void GetCoordXYZ(boost::shared_ptr<Kml::Point> coord, double& x, double& y, double& z);
    boost::shared_ptr<UserSettingsData> _usettingsdata;
    void initSymbolGeometry();
    void Init();
    double calculateSymbolScaleCamx(double camx, double camy, double camz, double symx, double symy,
                                    double symz, double fov, double pixHeight);
    boost::shared_ptr<SViewData> _viewData;
    double radiansToDegreesWithRadian(double radians) ;
    double _camX;
    double _camY;
    double _camZ;
    QMatrix4x4 calculateSymbolMatrices(double shieldX, double shieldY, double shieldZ, bool newCoordConv);
    QOpenGLBuffer _arrayBuf;
    QOpenGLBuffer _indexBuf;
    bool findIntersectWith2LinesX1(double x1, double y1, double x2, double y2, double x3,
                                   double y3, double x4, double y4, double& intersectX, double& intersectY);
    int findIntersectWithFootprintLat1(double lat1, double lon1, double lat2,
            double lon2, std::vector<double>& vintersectLat, std::vector<double>& vintersectLon);
    void GroupPolylinesByName(boost::shared_ptr<RISPlacemarkCollection> placemarkCollection);
    void copyPlacemarksFrom(boost::shared_ptr<RISPlacemarkCollection> from, boost::shared_ptr<RISPlacemarkCollection> to);
    void SetFilteredLabelsCopy();
    double degreesToRadiansWithDegree(double degrees);
    double FindRotationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double charLocation, double charsize,
                                double& charx, double& chary, double& charalt);
    void renderShield(double xpos, double ypos, double zpos, std::string text, double scale,
                                        double rotation, bool tiltUp, SHIELDTYPES shieldType, bool newCoordConv);
    void findCenterLocationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double& x, double& y, double& z);
    void DrawStreetLabelsFlat();
    void makeInterstateTextureCurrent();
    void makeUsHwyTextureCurrent();
    void makeStateRoadTextureCurrent();
    void makeCountyRoadTextureCurrent();
    double CalculateSymbolScale3DCamx(double camx, double camy, double camz, double symbolx,
                                      double symboly, double symbolz, double fov, double pixelHeight);
    bool pointInsideFootPrintLat(double latitude, double longitude);
    int PointInsidePolyNvert(int nvert, float * vertx, float * verty, float testx, float testy);
//    double CalculateSymbolZRotDeltaX(double& deltax, double& deltay);
    //double CalculateSymbolZRotDeltaX(double & losHeading);
    double CalculateSymbolXRotDeltaX(double& deltax, double& deltay, double& deltaz);
    double CalculateStreetVectorLength(boost::shared_ptr<Kml::Vectors> lineString);
    bool AddFilteredLabelsDeepPitch(std::vector<boost::shared_ptr<Kml::Vectors> > deepPitch, std::vector<boost::shared_ptr<Kml::Vectors> > horizon);
    int PointInsidePolyNumVertices(int nvert, float* vertx, float* verty, float testx, float testy);
    void AddCameraValuesCameraX(double camX, double camY, double camZ, boost::shared_ptr<SViewData> viewData);
    void SplitLabelsByPitch();
    void FilterLabelsByName();
    void FilterLabelsInFootPrint();
    void FilterLabelsByFit();
    boost::shared_ptr<RISPlacemarkCollection> createPlinesInFootprint(boost::shared_ptr<RISPlacemarkCollection> placemarks);
    void labelGenThreadMethod();
    void SetCameraValuesCopy();
    void SetPlacemarksCopy();
    std::map<std::string, int> _filteredEntities;
    void synchronizeLabelValues();
    QMatrix4x4 _projectionMatrix;
    QOpenGLShaderProgram* _program;
    //boost::shared_ptr<boost::thread> _labelGenThread;
    QOpenGLTexture* loadTextureWithFilename(std::string filename, std::string fileType, std::string folder);
    void loadShieldVertices();
    bool _fadeOut;
    bool _fadeIn;
    bool _swapLabels;
    double _alpha;
    boost::shared_ptr<RISOpenglText> _openglText;
    boost::shared_ptr<RISPlacemarkCollection> _risPlacemarkCollectionOriginal;
    boost::shared_ptr<RISPlacemarkCollection> _risPlacemarkCollectionLabelThreadCopy;
    boost::shared_ptr<RISPlacemarkCollection> _risPlacemarkCollectionvportChopped;
    bool _labelGenThreadStarted;
    double _camXOriginal;
    double _camYOriginal;
    double _camZOriginal;

    double _camXLblThreadCopy;
    double _camYLblThreadCopy;
    double _camZLblThreadCopy;

    boost::shared_ptr<SViewData> _viewDataOriginal;
    boost::shared_ptr<SViewData> _viewDataLblThreadCopy;

    std::vector<boost::shared_ptr<Kml::Vectors> >_labelLocationsHorizonLblThread;
    std::vector<boost::shared_ptr<Kml::Vectors> >_labelLocationsHorizonRenderThread;

    std::vector< boost::shared_ptr<Kml::Vectors> >_labelLocationsDeepPitchLblThread;
    std::vector<boost::shared_ptr<Kml::Vectors> >_labelLocationsDeepPitchRenderThread;

    //NSLock* _queueLock;
    boost::shared_ptr<ROIMan> _roiMan;
    double _frameWidth;
    double _frameHeight;
    double _displayWidth;
    double _displayHeight;
    double _pixelHeight;
    QOpenGLTexture* _interstateTexture;
    QOpenGLTexture* _usHwyTexture;
    QOpenGLTexture* _countyRoadRectTexture;
    QOpenGLTexture* _stateRoadCircleTexture;


    GLfloat         _positionVerticesShield[12];
    GLfloat         _textureVerticesShield[8];
    std::vector<boost::shared_ptr<Kml::Vectors> >_possibleLabelLocationsFlat;
    std::vector<boost::shared_ptr<Kml::Vectors> >_possibleLabelLocInsideFP;
    std::vector<std::string>_labelSearch;
    std::vector<boost::shared_ptr<Kml::Vectors> >_possibleLabelLocNonRepeat;
    //NSThread* _labelGenThread;
    //NSLock* _placemarkCollectionLock;
    boost::mutex _placemarkCollectionLock;
    bool _newPlacemarks;
    //NSLock* _cameraDataLock;
    boost::mutex _cameraDataLock;
    //NSLock* _filteredLabelsLock;
    boost::mutex _filteredLabelsLock;
    bool _newFilteredLabels;
};
