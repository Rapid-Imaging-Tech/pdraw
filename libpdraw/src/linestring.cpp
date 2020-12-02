#include "linestring.h"
#include <symbol.h>
#include <iostream>
#include "RotationTK.h"
#include "utilities.h"
#define ICONSIZE 15.0
#define FLAGPOLEOFFSET 50.0
using namespace std;

void linestring::SetUserSettings(){

//    if(!(_userSettingsData->_intSettings.find("TEXTSCALE") == _userSettingsData->_intSettings.end())){
//        int iTextScale = _userSettingsData->_intSettings["TEXTSCALE"];
//        _textScale = static_cast<double>(iTextScale) / 1000.0;
//    }
//    else
//        this->_textScale = 10.0;
}

void linestring::InitOpenGLEntities()//boost::shared_ptr<UserSettingsData> usettingsData)
{
    //WriteToStatusLog("createing symbol", false, true);
    _lineVertices.reset(new VertexDataLW[6]);
    _lineVertices[0].position.setX(((float) -ICONSIZE));
    _lineVertices[0].position.setY(((float) (ICONSIZE * 2) ));
    _lineVertices[0].position.setZ(((float) -4.0));
    //we're just using a pixel since this is a solid color
    _lineVertices[0].texCoord.setX(0.0);
    _lineVertices[0].texCoord.setY(1.0);

    //TS: set these up as 2 triangles
    _lineVertices[1].position.setX(((float) -ICONSIZE));
    _lineVertices[1].position.setY(((float) 0.0 ));
    _lineVertices[1].position.setZ(((float) -4.0));
    //we're just using a pixel since this is a solid color
    _lineVertices[1].texCoord.setX(0.0);
    _lineVertices[1].texCoord.setY(0.0);


    _lineVertices[2].position.setX(((float) ICONSIZE));
    _lineVertices[2].position.setY(((float) (ICONSIZE * 2) ));
    _lineVertices[2].position.setZ(((float) -4.0));
    //we're just using a pixel since this is a solid color
    _lineVertices[2].texCoord.setX(1.0);
    _lineVertices[2].texCoord.setY(1.0);

    _lineVertices[3].position.setX(((float) ICONSIZE));
    _lineVertices[3].position.setY(((float) 0.0 ));
    _lineVertices[3].position.setZ(((float) -4.0));
    //we're just using a pixel since this is a solid color
    _lineVertices[3].texCoord.setX(1.0);
    _lineVertices[3].texCoord.setY(0.0);

    //draw flagpole
    _lineVertices[4].position.setX(((float) 0.0));
    _lineVertices[4].position.setY(((float) 0.0 ));
    _lineVertices[4].position.setZ(((float) -4.0));
    //we're just using a pixel since this is a solid color
    _lineVertices[4].texCoord.setX(0.5);
    _lineVertices[4].texCoord.setY(0.5);

    _lineVertices[5].position.setX(((float) 0.0));
    _lineVertices[5].position.setY(((float) FLAGPOLEOFFSET));
    _lineVertices[5].position.setZ(((float) -4.0));
    //we're just using a pixel since this is a solid color
    _lineVertices[5].texCoord.setX(0.6);
    _lineVertices[5].texCoord.setY(0.6);


    //Tegra VideoPlayer stuff
    _arrayBuf.create();
    _arrayBuf.bind();
    _arrayBuf.allocate(_lineVertices.get(), 6 * sizeof(VertexDataLW));
    //_openglText->setText(_symbolText);
    _entitiesInitialized = true;
}

void linestring::UpdateDistToVehicle(double vehLat, double vehLon)
{
    //_distToVehicle = distLatLonQuick(vehLat, _lat, vehLon, _lon);
}

linestring::~linestring()
{
    if(_arrayBuf.isCreated())
        _arrayBuf.destroy();
}

linestring::linestring(std::string text, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
               boost::shared_ptr<QOpenGLTexture> whiteTexture, boost::shared_ptr<openglfont> font):
    _textScale(10.0),
    _entitiesInitialized(false),
    _distToVehicle(0.00),
//    _rotationTK(usettingsData, logger),
//    _kmlPathDetails(new KmlPathDetails),
    _visible(true)


