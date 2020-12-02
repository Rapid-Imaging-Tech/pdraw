#ifndef RASTERTILE_H
#define RASTERTILE_H

//#ifndef RAVECTORTILE_H
//#define RAVECTORTILE_H
#if defined(__APPLE__)
#include "RIMatrixMath.hpp"
#else
#include <QOpenGLBuffer>
#include <qopenglshaderprogram.h>
#include <QOpenGLTexture>
#endif

#include "LFDefinitionsRA.h"
#include "utilities.h"

#include <boost/shared_ptr.hpp>
#include "KMLEntities.h"

#include <boost/thread.hpp>
#include "UserSettingsData.h"
#include <string>
#include "elevationmanagerlocal.h"
#include "symbol.h"
#include "boost/shared_array.hpp"
#include "coordinateconverter.h"
#include <vector_tile.pb.h>
#include "openglfont.h"
#include "tile.h"
class RasterTile : public tile, public LoggableConfigurableObject
{
private:
#if defined(__APPLE__)
    GLuint _indexVBO;
    GLuint _instanceVBO;
#else
    boost::shared_ptr<QOpenGLTexture> loadTextureWithFilename(std::string filename, std::string fileType, std::string folder);
#endif
    std::vector<unsigned int> _gridIndices;
    
    boost::shared_ptr<openglfont> _openglFont;
    std::string _retinaLevel;
    std::string _tileserverStyle;
    std::string _tileserverIP;
    std::string _tileserverPort;
    std::string _pngFolder;
     boost::shared_ptr<CoordinateConverter> _coordConverter;
     virtual void SetUserSettings() ;
public:
     virtual void OnNotifyCustomSymbolChange() ;
     virtual void AddSymbol() ;
     virtual void SortEntities(double vehLat, double vehLon);
     virtual void SetFont(boost::shared_ptr<openglfont> openglFont)
     {
         _openglFont = openglFont;
     };
    virtual void DataLoadFunction();
    std::string _rasterFileName;
    void SetupOpenglForEntities();
    void StartDataLoadThread();
    boost::shared_ptr<boost::thread> _dataLoadThread;
    bool _readyToSetupOpengl;
    bool _readyToRender;
#if defined(__APPLE__)
    GLuint _textureToBind;//todo: make a shared_ptr
    virtual bool RenderPerspectiveEntities(GLuint program, bool limitMet, boost::shared_ptr<SViewData> viewData, RIMatrix4x4 projection, double pixelSize);
    virtual bool RenderOrthographicEntities(GLuint program, bool limitMet, boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight, RIMatrix4x4 projection);
    //BoundingBox _boundingBox;
#else
    boost::shared_ptr<QOpenGLTexture> _textureToBind;//todo: make a shared_ptr
    virtual bool RenderPerspectiveEntities(QOpenGLShaderProgram* program, bool limitMet, boost::shared_ptr<SViewData> viewData, QMatrix4x4 projection,
                                           double pixelSize);
    virtual bool RenderOrthographicEntities(QOpenGLShaderProgram* program, bool limitMet, boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight, QMatrix4x4 projection);
    QOpenGLBuffer _arrayBuf;
    QOpenGLBuffer _indexBuf;
#endif    
//BoundingBox _boundingBox;
    ~RasterTile();
    RasterTile(int x, int y, int zoomLevel, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
               boost::shared_ptr<CoordinateConverter> coordConverter);
    void LoadVertexBuffers();
    
    //boost::shared_array<VertexDataLW> _tileGridVertices;
    std::vector<float> _fTileGridVertices;
    virtual int SetEML(boost::shared_ptr<ElevationManagerLocal> eml);
    boost::shared_ptr<ElevationManagerLocal> _EML;
    int BumpyTile(double lt1, double ln1, double lt2, double ln2, int nxycellcount);

};


//#endif // RAVECTORTILE_H


#endif // RASTERTILE_H
