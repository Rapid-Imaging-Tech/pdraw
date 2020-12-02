#define AUTOSCALETEXT
#include "pathway.h"


extern GLuint gbl_u_SamplerLocation ;
extern GLuint gbl_u_mvpLocation ;
extern GLuint gbl_u_colorLocation ;
pathway::pathway(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
                 boost::shared_ptr<CoordinateConverter> coordConverter, boost::shared_ptr<Kml::Vectors> spVector,
                 GLuint whiteTexture, boost::shared_ptr<openglfont> font, boost::shared_ptr<SViewData> viewData,
                 boost::shared_ptr<ElevationManagerLocal> eml,
                 std::string alignedText, int indexIntoUB, bool outsideTileN, bool outsideTileS, bool outsideTileE, bool outsideTileW ):
pathway::pathway( usettingsData, logger, coordConverter, spVector, whiteTexture, font, viewData, eml)

{
    _outsideTileN = outsideTileN;
    _outsideTileS = outsideTileS;
    _outsideTileE = outsideTileE;
    _outsideTileW = outsideTileW;
    _text = alignedText;
    #ifdef AUTOSCALETEXT
        _openglAutoAlignedText.reset(new openglautoalignedtext(font, 10.0, usettingsData,logger, _coordConverter, _EML, _text, spVector, indexIntoUB));
    #else
        _openglAlignedText.reset(new openglalignedtext(font, 10.0, usettingsData,logger, _coordConverter, _EML, _text, spVector));
    #endif
    
    //_openglAlignedText.reset(new openglalignedtext(font, 10.0, usettingsData,logger, _coordConverter, _EML, _text, spVector));
    _hasAlignedText = true;
}
bool pathway::StreetLabelLoadingComplete()
{
#if defined(AUTOSCALETEXT)
    return _openglAutoAlignedText->_loadingComplete;
#else
    return _openglAlignedText->_loadingComplete;
#endif
}

pathway::pathway(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
                 boost::shared_ptr<CoordinateConverter> coordConverter, boost::shared_ptr<Kml::Vectors> spVector,
                 GLuint whiteTexture, boost::shared_ptr<openglfont> font, boost::shared_ptr<SViewData> viewData,
                 boost::shared_ptr<ElevationManagerLocal> eml):
                    _kmlPathDetails(new KmlPathDetails),
                    _visible(true),
                    //_arrayBuf(QOpenGLBuffer::VertexBuffer),
                    _lineWidth(0.0),
                    _openglfont(font),
                    _hasAlignedText(false)
{
     _EML = eml;
    //called from thread pool when we grab a transportaion_name from the server
    _coordConverter = coordConverter;
    _whiteTexture = whiteTexture;
    LoggableConfigurableObject::Init(usettingsData, logger, "pathway");
    _spVectors = spVector;
    _pathColor = _spVectors->GetColor();
    _lineWidth = _spVectors->GetLineWidth();
    _pathVertices.clear();
    _kmlPathDetails = _spVectors->_kmlPathDetails;
    _viewData = viewData;
}
void pathway::AddLabelVertices(boost::shared_ptr<std::vector<float> > vecLabelVertices)
{
#if defined(AUTOSCALETEXT)
    if(_hasAlignedText)
        //DENSITY: ONLY NEED TO CALL THIS ONCE, THIS WILL SETUP VERTICES FOR THE TEXT CENTERED AT THE ORIGIN, THE SHADER WILL TRANSLATE/ROTATE AFTER THAT
        _openglAutoAlignedText->AddLabelVertices(vecLabelVertices);//add vertices to a master buffer for the given tile
#else
    if(_hasAlignedText)
        _openglAlignedText->AddLabelVertices(vecLabelVertices);
#endif
}

