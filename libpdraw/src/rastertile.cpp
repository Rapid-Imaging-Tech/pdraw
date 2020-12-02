//#define CLOCK_REALTIME
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "vector_tile.pb.h"
#pragma GCC diagnostic pop
#if defined(__APPLE__)
#include "vector_tile_compression.hpp"
#else
#include "vectortile/vector_tile_compression.hpp"
#endif
#include "geodatacachesettings.h"
#include "boost/asio.hpp"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

#include "geovectileparser.h"

#include "rastertile.h"
#include "rafootprint.h"
#include "utilities.h"

#include <math.h>
#include "geotileservergl.h"
#if defined(__APPLE__)
#include "objcUtilityWrapper.h"
extern GLuint gbl_u_SamplerLocation ;
extern GLuint gbl_u_mvpLocation ;
extern GLuint gbl_u_colorLocation ;
#else
#include </usr/include/png.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QOpenGLTexture>
#endif


#include <sys/stat.h>

void RasterTile::OnNotifyCustomSymbolChange()
{
    //probably do nothing here since we don't have any symbols, only raster images of the overlay tiles
}
 void RasterTile::SetUserSettings()
{

     using namespace boost::filesystem;
     _pngFolder = "";
     //_usettingsdata = usettingsData;
     if(!_userSettingsData->_stringSettings["PNGFOLDER"].empty())
         this->_pngFolder = _userSettingsData->_stringSettings["PNGFOLDER"];
     else
         this->_pngFolder = "";
     //if png folder doesn't exist try to create it
         path p(this->_pngFolder);
       try
       {
         if (create_directory(p))
         {
         }
       }
       catch (filesystem_error &e)
       {
         std::cerr << e.what() << '\n';
       }

     if(!_userSettingsData->_stringSettings["TILESERVERIP"].empty())
         this->_tileserverIP = _userSettingsData->_stringSettings["TILESERVERIP"];
     else
         this->_tileserverIP = "localhost";

     if(!_userSettingsData->_stringSettings["TILESERVERPORT"].empty())
         this->_tileserverPort = _userSettingsData->_stringSettings["TILESERVERPORT"];
     else
         this->_tileserverPort = "8080";

     if(!_userSettingsData->_stringSettings["TILESERVERSTYLE"].empty())
         this->_tileserverStyle = _userSettingsData->_stringSettings["TILESERVERSTYLE"];
     else
         this->_tileserverStyle = "/styles/klokantech-basic/";

     if(!(_userSettingsData->_intSettings.find("RETINALEVEL") == _userSettingsData->_intSettings.end())){
         int level = _userSettingsData->_intSettings["RETINALEVEL"];
         switch(level){
         case 1:
             this->_retinaLevel = "";
             break;
         case 2:
             this->_retinaLevel = "@2x";
             break;
         case 3:
             this->_retinaLevel = "@3x";
             break;
         case 4:
             this->_retinaLevel = "@4x";
             break;
         }
     }
     else
         this->_retinaLevel = "";

     std::string islogging(_userSettingsData->_stringSettings["CLEARCACHEFOLDER"]);
     if(islogging.compare("TRUE") == 0)
     {
         _clearCacheFolder = true;
     }
     else
         _clearCacheFolder = false;
}

void RasterTile::SortEntities(double vehLat, double vehLon)
{
    //TODO: IF WE EVER HAVE OPENGL ENTITIES ENCAPSULATED IN RASTERTILE, AND WE NEED TO SORT
    //NEAREST TO FARTHEST FROM VEHICLE, DO IT HERE
    UpdateSortOrderNum();
}

RasterTile::~RasterTile()
{
    //cout<<"destro RasterTile"<<endl;
    if(_clearCacheFolder)
    {
        std::string strError;
        bool success = DeleteFile(_rasterFileName, strError);
        if(!success)
        {
            //WriteToStatusLog(strError, true, true);
        }
    }
}

RasterTile::RasterTile(int x, int y, int zoomLevel, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
                       boost::shared_ptr<CoordinateConverter> coordConverter):
    _readyToSetupOpengl(false),
    _readyToRender(false),
    tile(x, y, zoomLevel)
