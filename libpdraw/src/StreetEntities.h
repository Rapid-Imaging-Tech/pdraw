#pragma once
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include "boost/shared_ptr.hpp"
#include "KMLEntities.h"
#include "ROIMan.h"
#include "LoggableObject.h"
#include "StreetSegment.h"
#include "symbolmgr.h"
#include <LFDefinitionsRA.h>
//enum SHIELDTYPES{USHWY, INTERSTATE, STATEROAD, COUNTYROAD, NONE};
typedef struct _POINTFLOAT{
    float x;
    float y;
}POINTFLOAT;

typedef struct _GLYPHMETRICSFLOAT{
    float gmfBlackBoxX;
    float gmfBlackBoxY;
    POINTFLOAT gmfptGlyphOrigin;
    float gmfCellIncX;
    float gmfCellIncY;
}GLYPHMETRICSFLOAT;

class StreetEntities : public LoggableConfigurableObject
{
 public:
    void Shutdown();
    void SetStreetVectors(std::vector<boost::shared_ptr<Kml::Vectors> > vecs);
    void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData) ;
    StreetEntities();
    ~StreetEntities();
    void Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID,
        boost::shared_ptr<ROIMan> roiman, boost::shared_ptr<SymbolMgr> symMgr);
    //void GenerateStreetLabels(std::vector<boost::shared_ptr<Kml::StreetLabel> > labels);
    void Render(boost::shared_ptr<SViewData> viewData, double camx, double camy, double camz, double frameWidth
        ,double frameHeight, double displayWidth, double displayHeight);
 private:
     std::vector<boost::shared_ptr<StreetSegment> > CreateStreetSegments(std::vector<boost::shared_ptr<Kml::Point> > points, double & streetLength);
     boost::thread* _labelGenThread;
     boost::shared_ptr<SymbolMgr> _symMgr;
     void DrawStreetLabelsFlat();
     std::vector<boost::shared_ptr<Kml::Vectors> > _labelLocationsDeepPitchRenderThread;
     std::vector<boost::shared_ptr<Kml::Vectors> > _labelLocationsDeepPitchLblThread;
     std::vector<boost::shared_ptr<Kml::Vectors> > _labelLocationsHorizonRenderThread;
     std::vector<boost::shared_ptr<Kml::Vectors> > _labelLocationsHorizonLblThread;
     void SetFilteredLabelsCopy();
     bool _newFilteredLabels;
     bool _newShieldsReady;
     double _frameWidth;
    double _frameHeight;
    double _displayWidth;
    double _displayHeight;
    double _pixelHeight;

     double _camXOriginal ;
    double _camYOriginal ;
    double _camZOriginal ;

    double _camXLblThreadCopy ;
    double _camYLblThreadCopy ;
    double _camZLblThreadCopy ;
    boost::shared_ptr<SViewData> _viewDataLblThreadCopy;
    boost::shared_ptr<SViewData> _viewDataOriginal;

    void AddPlacemarks(std::vector<boost::shared_ptr<Kml::Vectors> > vecs);
    void CopyStreetVecs(const std::vector<boost::shared_ptr<Kml::Vectors> >& from, std::vector<boost::shared_ptr<Kml::Vectors> >& to);
     boost::mutex _cameraDataMutex;
     boost::mutex _placemarkCollectionMutex;
     boost::mutex _filteredLabelMutex;
     bool _newPlacemarks;
     void SetCameraValuesCopy();
     void AddCameraValues(double camX, double camY, double camZ, boost::shared_ptr<SViewData> viewData);
     bool _labelGenStarted;
     bool _labelGenShutdown;
     void LabelGenThreadMethod();
     bool _labelLandGenThreadStarted;
     void StartLabelGenThread();
    void DrawStreetLabelsHorizon(double camx, double camy, double camz);
    void DrawStreetLabelsFlat(double camx, double camy, double camz);
    void FilterLabelsByFit();
    void SplitLabelsByPitch();
    double FindRotation(std::vector<boost::shared_ptr<StreetSegment> > segments, double charLocation, double charsize, double &charx,
        double &chary, double &charalt);
    int PointInsidePoly(int nvert, float *vertx, float *verty, float testx, float testy);
    std::vector<std::string> _labelSearch;
    void FilterLabelsByName();
    void FilterLabelsInFootPrint();
    void FilterMultipleLabels();
    std::vector<boost::shared_ptr<Kml::Vectors> > _possibleLabelLocationsFlat;
    std::vector<boost::shared_ptr<Kml::Vectors> > _possibleLabelLocInsideFP;
    std::vector<boost::shared_ptr<Kml::Vectors> > _possibleLabelLocNonRepeat;
    std::vector<boost::shared_ptr<Kml::Vectors> > _labelLocationsDeepPitch;
    std::vector<boost::shared_ptr<Kml::Vectors> > _labelLocationsHorizon;
    double CalculateStreetVectorLength(boost::shared_ptr<Kml::Vectors> vectors);
    std::vector<boost::shared_ptr<Kml::Vectors> > _streetVectors;
    std::vector<boost::shared_ptr<Kml::Vectors> > _risStreetVectorsOriginal;
    std::vector<boost::shared_ptr<Kml::Vectors> > _risStreetVectorsLabelThreadCopy;
    std::vector<boost::shared_ptr<Kml::Vectors> > _risStreetVectrsvportChopped;
    void SetPlacemarksCopy();
    void SynchronizeLabelValues();
    boost::shared_ptr<UserSettingsData> _usettingsdata;
    double CalculateXrot(double x, double y, double z);
    double _camx;
    double _camy;
    double _camz;
    double _fov;
    double CalculateSymbolScale3D(double camx, double camy, double camz, double symbolx, double symboly, double symbolz, double FOV, double pixelHeight);
    int WhatQuadrant(double camLat, double camLon, double pointLat, double pointLon);
    void RenderFont(float xPos, float yPos, float zPos, unsigned int base, const char *str, double scale,bool outline, bool textOnly, double rotation,
        double camx, double camy, double camz, bool tiltUpToCamera, double shiftX);
    boost::shared_ptr<ROIMan> _pROIMan;
    void ReleaseFont(unsigned int base);
    std::vector<boost::shared_ptr<Kml::StreetLabel> > _labels;
    unsigned int m_fontListBase;
    unsigned int CreateOutlineFont(char *fontName, int fontSize, float depth);
    float m_angle;
    GLYPHMETRICSFLOAT gmf[256];   // holds orientation and placement
    double GetTextWidth(std::string text);
    void FindCenterLocation(std::vector<boost::shared_ptr<StreetSegment> > segments, double &x, double &y, double& z, double& lat, double& lon);
    std::vector<boost::shared_ptr<Kml::Vectors> > CreatePlinesInFootprint(std::vector<boost::shared_ptr<Kml::Vectors> > placemarks);
    bool PointInsideFootPrint(double latitude, double longitude);
    int FindIntersectWithFootprint(double lat1, double lon1, double lat2,  double lon2,
        std::vector<double>& vintersectLat, std::vector<double>& vintersectLon);
    bool FindIntersectWith2Lines(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4,
        double& intersectX, double& intersectY);
    void AddFilteredLabelsDeepPitch(std::vector<boost::shared_ptr<Kml::Vectors> > deepPitch,
    std::vector<boost::shared_ptr<Kml::Vectors> > horizon);
    void CreateShield(double xpos, double ypos, double lat, double lon, double zpos, std::string text, double scale,
        double rotation, bool tiltUp, SHIELDTYPES shieldType);
    double GetDistance(double lat1, double lon1, double lat2, double lon2);
};
