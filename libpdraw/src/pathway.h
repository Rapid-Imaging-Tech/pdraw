#ifndef PATHWAY_H
#define PATHWAY_H

#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"
#include "kmlpathdetails.h"
#include "LFDefinitionsRA.h"
#include "elevationmanagerlocal.h"
#if defined(__APPLE__)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
//#include "vmath.h"
#include "RIMatrixMath.hpp"
#else
#include "qopengltexture.h"
#include "qopenglshaderprogram.h"
#include "qopenglbuffer.h"
#endif
#include "coordinateconverter.h"
#include "UserSettingsData.h"
#include "StatusLogger.h"

#include "KMLEntities.h"
#include "openglfont.h"
#include "openglalignedtext.h"
#include "openglautoalignedtext.hpp"
class pathway: public LoggableConfigurableObject
{
private:
    boost::shared_ptr<openglautoalignedtext> _openglAutoAlignedText;
    boost::shared_ptr<SViewData> _viewData;
    bool _hasAlignedText;
    std::string _text;
    boost::shared_ptr<openglfont> _openglfont;
    boost::shared_ptr<openglalignedtext> _openglAlignedText;
    double _lineWidth;
    kmlbase::Color32 _pathColor;
    boost::shared_ptr<Kml::Vectors> _spVectors;
    bool _visible;
    boost::shared_ptr<KmlPathDetails> _kmlPathDetails;//kml custom symbols will use this
    //load white texture
#if defined(__APPLE__)
    GLuint _instanceVBO;
    GLuint _whiteTexture;
    GLuint loadTextureWithFilename(std::string filename, std::string fileType, std::string folder);
#else
    boost::shared_ptr<QOpenGLTexture> loadTextureWithFilename(std::string filename, std::string fileType, std::string folder);
    boost::shared_ptr<QOpenGLTexture> _whiteTexture;
#endif
    
    std::string _pngFolder;
     boost::shared_ptr<CoordinateConverter> _coordConverter;
     //boost::shared_array<VertexDataLW> _pathVertices;
     std::vector<float> _pathVertices;
     
protected:
     virtual void SetUserSettings() ;
public:
    void DetectLabelCollisions(boost::shared_ptr<pathway> otherPathway);
    bool _outsideTileN;
    bool _outsideTileS;
    bool _outsideTileE;
    bool _outsideTileW;
     bool StreetLabelLoadingComplete();
     void GetLabelIndices(boost::shared_ptr<std::vector<unsigned int> > indices,  boost::shared_ptr<SViewData> vehicleSixDof,
                          double pixelSize, int densityLevel, double vehx, double vehy, double vehz);
     void AddLabelVertices(boost::shared_ptr<std::vector<float> > vecLabelVertices);
     void SetText(std::string text);
     boost::shared_ptr<KmlPathDetails> GetPathDetails(){return _kmlPathDetails;}
     void SetVisible(bool visible){_visible = visible;}
    void DataLoadFunction();
    void SetupOpenglForEntities(boost::shared_ptr<SViewData> viewData);
    bool _readyToRender;
#if defined(__APPLE__)
    boost::shared_ptr<GLuint> _textureToBind;//todo: make a shared_ptr
    bool RenderPerspectiveEntities(GLuint program, boost::shared_ptr<SViewData> viewData, RIMatrix4x4 projectionMatrix);
    pathway(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
            boost::shared_ptr<CoordinateConverter> coordConverter,
            boost::shared_ptr<Kml::Vectors> spVector,
            GLuint whiteTexture, boost::shared_ptr<openglfont> font,  boost::shared_ptr<SViewData> viewData,
            boost::shared_ptr<ElevationManagerLocal> eml);
    pathway(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
            boost::shared_ptr<CoordinateConverter> coordConverter, boost::shared_ptr<Kml::Vectors> spVector,
            GLuint whiteTexture, boost::shared_ptr<openglfont> font, boost::shared_ptr<SViewData> viewData,
            boost::shared_ptr<ElevationManagerLocal> eml,
            std::string alignedText, int indexIntoUB, bool outsideTileN, bool outsideTileS, bool outsideTileE, bool outsideTileW );
    GLuint _arrayBuf;
    GLuint _indexBuf;
#else
    boost::shared_ptr<QOpenGLTexture> _textureToBind;//todo: make a shared_ptr
    bool RenderPerspectiveEntities(QOpenGLShaderProgram* program, boost::shared_ptr<SViewData> viewData, QMatrix4x4 projectionMatrix);
    //boost::shared_ptr<QOpenGLTexture> whiteTexture, boost::shared_ptr<openglfont> font,  boost::shared_ptr<SViewData> viewData,
    pathway(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
               boost::shared_ptr<CoordinateConverter> coordConverter,
            boost::shared_ptr<Kml::Vectors> spVector,
            boost::shared_ptr<QOpenGLTexture> whiteTexture, boost::shared_ptr<openglfont> font,  boost::shared_ptr<SViewData> viewData,
            boost::shared_ptr<ElevationManagerLocal> eml);
    pathway(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
                     boost::shared_ptr<CoordinateConverter> coordConverter, boost::shared_ptr<Kml::Vectors> spVector,
                           boost::shared_ptr<QOpenGLTexture> whiteTexture, boost::shared_ptr<openglfont> font, boost::shared_ptr<SViewData> viewData,
            boost::shared_ptr<ElevationManagerLocal> eml,
                     std::string alignedText);
    QOpenGLBuffer _arrayBuf;
    QOpenGLBuffer _indexBuf;
#endif
    ~pathway();
    
    void LoadVertexBuffers();
    
    //boost::shared_array<VertexDataLW> _tileGridVertices;
    //int SetEML(boost::shared_ptr<ElevationManagerLocal> eml){_EML = eml;}
    boost::shared_ptr<ElevationManagerLocal> _EML;
    //BumpyTile(double lt1, double ln1, double lt2, double ln2, int nxycellcount);
};

#endif // PATHWAY_H
