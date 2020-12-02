#ifndef TILE_H
#define TILE_H

#if !defined(__APPLE__)
#include "qopenglshaderprogram.h"
#endif
#include "boost/shared_ptr.hpp"
#include "SViewData.h"
#include "openglfont.h"
#include "elevationmanagerlocal.h"
#include "utilities.h"
#include "RIMatrixMath.hpp"
//#include "LoggableObject.h"
class tile //: public LoggableConfigurableObject
{
protected:
    bool _clearCacheFolder;
    double _distToVehicle;
    unsigned int _sortOrderNum;
    bool _carryFlag;
    double _centerLat;
    double _centerLon;
    int _x;
    int _y;
    int _zoomLevel;
    BoundingBox _boundingBox;
    bool IsPointInside(const double& lat, const double& lon);
public:
    int _ubBindingPoint;
   // bool tile::IsPointInside(const double& lat, const double& lon)
    virtual void OnNotifyCustomSymbolChange() = 0;
    virtual void AddSymbol() = 0;
    double GetDistToVehicleQuick(){return _distToVehicle;};
    int GetX(){return _x;};
    int GetY(){return _y;};
    double GetCenterLat(){return _centerLat;};
    double GetCenterLon(){return _centerLon;};
    void UpdateDistToVehicle(double vehLat, double vehLon) ;
    virtual void SortEntities(double vehLat, double vehLon) = 0;
    int GetSortOrderNum(){return _sortOrderNum;};
    void UpdateSortOrderNum();
    //virtual int GetX() = 0;
    //virtual int GetY() = 0;
    virtual void DataLoadFunction() = 0;
    virtual void SetFont(boost::shared_ptr<openglfont> openglFont) = 0;
    virtual int SetEML(boost::shared_ptr<ElevationManagerLocal> eml) = 0;
    
#if defined(__APPLE__)
    virtual bool RenderPerspectiveEntities(GLuint program, bool limitMet, boost::shared_ptr<SViewData> Data, RIMatrix4x4 projection, double pixelSize, OVERLAYTYPE overlayType, bool billboardStyle,
                                           double textScale, int labelDensity) = 0;
    virtual bool RenderOrthographicEntities(GLuint program, bool limitMet, boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight, RIMatrix4x4 projection, vector<double>& alreadyDrawnIcons,
                                            bool billboardStyle, bool streetShieldsOn, int labelDensity) = 0;
    
#else
    virtual bool RenderPerspectiveEntities(QOpenGLShaderProgram* program, bool limitMet, boost::shared_ptr<SViewData> viewData, QMatrix4x4 projection,
                                           double pixelSize) = 0;
    virtual bool RenderOrthographicEntities(QOpenGLShaderProgram* program, bool limitMet, boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight, QMatrix4x4 projection) = 0;
#endif
    tile(int x, int y, int zoomLevel);
};

#endif // TILE_H