#if defined(__APPLE__)
#else
    , _arrayBuf(QOpenGLBuffer::VertexBuffer),
    _indexBuf(QOpenGLBuffer::IndexBuffer)
#endif
{

    LoggableConfigurableObject::Init(usettingsData, logger, "RasterTile");
    _coordConverter = coordConverter;
    _retinaLevel = "";
    //_x = x;
    //_y = y;
    //_zoomLevel = zoomLevel;
    //_boundingBox = tile2boundingBox(x, y, zoomLevel);
}
#if defined(__APPLE__)
bool RasterTile::RenderOrthographicEntities(GLuint program, bool limitMet, boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight, RIMatrix4x4 projection)
#else
bool RasterTile::RenderOrthographicEntities(QOpenGLShaderProgram* program, bool limitMet, boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight, QMatrix4x4 projection)
#endif
{
    return true;
}
void RasterTile::AddSymbol()
{
    //lfclient is adding a custom symbol from a kml file, in raster tile probably do nothing
}

#if defined(__APPLE__)
bool RasterTile::RenderPerspectiveEntities(GLuint program, bool limitMet, boost::shared_ptr<SViewData> viewData, RIMatrix4x4 projection,
                                           double pixelSize)
#else
bool RasterTile::RenderPerspectiveEntities(QOpenGLShaderProgram* program, bool limitMet, boost::shared_ptr<SViewData> viewData, QMatrix4x4 projection,
                                           double pixelSize)
#endif
{


    bool createdGLEntities = false;
    if(!_readyToSetupOpengl && !_readyToRender){
        return createdGLEntities; //not loaded and not ready yet, don't render
    }
    else if(limitMet && _readyToSetupOpengl && !_readyToRender){
        //don't attempt to setup opengl stuff cause we're clogging up the pipeline,
        //just return, and eventually we'll get to this
        return createdGLEntities;
    }
    else if(_readyToSetupOpengl && !_readyToRender ){
        SetupOpenglForEntities();
        createdGLEntities = true;//let lfclient know that we created some entities so it
        //can keep track and limit the amount of them done so we don't bog down the cpu
        _readyToRender = true;
    }
//return true;//todo: remove
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
#if !defined(__APPLE__)
    glEnable(GL_MULTISAMPLE);
#endif
    if(!_textureToBind){
        return createdGLEntities;
    }
#if defined(__APPLE__)

    // Bind the base map
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, _textureToBind);
    
    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    
    // Set the base map sampler to texture unit to 0
    glUniform1i ( gbl_u_SamplerLocation, 0 );
    
    int vertexCoordLoc = glGetAttribLocation(program,"a_position");
    
    
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int textureCoordLoc = glGetAttribLocation(program,"a_texCoord");
    
    glVertexAttribPointer ( vertexCoordLoc, 3, GL_FLOAT,
                                                      GL_FALSE, 5 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));//_tileGridVertices
    glEnableVertexAttribArray ( vertexCoordLoc );
    
   glVertexAttribPointer ( textureCoordLoc, 2, GL_FLOAT,
                          //GL_FALSE, 5 * sizeof(GLfloat), &(_tileGridVertices[3]) );//_tileGridVertices
                          GL_FALSE, 5 * sizeof(GLfloat),  (void*)(3 * sizeof(GLfloat)));//_tileGridVertices
   glEnableVertexAttribArray ( textureCoordLoc );
    
    
    
    // Offset for texture coordinate
    
    
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    //int texcoordLocation = 0;//TODO: program->attributeLocation("a_texcoord");
    //program->enableAttributeArray(texcoordLocation);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVBO);
#else
    _textureToBind->bind();

    _textureToBind->setMagnificationFilter(QOpenGLTexture::Linear);
    _textureToBind->setMinificationFilter(QOpenGLTexture::Linear);
    _indexBuf.bind();
    //_indexBuf.allocate(&_gridIndices[0], _gridIndices.size() * sizeof(unsigned int));
    _arrayBuf.bind();

    int offset = 0;
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexDataLW));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexDataLW));
#endif
    glDrawElements(
         GL_TRIANGLES,      // mode
         _gridIndices.size(),    // count
         GL_UNSIGNED_INT,   // type
         (void*)0           // element array buffer offset
     );

    return createdGLEntities;
}