{
    LoggableConfigurableObject::Init(usettingsData, logger, "Symbol");
    //_iconInfo = icInfo;
    //_userSettingsData = usettingsData;

//    if(!(_userSettingsData->_intSettings.find("TEXTSCALE") == _userSettingsData->_intSettings.end())){
//        int iTextScale = _userSettingsData->_intSettings["TEXTSCALE"];
//        _textScale = static_cast<double>(iTextScale) / 1000.0;
//    }
//    else
//        this->_textScale = 10.0;

    _openglFont = font;
    _openglText.reset(new opengltext(_openglFont, _textScale, usettingsData, logger));

    _labelText = text;
    //_symbolTexture = texture;

    _textureWhite = whiteTexture;
    //_lat = lat;
    //_lon = lon;
    //_xcartesian = xcartesian;
    //_ycartesian = ycartesian;
    //_zcartesian = zcartesian;

};

void linestring::render(boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight, QOpenGLShaderProgram* program,  double vehx, double vehy, double vehz, QMatrix4x4 projection)
{
    if(!_visible)
        return ;
    if(!_entitiesInitialized)
        return;
    //take p/h/r use the new rotationtk getpixel4geo routine.  At this point we should be in ortho space
    //so just draw to x,y for this symbol

    //double px = _xcartesian - vehx;
    //double py = _ycartesian - vehy;
    //double pz = _zcartesian - vehz;

    double vp = 0.00;
    double vh = 0.00;
    double vr = 0.00;

    double cp = viewData->dCameraPitch;
    double ch = viewData->dCameraHeading;
    double cr = viewData->dCameraRoll;

    double hfov = viewData->dFovHorizontalAngle;
    double vfov = viewData->dFovVerticalAngle;
    double nx = windowWidth;
    double ny = windowHeight;

    double sx = 0.00;
    double sy = 0.00;
    double distance = 0.00;
    double camcart[3],cart[3];

    camcart[0] = vehx;
    camcart[1] = vehy;
    camcart[2] = vehz;
    //cart[0] = _xcartesian;
    //cart[1] = _ycartesian;
    //cart[2] = _zcartesian;

//    _rotationTK.SetCamera(camcart,-cp,ch,cr);
//    _rotationTK.SetFOVs(hfov,vfov);
//    _rotationTK.ResizeWindow(nx,ny);
//    _rotationTK.LoadMatrices();
    //QVector4D sc = _rotationTK.GetPixels4Cart(cart);
    //sx = sc[0];
    //sy = sc[1];

    if(sx == -1 && sy == -1)
        return;//these are behind the camera and should not draw
    //for now lets assume 100, 100 as screen coordinates unti pixel4geo is implemented
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

    //if(_iconInfo->_pmType == PM_CUSTOM && _customTexture)
    //{
      //  _customTexture->bind();
    //}
    //else if(_iconInfo->_pmType == PM_CUSTOM && !_customTexture)
    //{
    //    WriteToStatusLog("cannot find custom symbol", true, true);
    //}
    //else
    //{
        //_symbolTexture->bind();
    //}
    //_symbolTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    //_symbolTexture->setMinificationFilter(QOpenGLTexture::Linear);
    _arrayBuf.bind();

    //double dgdCamPitch = cp + 90.0;
    //double scale = dgdCamPitch / 90.0;

    //double scaledFlagPoleHeight = scale * FLAGPOLEOFFSET;

    //QMatrix4x4 matrix;
    //matrix.setToIdentity();
    //matrix.translate((GLdouble) sx ,(GLdouble) (sy + scaledFlagPoleHeight),(GLdouble) 0.0);
    //program->setUniformValue("mvp_matrix", projection * matrix);
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

     _textureWhite->bind();

     //matrix.translate((GLdouble) 0.0 ,(GLdouble) (-1.0 * (scaledFlagPoleHeight)),(GLdouble) 0.0);

     glLineWidth(1.0);
     //matrix.scale(1.0, scale, 1.0);
     //program->setUniformValue("mvp_matrix", projection * matrix);
     glDrawArrays(GL_LINE_STRIP, 4, 2);
     /////////start draw text
      //move up an additional ICONSIZE/height + another 10 se we're above the symbol
      double textOffset = ICONSIZE * 2 + 10;
      //matrix.setToIdentity();
      //matrix.translate((GLdouble)sx, (GLdouble) (sy + textOffset + scaledFlagPoleHeight), (GLdouble)0.0);//move up in the y direction for text

      //matrix.scale(0.5);
      //program->setUniformValue("mvp_matrix", projection * matrix);
      _openglText->render(program);
      ///////////end draw text
}
