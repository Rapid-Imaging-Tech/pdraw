#ifndef SYMBOL
#define SYMBOL
#include "RotationTK.h"

#include "boost/shared_ptr.hpp"
#include "SViewData.h"
#include "LFDefinitionsRA.h"
#if defined(__APPLE__)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#include "RIMatrixMath.hpp"
#else
#include <QOpenGLTexture>
#include <qopenglbuffer.h>
#include <qopenglshaderprogram.h>
#endif
#include "boost/shared_array.hpp"
#include "openglfont.h"
#include "opengltext.h"
#include "UserSettingsData.h"
//#include "vectortile.h"
#include "LFClient.h"
#include "LoggableObject.h"
#include "kmlpathdetails.h"
class iconInfo
{
public:
    iconInfo(POIENUM pmType, std::string iconFilename):
        _pmType(pmType),
        _iconFileName(iconFilename)
    {
    };

    POIENUM _pmType;
    std::string _iconFileName;
private:
};
class symbolCartesianPoint{
public:
    symbolCartesianPoint(double x, double y, double z)
    {
        _x = x;
        _y = y;
        _z = z;
    };
    double _x;
    double _y;
    double _z;
};
class symbol: public LoggableConfigurableObject
{
public:
    void UpdateDistToVehicle(double vehLat, double vehLon);
    double GetDistToVehicle(){return _distToVehicle;};
    void InitOpenGLEntities();
//#if defined(__APPLE__)
symbol(double lat, double lon, double xcartesian, double ycartesian, double zcartesian, GLuint texture,
           boost::shared_ptr<openglfont> font, std::string text,
           boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
           GLuint whiteTexture,  boost::shared_ptr<iconInfo> icInfo);
    ~symbol();
    void render( boost::shared_ptr<SViewData> _viewData, double windowWidth, double windowHeight, GLuint program,
                double vehx, double vehy, double vehz, RIMatrix4x4 projection, std::vector<double>& alreadyDrawnIcons, bool billboardStyle, int currentLabelDensityLevel);
//#else
//    symbol(double lat, double lon, double xcartesian, double ycartesian, double zcartesian, boost::shared_ptr<QOpenGLTexture> texture,
//           boost::shared_ptr<openglfont> font, std::string text,
//           boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
//           boost::shared_ptr<QOpenGLTexture> whiteTexture,  boost::shared_ptr<iconInfo> icInfo);
//    ~symbol();
//    void render( boost::shared_ptr<SViewData> _viewData, double windowWidth, double windowHeight, QOpenGLShaderProgram* program,
//                 double vehx, double vehy, double vehz, QMatrix4x4 projection);
//#endif
    double GetLat(){return _lat;};
    double GetLon(){return _lon;};
    POIENUM GetIconType()
    {
        if(_iconInfo)
            return _iconInfo->_pmType;
        else
            return PM_UNKNOWN;
    };
    void SetKmlPathDetails( boost::shared_ptr<KmlPathDetails> pathDetails){_kmlPathDetails = pathDetails;}
    boost::shared_ptr<KmlPathDetails> GetKmlPathDetails( ){return _kmlPathDetails;}
    void SetVisible(bool visible){_visible = visible;}
    bool GetVisible(){return _visible;}
    bool GetEntitiesInitialized(){return _entitiesInitialized;};
private:
    //bool IsCurrentBillboardLevel(int currentBillboardLevel);
    bool _visible;
    boost::shared_ptr<KmlPathDetails> _kmlPathDetails;//kml custom symbols will use this
    virtual void SetUserSettings() ;
    boost::shared_ptr<iconInfo> _iconInfo;
    bool _entitiesInitialized;
    std::string _symbolText;
    double _textScale;
    boost::shared_ptr<openglfont> _openglFont;
    boost::shared_ptr<opengltext> _openglText;
//#if defined(__APPLE__)
    GLuint _symbolTexture;
    GLuint _symbolTextureWhite;
    GLuint _instanceVBO;
//#else
//    QOpenGLBuffer _arrayBuf;
//    boost::shared_ptr<QOpenGLTexture> _symbolTexture;
//    boost::shared_ptr<QOpenGLTexture> _symbolTextureWhite;
//#endif
    std::vector<float> _lineVertices;
     
    RotationTK _rotationTK;
    double _lat;
    double _lon;
    //boost::shared_ptr<symbolCartesianPoint> _cartesianBase;
    double _distToVehicle;
public:
    boost::shared_ptr<symbolCartesianPoint>  _cartesianBillboardPositions[32];//no offsets, just use index 2 for level2 and leave 0 and 1 empty
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL2;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL3;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL4;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL5;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL6;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL7;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL8;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL9;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL10;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL11;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL12;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL13;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL14;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL15;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL16;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL17;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL18;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL19;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL20;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL21;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL22;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL23;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL24;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL25;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL26;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL27;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL28;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL29;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL30;
//    boost::shared_ptr<symbolCartesianPoint> _cartesianL31;

    
};

#endif // SYMBOL