#if !defined(__APPLE__)
boost::shared_ptr<QOpenGLTexture> RasterTile::loadTextureWithFilename(std::string filename, std::string fileType, std::string folder)
{
    std::string errorMsg;
    if(!FileExists(filename, errorMsg))
    {
        WriteToStatusLog(errorMsg, true, true);
        std::string rismsg("can't find file ");
        rismsg.append(filename);
        return 0x00;
    }
    boost::shared_ptr<QOpenGLTexture> textureRef;// = 0x00;
    textureRef.reset();

    textureRef.reset(new QOpenGLTexture(QImage(filename.c_str()).mirrored()));
    if(textureRef == 0x00)
    {
        std::string rismsg("can't create texture from: ");
        rismsg.append(filename);
        return 0x00;
    }
    try
    {
    // Set nearest filtering mode for texture minification
    // Set bilinear filtering mode for texture magnification
    textureRef->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    textureRef->setWrapMode(QOpenGLTexture::MirroredRepeat);
    }
    catch(...)
    {
        std::string rismsg("seg fault in setminification: ");
    }

    return textureRef;
}
#endif

void RasterTile::SetupOpenglForEntities()
{
    WriteToStatusLog("setting up opengl entities", false, false);
#if defined(__APPLE__)
    LFObjCFunctionsCPPWrapper ocWrapper;
    int width = 0;
    int height = 0;
    //create texture of raster image of raster returned from server
    GLubyte* imageData = ocWrapper.loadpng(_rasterFileName, width, height);
    glGenTextures(1, &_textureToBind);
    glBindTexture(GL_TEXTURE_2D, _textureToBind);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    
    
    glGenBuffers(1, &_instanceVBO);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    // Give our vertices to OpenGL.
    //glBufferData(GL_ARRAY_BUFFER, sizeof(lftestdataraster), lftestdataraster, GL_STATIC_DRAW);
    
    //glGenBuffers(1, &_instanceVBO);
    //glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, (BUMPYGRIDSIZE+1) * (BUMPYGRIDSIZE+1) * sizeof(float) * 5, &_fTileGridVertices[0]/*&_tileGridVertices[0]*/, GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &_indexVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _gridIndices.size() * sizeof(unsigned int), &_gridIndices[0], GL_STATIC_DRAW);
    
    //_textureToBind = loadTextureWithFilename(_rasterFileName, "png", "");
#else
    //these 3 lines need to be on main thread, other than that all the parsing can be done on
    //a seperate thread to smooth things out
    _arrayBuf.create();
    _arrayBuf.bind();
    _arrayBuf.allocate(&_fTileGridVertices[0], (BUMPYGRIDSIZE + 1) * (BUMPYGRIDSIZE + 1) *  5 * sizeof(float));

    _indexBuf.create();
    _indexBuf.bind();
    _indexBuf.allocate(&_gridIndices[0], _gridIndices.size() * sizeof(unsigned int));

    _textureToBind = loadTextureWithFilename(_rasterFileName, "png", "");
#endif
    if(_clearCacheFolder)
    {
        std::string strError;
        bool success = DeleteFile(_rasterFileName, strError);
        if(!success)
        {
            WriteToStatusLog(strError, true, true);
        }
    }
}
extern std::string GetDataDirectory();
void RasterTile::DataLoadFunction()
{
    
    boost::asio::io_service io_service_;
    boost::asio::io_service io_service2_;
    std::stringstream pngTileSS;
    std::stringstream vtsTileSS;
    std::stringstream pngFileName;
    std::stringstream pbfFileName;
    
    //png filename for street lines/names
    pngFileName<<_pngFolder<<_zoomLevel<<"-"<<_x<<"-"<<_y<<_retinaLevel<<".png";
    pngTileSS<<_tileserverStyle<<_zoomLevel<<"/"<<_x<<"/"<<_y<<_retinaLevel<<".png";
    std::string pngss(pngTileSS.str());
    std::string png(pngFileName.str());

    std::string rasterPath = GetDataDirectory();
    rasterPath.append("/");
    rasterPath.append(png);
    std::string errorMsg;
    if(!FileExists(rasterPath, errorMsg))
    {
        //std::string rismsg("can't find file ");
        //rismsg.append(fullPath);
        //WriteToStatusLog(errorMsg, true, true);
        //this->WriteToStatusLog(rismsg, true, true);
        //return 0x00;
        
        //if file doesn't exist, lets go to the tileserver to grab it
//        geotileservergl tileserver2(io_service2_, _tileserverIP, _tileserverPort, pbfss, pbf, _userSettingsData, _statusLogger);//TS: GONNA NEED TO GRAB ADDRESS FROM SETTINGS.INI FILE
//        io_service2_.run();
//        io_service2_.stop();
        geotileservergl tileserver(io_service_, _tileserverIP, _tileserverPort, pngss, png, _userSettingsData, _statusLogger);//TS: GONNA NEED TO GRAB ADDRESS FROM SETTINGS.INI FILE
        io_service_.run();
        io_service_.stop();
    }

    

    //name of png file is pbfFileName
    int width = 0;
    int height = 0;
    _rasterFileName = pngFileName.str();

    _fTileGridVertices.clear();

    BumpyTile(_boundingBox.south, _boundingBox.west, _boundingBox.north,_boundingBox.east, BUMPYGRIDSIZE);
    _readyToSetupOpengl = true;
    //    //these 3 lines need to be on main thread, other than that all the parsing can be done on
    //    //a seperate thread to smooth things out
}