void pathway::DetectLabelCollisions(boost::shared_ptr<pathway> otherPathway)
{
    if(_hasAlignedText)
    {
        double maxScaleBeforeCollision = _openglAutoAlignedText->DetectLabelCollisions(otherPathway->_openglAutoAlignedText);
    }
}
void pathway::SetupOpenglForEntities(boost::shared_ptr<SViewData> viewData)
{
    _viewData = viewData;
    //int index = 0;
    std::vector<boost::shared_ptr<Kml::Point> > points = _spVectors->GetStreetVectors();
    for(int i = 0 ; i < points.size() ; i++)
    {
        boost::shared_ptr<Kml::Point> spPoint = points[i];

        //go through all vertices, and add to _pathVertices
        double x = 0.00;
        double y = 0.00;
        double z = 0.00;
        double lat = spPoint->GetLat();
        double lon = spPoint->GetLon();
        double elev = _EML->GetElevation(lat,lon);
        _coordConverter->GeodeticToCartesian(lat, lon, elev, x,y,z);
        
        double texelX = 0.5;
        double texelY = 0.5;

        _pathVertices.push_back((float)x);
        _pathVertices.push_back((float)y);
        _pathVertices.push_back((float)z);
        _pathVertices.push_back((float)texelX);
        _pathVertices.push_back((float)texelY);
        

    }

    glGenBuffers(1, &_instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, _pathVertices.size() * sizeof(float), &_pathVertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if(_hasAlignedText)
    {
        //setup openglalignedtext, send in spVector to align to and create opengl entities
        //_openglAlignedText->setTextOpenglEntities(_text, _spVectors, _viewData);
    }
}

void pathway::GetLabelIndices(boost::shared_ptr<std::vector<unsigned int> > indices, boost::shared_ptr<SViewData> vehicleSixDof,
                              double pixelSize, int densityLevel, double vehx, double vehy, double vehz)
{
#if defined(AUTOSCALETEXT)
    //int currentLevel = 1;
    if(_hasAlignedText)
        _openglAutoAlignedText->GetLabelIndices(indices, vehicleSixDof, pixelSize, densityLevel,  vehx,  vehy,  vehz);
#else
    if(_hasAlignedText)
        _openglAlignedText->GetLabelIndices(indices, vehicleSixDof, pixelSize);
#endif
}

pathway::~pathway()
{

}

void pathway::SetUserSettings()
{
   // cout<<"setting user settings for pathway"<<endl;
}


bool pathway::RenderPerspectiveEntities(GLuint program, boost::shared_ptr<SViewData> viewData, RIMatrix4x4 projectionMatrix)

{
     //TODO: ADD THE ALIGNED TEXT TO THIS PORTION

     _viewData = viewData;
     if(!_visible)
         return false;
     // _arrayBuf.bind();
     if(_hasAlignedText)
     {
        //_openglAlignedText->render(program, viewData, projectionMatrix);
     }

    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, _whiteTexture);


//    uint32_t get_red() const {
//      return color_abgr_ & 0x000000ff;
//    }


    int ired = _pathColor.get_red();
    int igreen = _pathColor.get_green();
    int iblue = _pathColor.get_blue();
    int ialpha = _pathColor.get_alpha();
    double dred = static_cast<double>(ired) / 255.0;
    double dgreen = static_cast<double>(igreen) / 255.0;
    double dblue = static_cast<double>(iblue) / 255.0;
    double dalpha = static_cast<double>(ialpha) / 255.0;


    RIVector4 vecColor(dred, dgreen, dblue, dalpha);
    //vmath::vec4 vecColor(dred, dgreen, dblue, (float)dalpha);
    glUniform4fv(gbl_u_colorLocation, 1, vecColor.get());//&vecColor[0]);
	glLineWidth(_lineWidth * 3);

    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    int vertexCoordLoc = glGetAttribLocation(program,"a_position");
    int textureCoordLoc = glGetAttribLocation(program,"a_texcoord");
    
    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    //GLuint vertexLoc = glGetAttribLocation(program, "InVertex");
    glEnableVertexAttribArray(vertexCoordLoc);
    glVertexAttribPointer(vertexCoordLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    
    glEnableVertexAttribArray(textureCoordLoc);
    glVertexAttribPointer(textureCoordLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    double numElements = _pathVertices.size() / 5;
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(numElements));
    RIVector4 vecColor2(dred, dgreen, dblue, (float)dalpha);
    //vmath::vec4 vecColor2(dred, dgreen, dblue, (float)dalpha);
    glUniform4fv(gbl_u_colorLocation, 1, vecColor2.get());

   return true;
 }
