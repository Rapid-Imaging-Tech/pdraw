


#ifndef STREETLINEMANAGER_H
#define STREETLINEMANAGER_H

#include "LoggableObject.h"
#include "coordinateconverter.h"
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
//#include "qopenglbuffer.h"
//#include "qopenglshaderprogram.h"
//#include "qopengltexture.h"
#include "vector_tile.pb.h"
#include "KMLEntities.h"
#include "elevationmanagerlocal.h"
#include "RIMatrixMath.hpp"
//#include "qopenglframebufferobject.h"
enum STLINECOLOR{STLNWHITE, STLNRED, STLNORANGE, STLNYELLOW, STLNGREEN, STLNMAGENTA, STLNBLUE, STLNPURPLE};

class StreetLineManager :  public LoggableConfigurableObject
{
private:
    double _streetWidth;
    bool _collar;
    void GetTexelColorLocation(STLINECOLOR color, float& texelX, float& texelY);
    STLINECOLOR GetColor(std::string strColor);
    STLINECOLOR _streetColorMinor;
    STLINECOLOR _streetColorService;
    STLINECOLOR _streetColorSecondary;
    STLINECOLOR _streetColorTertiary;
    STLINECOLOR _streetColorRail;
    STLINECOLOR _streetColorTrunk;
    STLINECOLOR _streetColorPrimary;
    STLINECOLOR _streetColorMotorway;

     //QOpenGLFramebufferObject* _qfbo;
    double _southLat ;
    double _northLat ;
    double _westLon ;
    double _eastLon ;

    double _cartxSW;
    double _cartySW;
    double _cartzSW;

    double _cartxNW;
    double _cartyNW;
    double _cartzNW;

    double _cartxNE;
    double _cartyNE;
    double _cartzNE;

    double _cartxSE;
    double _cartySE;
    double _cartzSE;
 
    GLuint _color_texture;
    GLuint _depth_texture;

    boost::shared_ptr<ElevationManagerLocal> _EML;

    void AssignElevationsAndXYZ(boost::shared_ptr<Kml::Vectors> vecs);
    boost::shared_ptr<std::vector<unsigned int> >  _vecStreetLinesMinor;
    boost::shared_ptr<std::vector<unsigned int> >  _vecStreetLinesService;
    boost::shared_ptr<std::vector<unsigned int> >  _vecStreetLinesSecondary;
    boost::shared_ptr<std::vector<unsigned int> >  _vecStreetLinesTertiary;
    boost::shared_ptr<std::vector<unsigned int> >  _vecStreetLinesRail;
    boost::shared_ptr<std::vector<unsigned int> >  _vecStreetLinesTrunk;
    boost::shared_ptr<std::vector<unsigned int> >  _vecStreetLinesPrimary;
    boost::shared_ptr<std::vector<unsigned int> >  _vecStreetLinesMotorway;
    void SortStreetLineTypes();
//#if defined(RESPONDAR)
    boost::shared_ptr<std::map<std::string, GLuint > > _symbolTextures;
    GLuint findTextureBasedOnHref(std::string href);
//#else
//    std::shared_ptr<std::map<std::string, std::shared_ptr<QOpenGLTexture> > > _symbolTextures;
//    std::shared_ptr<QOpenGLTexture> findTextureBasedOnHref(std::string href);
//#endif


    boost::shared_ptr<std::vector<float> > _streetLinesVertices;
    boost::shared_ptr<std::vector<unsigned int> > _streetLinesIndices;

    boost::shared_ptr<std::vector<float> > _streetLinesVerticesTexture;
    boost::shared_ptr<std::vector<unsigned int> > _streetLinesIndicesTexture;

    boost::shared_ptr<std::vector<float> > _streetLinesVerticesTriang;
    boost::shared_ptr<std::vector<unsigned int> > _streetLinesIndicesTriang;
//#if defined(RESPONDAR)
    GLuint _indexBufStreetLinesTriang;
    GLuint _arrayBufStreetLinesTriang;
//#else
//    QOpenGLBuffer _indexBufStreetLines;
//    QOpenGLBuffer _arrayBufStreetLines;
//    //QOpenGLBuffer _indexBufStreetLineTexture;
//    //QOpenGLBuffer _arrayBufStreetLineTexture;
//
//    QOpenGLBuffer _indexBufStreetLinesTriang;
//    QOpenGLBuffer _arrayBufStreetLinesTriang;
//#endif
    int _x;
    int _y;
    int _zoomLevel;
    boost::shared_ptr<CoordinateConverter> _coordConverter;
public:
    int SetEML(boost::shared_ptr<ElevationManagerLocal> eml) ;
    void addStreetLine(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry);
    bool SetupOpenglForEntities(GLuint program);
//#if defined(RESPONDAR)
//extern GLuint gbl_u_SamplerLocation ;
    bool Render(GLuint program, boost::shared_ptr<SViewData> viewData, RIMatrix4x4 projection, double pixelSize);
//#else
//    bool Render(QOpenGLShaderProgram* program, std::shared_ptr<SViewData> viewData,
//                                               QMatrix4x4 projection, double pixelSize);
//#endif
    void Render();
    StreetLineManager(int x, int y, int zoomLevel, boost::shared_ptr<UserSettingsData> usettingsData,
                                        boost::shared_ptr<StatusLogger> logger, boost::shared_ptr<CoordinateConverter> coordConverter, boost::shared_ptr<std::map<std::string, GLuint > > symbolTextures, double lineWidth, bool collar);
    virtual void SetUserSettings();
};

#endif // STREETLINEMANAGER_H