int RasterTile::SetEML(boost::shared_ptr<ElevationManagerLocal> eml) {
    _EML = eml;
    return 1;
}

 int RasterTile::BumpyTile(double lat1, double lon1, double lat2, double lon2, int nxycellcount)
{

    double deltaLat = (lat2-lat1) / nxycellcount;//distance between grid points in lat
    double deltaLon = (lon2-lon1) / nxycellcount;//distance between grid points in lon
    
    int numberOfPointsInGrid = (nxycellcount + 1) * (nxycellcount + 1);//total number of points needed to cover grid, at border and interior
    int index = 0;
    
    double deltaTexel = 1.0 / static_cast<double>(nxycellcount);
    for(int ilat = 0 ; ilat < nxycellcount + 1 ; ilat++)
    {
        for(int ilon = 0 ; ilon < nxycellcount + 1 ; ilon++)
        {
            double x = 0.00;
            double y = 0.00;
            double z = 0.00;
            double lat = ilat * deltaLat + lat1;
            double lon = ilon * deltaLon + lon1;
            double elev = _EML->GetElevation(lat,lon);
            _coordConverter->GeodeticToCartesian(lat, lon, elev, x,y,z);

            double texelX = ilon * deltaTexel;
#if defined(__APPLE__)
            double texelY = 1.0 - ilat * deltaTexel;
#else
            double texelY = ilat * deltaTexel;
#endif
             _fTileGridVertices.push_back((float)x);
            _fTileGridVertices.push_back((float)y);
            _fTileGridVertices.push_back((float)z);
            _fTileGridVertices.push_back((float)texelX);
            _fTileGridVertices.push_back((float)texelY);
            //_tileGridVertices[index].position[0] = (float) x;
            //_tileGridVertices[index].position[1] = (float) y;
            //_tileGridVertices[index].position[2] = (float) z;
            //_tileGridVertices[index].texCoord[0] = (float)texelX;
            //_tileGridVertices[index].texCoord[1] = (float)texelY;
            index++;
        }
    }

    //2nd row of the grid
    for(int row = 0 ; row < nxycellcount ; row++)
    {
    for(int ix = 0 ; ix < nxycellcount; ix++)
    {
        int offsetFirstRow = row * (nxycellcount + 1);
        int offsetSecondRow = (row + 1) * (nxycellcount + 1);

        //triangle 1
        _gridIndices.push_back(ix + offsetFirstRow);
        _gridIndices.push_back(ix + offsetSecondRow);
        _gridIndices.push_back(ix + 1 + offsetFirstRow);

        //triangle 2
        _gridIndices.push_back(ix + 1 + offsetFirstRow);
        _gridIndices.push_back(ix + offsetSecondRow);
        _gridIndices.push_back(ix + 1 + offsetSecondRow);

    }
    }
     return 1;
 }

