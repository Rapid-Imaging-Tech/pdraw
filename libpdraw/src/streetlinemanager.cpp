


//
//  streetlinemanager.cpp
//  SmartCam
//
//  Created by David Dvergsten on 9/24/19.
//  Copyright © 2019 Rapid Imaging Technologies. All rights reserved.
//

#include "streetlinemanager.hpp"
#include "streetlinetriangulation.hpp"
//#include "qopengltexture.h"
#include "geovectileparser.h"
#include "boost/thread/mutex.hpp"
#include "utilities.h"
boost::mutex missingIconMutexSL;
std::vector<std::string> _missingIconsSL;


StreetLineManager::StreetLineManager(int x, int y, int zoomLevel, boost::shared_ptr<UserSettingsData> usettingsData,
                                    boost::shared_ptr<StatusLogger> logger,
                                    boost::shared_ptr<CoordinateConverter> coordConverter, boost::shared_ptr<std::map<std::string,
                                     GLuint > > symbolTextures, double lineWidth, bool collar):
    _x(x),
    _y(y),
    _zoomLevel(zoomLevel),

    //_arrayBufStreetLines(QOpenGLBuffer::VertexBuffer),
    //_indexBufStreetLines(QOpenGLBuffer::IndexBuffer),
    //_arrayBufStreetLinesTriang(QOpenGLBuffer::VertexBuffer),
    //_indexBufStreetLinesTriang(QOpenGLBuffer::IndexBuffer),
    _streetColorMinor(STLNWHITE),
    _streetColorService(STLNWHITE),
    _streetColorSecondary(STLNWHITE),
    _streetColorTertiary(STLNWHITE),
    _streetColorRail(STLNWHITE),
    _streetColorTrunk(STLNWHITE),
    _streetColorPrimary(STLNWHITE),
    _streetColorMotorway(STLNWHITE),
    _streetWidth(lineWidth),
    _collar(collar)
{
    _symbolTextures = symbolTextures;
    _vecStreetLinesMinor.reset(new std::vector<unsigned int> );
    _vecStreetLinesService.reset(new std::vector<unsigned int> );
    _vecStreetLinesSecondary.reset(new std::vector<unsigned int> );
    _vecStreetLinesTertiary.reset(new std::vector<unsigned int> );
    _vecStreetLinesRail.reset(new std::vector<unsigned int> );
    _vecStreetLinesTrunk.reset(new std::vector<unsigned int> );
    _vecStreetLinesPrimary.reset(new std::vector<unsigned int> );
    _vecStreetLinesMotorway.reset(new std::vector<unsigned int> );


  _streetLinesVertices.reset(new std::vector<float>);
  _streetLinesIndices.reset(new std::vector<unsigned int>);

  _streetLinesVerticesTriang.reset(new std::vector<float>);
  _streetLinesIndicesTriang.reset(new std::vector<unsigned int>);

    LoggableConfigurableObject::Init(usettingsData, logger, "VectorTile");
    _coordConverter = coordConverter;
}

STLINECOLOR StreetLineManager::GetColor(std::string strColor)
{/*
    MAGENTA
    STCOLORSERVICE YELLOW
    STCOLORSECONDARY ORANGE
    STCOLORTERTIARY RED
    STCOLORRAIL GREEN
    STCOLORTRUNK WHITE
    STCOLORPRIMARY BLUE
    STCOLORMOTORWAY PURPLE*/
    //convert string value to STLINECOLOR;
    STLINECOLOR color = STLNWHITE;
    if(strColor.compare("YELLOW") == 0){
        color = STLNYELLOW;
    }
    else if(strColor.compare("ORANGE") == 0){
        color = STLNORANGE;
    }
    else if(strColor.compare("RED") == 0){
        color = STLNRED;
    }
    else if(strColor.compare("GREEN") == 0){
        color = STLNGREEN;
    }
    else if(strColor.compare("WHITE") == 0){
        color = STLNWHITE;
    }
    else if(strColor.compare("BLUE") == 0){
        color = STLNBLUE;
    }
    else if(strColor.compare("PURPLE") == 0){
        color = STLNPURPLE;
    }
    else if(strColor.compare("MAGENTA") == 0){
        color = STLNMAGENTA;
    }
    return color;
}

void StreetLineManager::SetUserSettings()
{
//    STCOLORMINOR MAGENTA
//    STCOLORSERVICE YELLOW
//    STCOLORSECONDARY ORANGE
//    STCOLORTERTIARY RED
//    STCOLORRAIL GREEN
//    STCOLORTRUNK WHITE
//    STCOLORPRIMARY BLUE
//    STCOLORMOTORWAY PURPLE

    if(!_userSettingsData->_stringSettings["STCOLORMINOR"].empty())
    {
        this->_streetColorMinor = GetColor(_userSettingsData->_stringSettings["STCOLORMINOR"]);
    }
    else
        this->_streetColorMinor = STLNWHITE;

    if(!_userSettingsData->_stringSettings["STCOLORSERVICE"].empty())
        this->_streetColorService = GetColor(_userSettingsData->_stringSettings["STCOLORSERVICE"]);
    else
        this->_streetColorService = STLNWHITE;

    if(!_userSettingsData->_stringSettings["STCOLORSECONDARY"].empty())
        this->_streetColorSecondary = GetColor(_userSettingsData->_stringSettings["STCOLORSECONDARY"]);
    else
        this->_streetColorSecondary = STLNWHITE;

    if(!_userSettingsData->_stringSettings["STCOLORTERTIARY"].empty())
        this->_streetColorTertiary = GetColor(_userSettingsData->_stringSettings["STCOLORTERTIARY"]);
    else
        this->_streetColorTertiary = STLNWHITE;

    if(!_userSettingsData->_stringSettings["STCOLORRAIL"].empty())
        this->_streetColorRail = GetColor(_userSettingsData->_stringSettings["STCOLORRAIL"]);
    else
        this->_streetColorRail = STLNWHITE;

    if(!_userSettingsData->_stringSettings["STCOLORTRUNK"].empty())
        this->_streetColorTrunk = GetColor(_userSettingsData->_stringSettings["STCOLORTRUNK"]);
    else
        this->_streetColorTrunk = STLNWHITE;

    if(!_userSettingsData->_stringSettings["STCOLORPRIMARY"].empty())
        this->_streetColorPrimary = GetColor(_userSettingsData->_stringSettings["STCOLORPRIMARY"]);
    else
        this->_streetColorPrimary = STLNWHITE;

    if(!_userSettingsData->_stringSettings["STCOLORMOTORWAY"].empty())
        this->_streetColorMotorway = GetColor(_userSettingsData->_stringSettings["STCOLORMOTORWAY"]);
    else
        this->_streetColorMotorway = STLNWHITE;
}


///add elevations to all points in kml::vectors points
void StreetLineManager::AssignElevationsAndXYZ(boost::shared_ptr<Kml::Vectors> vecs)
{
    vector<boost::shared_ptr<Kml::Point> > pts = vecs->GetStreetVectors();
    int ptsSize = pts.size();
    boost::shared_ptr<Kml::Point> p1 = pts[0];
    boost::shared_ptr<Kml::Point> p2 = pts[ptsSize - 1];

    for(int i = 0 ; i < ptsSize ; i++)
    {
        boost::shared_ptr<Kml::Point> p1 = pts[i];
        double lat = p1->GetLat();
        double lon = p1->GetLon();
        double elev = _EML->GetElevation(lat, lon);
        p1->SetAlt(elev);
        double x, y, z = 0.00;
        _coordConverter->GeodeticToCartesian(lat, lon, elev, x, y, z);
        p1->SetX(x);
        p1->SetY(y);
        p1->SetZ(z);
    }
}
int StreetLineManager::SetEML(boost::shared_ptr<ElevationManagerLocal> eml) {
    _EML = eml;
    return 1;
}
void StreetLineManager::GetTexelColorLocation(STLINECOLOR color, float& texelX, float& texelY)
{
    //get location of color in the colorgradiant.png/texture
    texelX = 0.5;
    texelY = 1.0 - 0.5;
    switch(color)
    {
    case STLNBLUE://
        //blue
       texelX = 0.81;
       texelY = 1.0 - 0.5;
        break;
    case STLNGREEN://
        //green
       texelX = 0.62;
       texelY = 1.0 - 0.5;
        break;
    case STLNMAGENTA://
        //magenta
       texelX = 0.70;
       texelY = 1.0 - 0.5;
        break;
    case STLNORANGE://
        //orange
       texelX = 0.22;
       texelY = 1.0 - 0.5;
        break;
    case STLNPURPLE://
        //purple
        texelX = 0.99;
        texelY = 1.0 - 0.5;
        break;
    case STLNRED://
        texelX = 0.02;
       texelY = 1.0 - 0.44;
        break;
    case STLNWHITE://
        //white
        texelX = 0.5;
        texelY = 1.0 - 0.95;
        break;
    case STLNYELLOW://
        //yellow
       texelX = 0.38;
       texelY = 1.0 - 0.5;
        break;
    }
    if(_collar)
    {
        //xelX = 0.5;
              // texelY = 0.001;
        texelX = 0.5;
        texelY = 1.0 - 0.001;
    }
    //blue
   //texelX = 0.02;
   //texelY = 0.44;
}

void StreetLineManager::addStreetLine(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry)
{
   // bool collared = false;
    std::string name;
    std::string classification;

    using kmldom::KmlFactory;
    //place a symbol at each location
    geovectileparser parser(_userSettingsData, _statusLogger);
    int extent = layer.extent();

    KmlFactory* factory = KmlFactory::GetFactory();
    kmldom::StylePtr ptrStyle = factory->CreateStyle();

    kmldom::IconStylePtr iconStyle = factory->CreateIconStyle();
    kmldom::IconPtr ptrIcon = factory->CreateIcon();
    kmldom::IconStyleIconPtr ptrIconStyleIcon = factory->CreateIconStyleIcon();
    classification = findStringForKey("class", layer, feat);

    name = findStringForKey("name", layer, feat);
    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecStreetLines(new std::vector<boost::shared_ptr<Kml::Vectors> >);
    bool outsideTileN = false;//does this line go outside the tile boundary
    bool outsideTileS = false;//does this line go outside the tile boundary
    bool outsideTileE = false;//does this line go outside the tile boundary
    bool outsideTileW = false;//does this line go outside the tile boundary
    parser.parseLineString(vecGeometry, _x, _y, _zoomLevel, extent, vecStreetLines, outsideTileN, outsideTileS, outsideTileE, outsideTileW);


    float texelX = 0.5;
    float texelY = 0.5;

    boost::shared_ptr<std::vector<unsigned int> > currentIndicesList;
    if(classification.compare("minor") == 0)
    {
        GetTexelColorLocation(_streetColorMinor, texelX, texelY);
        currentIndicesList = _vecStreetLinesMinor;
    }
    else if(classification.compare("service") == 0)
    {
        GetTexelColorLocation(_streetColorService, texelX, texelY);
        currentIndicesList = _vecStreetLinesService;
    }
    else if(classification.compare("secondary") == 0)
    {
        GetTexelColorLocation(_streetColorSecondary, texelX, texelY);
        currentIndicesList = _vecStreetLinesSecondary;
    }
    else if(classification.compare("tertiary") == 0)
    {
        GetTexelColorLocation(_streetColorTertiary, texelX, texelY);
        currentIndicesList = _vecStreetLinesTertiary;
    }
    else if(classification.compare("rail") == 0)
    {
        GetTexelColorLocation(_streetColorRail, texelX, texelY);
        currentIndicesList = _vecStreetLinesRail;
    }
    else if(classification.compare("trunk") == 0)
    {
        GetTexelColorLocation(_streetColorTrunk, texelX, texelY);
        currentIndicesList = _vecStreetLinesTrunk;
    }
    else if(classification.compare("primary") == 0)
    {
        GetTexelColorLocation(_streetColorPrimary, texelX, texelY);
        currentIndicesList = _vecStreetLinesPrimary;
    }
    else
    {
       // collared = true;
        GetTexelColorLocation(_streetColorMotorway, texelX, texelY);
        currentIndicesList = _vecStreetLinesMotorway;
    }
    unsigned int indices = _streetLinesVertices->size() / 5;//this is pointing to the current location
    int counter = 0;
    for(int i = 0 ; i < vecStreetLines->size() ; i++)//I think we rarely have this a size > 1 if ever
    {

        boost::shared_ptr<Kml::Vectors> streetLines = (*(vecStreetLines.get()))[i];
        AssignElevationsAndXYZ(streetLines);
        std::vector<boost::shared_ptr<Kml::Point> > points = streetLines->GetStreetVectors();

        TriangulationLineString triLineString;
        TriangulationLineString triLineStringCollar;
        TriangulationPoint lastPoint(0.0, 0.0, 0.0);
        for(int j = 0 ; j < points.size() ; j++)//TODO: GET RID OF THE -1
        {
            counter = j;
            boost::shared_ptr<Kml::Point> pointSingle = points[j];
            double x = pointSingle->GetX();
            double y = pointSingle->GetY();
            double z = pointSingle->GetZ();
            double elev = 0.00;

            if(j == 3)
            {
                //x += 100.0;
            }
            //_streetLinesVertices->push_back((float)x);
            //_streetLinesVertices->push_back((float)y);
            //_streetLinesVertices->push_back((float)z);
            //_streetLinesVertices->push_back((float)texelX);
            //_streetLinesVertices->push_back((float)texelY);

            //create a triangulationsegment and add it to linestring linestring._linesegments.pushback(seg1)

            //TriangulationPoint lastPoint(x, y);
            TriangulationPoint currentPoint(x, y, z);
            if(j > 0 )
            {
                std::shared_ptr<TriangulationLineSegment> lineseg(new TriangulationLineSegment(lastPoint, currentPoint));
                lineseg->AddOffsets(_streetWidth);//setup the left and right offsets to the centerline, fyi they are not connected back to back, just offset by 90
                triLineString._lineSegments.push_back(lineseg);

                //if creating a collar, create another segment but wider offset
                //std::shared_ptr<TriangulationLineSegment> linesegcollar(new TriangulationLineSegment(lastPoint, currentPoint));
                //linesegcollar->AddOffsets(5.68);//setup the left and right offsets to the centerline, fyi they are not connected back to back, just offset by 90
                //triLineStringCollar._lineSegments.push_back(linesegcollar);
            }
            lastPoint = currentPoint;//for next iteration
            //we're going to add 2 points at a time since we will be drawing GL_LINES
            //then we don't have to worry about breaking lines at end of 1 street to begin of another
            if(j < points.size() - 1)//TODO: CHANGE BACK TO 1, BECAUSE WERE DOING GL_LINES WE NEED DUPLICATES OF EACH POINT
            {
                currentIndicesList->push_back(indices);
                indices++;
                currentIndicesList->push_back(indices);
            }
        }
        int index = _streetLinesVerticesTriang->size() / 5;
        //now get the l/r offset lines from the trilinestring and add to buffer, use indices to point to duplicate vertices.
        double lastleftx = 0.00;
        double lastlefty = 0.00;
        double lastleftz = 0.00;

        double lastrightx = 0.00;
        double lastrighty = 0.00;
        double lastrightz = 0.00;

        //create street lines as triangles, make sure stencil buffer turned on before rendering
        // to make a collar, basically use trilinestringcollar code from above and repeat this for loop
        for(int i = 0 ; i < triLineString._lineSegments.size()  ; i++)
        {

            std::shared_ptr<TriangulationLineSegment> currentSegment = triLineString._lineSegments[i];

            //fill in triangles from last line segment so there are no empty wedges where there is an angle between linesegments
            if(i != 0)
            {
                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointLeftOffset->_x));
                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointLeftOffset->_y));
                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointLeftOffset->_z));
                _streetLinesVerticesTriang->push_back(texelX);
                _streetLinesVerticesTriang->push_back(texelY);

                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_x));
                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_y));
                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_z));
                _streetLinesVerticesTriang->push_back(texelX);
                _streetLinesVerticesTriang->push_back(texelY);

                _streetLinesVerticesTriang->push_back((float)(lastleftx));
                _streetLinesVerticesTriang->push_back((float)(lastlefty));
                _streetLinesVerticesTriang->push_back((float)(lastleftz));
                _streetLinesVerticesTriang->push_back(texelX);
                _streetLinesVerticesTriang->push_back(texelY);

                _streetLinesIndicesTriang->push_back(index);
                index++;
                _streetLinesIndicesTriang->push_back(index);
                index++;
                _streetLinesIndicesTriang->push_back(index);
                index++;

                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointLeftOffset->_x));
                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointLeftOffset->_y));
                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointLeftOffset->_z));
                _streetLinesVerticesTriang->push_back(texelX);
                _streetLinesVerticesTriang->push_back(texelY);

                _streetLinesVerticesTriang->push_back((float)(lastrightx));
                _streetLinesVerticesTriang->push_back((float)(lastrighty));
                _streetLinesVerticesTriang->push_back((float)(lastrightz));
                _streetLinesVerticesTriang->push_back(texelX);
                _streetLinesVerticesTriang->push_back(texelY);

                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_x));
                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_y));
                _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_z));
                _streetLinesVerticesTriang->push_back(texelX);
                _streetLinesVerticesTriang->push_back(texelY);

                _streetLinesIndicesTriang->push_back(index);
                index++;
                _streetLinesIndicesTriang->push_back(index);
                index++;
                _streetLinesIndicesTriang->push_back(index);
                index++;
            }

            //triangle 1
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointLeftOffset->_x));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointLeftOffset->_y));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointLeftOffset->_z));
            _streetLinesVerticesTriang->push_back(texelX);
            _streetLinesVerticesTriang->push_back(texelY);

            _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_x));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_y));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_z));
            _streetLinesVerticesTriang->push_back(texelX);
            _streetLinesVerticesTriang->push_back(texelY);

            _streetLinesVerticesTriang->push_back((float)(currentSegment->_endPointLeftOffset->_x));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_endPointLeftOffset->_y));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_endPointLeftOffset->_z));
            _streetLinesVerticesTriang->push_back(texelX);
            _streetLinesVerticesTriang->push_back(texelY);

            _streetLinesIndicesTriang->push_back(index);
            index++;
            _streetLinesIndicesTriang->push_back(index);
            index++;
            _streetLinesIndicesTriang->push_back(index);
            index++;

            _streetLinesVerticesTriang->push_back((float)(currentSegment->_endPointLeftOffset->_x));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_endPointLeftOffset->_y));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_endPointLeftOffset->_z));
            _streetLinesVerticesTriang->push_back(texelX);
            _streetLinesVerticesTriang->push_back(texelY);

            _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_x));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_y));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_startPointRightOffset->_z));
            _streetLinesVerticesTriang->push_back(texelX);
            _streetLinesVerticesTriang->push_back(texelY);

            _streetLinesVerticesTriang->push_back((float)(currentSegment->_endPointRightOffset->_x));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_endPointRightOffset->_y));
            _streetLinesVerticesTriang->push_back((float)(currentSegment->_endPointRightOffset->_z));
            _streetLinesVerticesTriang->push_back(texelX);
            _streetLinesVerticesTriang->push_back(texelY);

            _streetLinesIndicesTriang->push_back(index);
            index++;
            _streetLinesIndicesTriang->push_back(index);
            index++;
            _streetLinesIndicesTriang->push_back(index);
            index++;

            lastrightx = (float)(currentSegment->_endPointRightOffset->_x);
            lastrighty = (float)(currentSegment->_endPointRightOffset->_y);
            lastrightz = (float)(currentSegment->_endPointRightOffset->_z);

            lastleftx = (float)(currentSegment->_endPointLeftOffset->_x);
            lastlefty = (float)(currentSegment->_endPointLeftOffset->_y);
            lastleftz = (float)(currentSegment->_endPointLeftOffset->_z);
        }
    }
    //take linestring and call connectoffsetlines, add addoffsets to connectoffsets function for each segment, then connect the offset lines
}

//#include "GLES3/gl3.h"
//#if defined(RESPONDAR)
//extern GLuint gbl_u_SamplerLocation ;
//bool VectorTile::RenderPerspectiveEntities(GLuint program, boost::shared_ptr<SViewData> viewData, RIMatrix4x4 projection, double pixelSize)
//#else
bool StreetLineManager::Render(GLuint program, boost::shared_ptr<SViewData> viewData,
                                           RIMatrix4x4 projection, double pixelSize)
//#endif

{
    // we will now want to set the texture created from the openglsetup routine as current and draw like we did rasters.
    // we'll need setup the grid of elevation data as before.

//#if defined(RESPONDAR)
//#else
    glBindBuffer(GL_ARRAY_BUFFER, _arrayBufStreetLinesTriang);
    //_arrayBufStreetLinesTriang.bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufStreetLinesTriang);
    //_indexBufStreetLinesTriang.bind();

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_ONE, GL_ONE);
    //_arrayBufStreetLines.bind();
    //_indexBufStreetLines.bind();

    GLuint colorTexture = findTextureBasedOnHref("colorgradient.png");
    glActiveTexture ( GL_TEXTURE0 );
    if(colorTexture)
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        //colorTexture->bind();
//    QVector4D c;
//        c.setX(1.0);
//        c.setY(1.0);
//        c.setZ(1.0);
//        c.setW(0.75);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //program->setUniformValue("lineColor", c);

    int offset = 0;
    int vertexLocation = glGetAttribLocation(program, "a_position");
    glVertexAttribPointer (vertexLocation, 3, GL_FLOAT, GL_FALSE, 5* sizeof(float), NULL);
    glEnableVertexAttribArray(vertexLocation);
//
//    int vertexLocation = program->attributeLocation("a_position");
//    program->enableAttributeArray(vertexLocation);
//    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, 5 * sizeof(float));

    // Offset for texture coordinate
    offset += 3 * sizeof(float);

    int texcoordLocation = glGetAttribLocation(program, "a_texCoord");
    glVertexAttribPointer (texcoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(texcoordLocation);
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
//    int texcoordLocation = program->attributeLocation("a_texcoord");
//    program->enableAttributeArray(texcoordLocation);
//    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, 5 * sizeof(float));

    glLineWidth(1.0);
//    glDrawElements(
//                   GL_LINES,      // mode
//                   (GLuint)_streetLinesIndices->size(),    // count
//                   GL_UNSIGNED_INT,   // type
//                   (void*)0           // element array buffer offset
//                   );

    glBindBuffer(GL_ARRAY_BUFFER, _arrayBufStreetLinesTriang);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufStreetLinesTriang);
    offset = 0;
     vertexLocation = glGetAttribLocation(program, "a_position");
    glVertexAttribPointer (vertexLocation, 3, GL_FLOAT, GL_FALSE, 5* sizeof(float), NULL);
    glEnableVertexAttribArray(vertexLocation);
    
//      vertexLocation = program->attributeLocation("a_position");
//    program->enableAttributeArray(vertexLocation);
//    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, 5 * sizeof(float));

     //Offset for texture coordinate
    offset += 3 * sizeof(float);

     //Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
     texcoordLocation = glGetAttribLocation(program, "a_texcoord");
    glVertexAttribPointer (texcoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(texcoordLocation);
    
//      texcoordLocation = program->attributeLocation("a_texcoord");
//    program->enableAttributeArray(texcoordLocation);
//    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, 5 * sizeof(float));
    if(_streetLinesIndicesTriang->size() > 0)//todo: remove
    {
            glDrawElements(
                   GL_TRIANGLES,      // mode
                   (GLuint)_streetLinesIndicesTriang->size(),    // count
                //3,
                   GL_UNSIGNED_INT,   // type
                   (void*)0           // element array buffer offset
                   );
    }
    return true;
//#endif
}

//TODO: COULD THIS GO IN UTILITIES???
#if defined(RESPONDAR)
GLuint StreetLineManager::findTextureBasedOnHref(std::string href)
#else
std::shared_ptr<QOpenGLTexture> StreetLineManager::findTextureBasedOnHref(std::string href)
#endif
{
    //std::vector<std::string> missingIcons;
    //todo: in here just look in the _texture map<string, qopengltexture> use href as the lookup
#if defined(RESPONDAR)
    GLuint textureToBind;// = 0x0;
#else
    std::shared_ptr<QOpenGLTexture> textureToBind;// = 0x0;
    textureToBind.reset();
#endif

    //textureToBind = _symbolTextures["pinred.png"];

    //todo: use _symbolTextures and lookup name from href
    //symbolTextures[href] is the texture we want
    //check if it exists first if not, use default of _pinretexturera
    if ( _symbolTextures->find(href) == _symbolTextures->end() )
    {
        boost::mutex::scoped_lock lock(missingIconMutexSL);

        // not found desired symbol
        bool foundRepeat = false;
        for(int jj = 0 ; jj < _missingIconsSL.size() ; jj++)
        {
            if(_missingIconsSL[jj].compare(href) == 0)
            {
                foundRepeat = true;
            }
        }
        if(foundRepeat == false)
        {

            //show message once
            std::stringstream ss;
            ss<<"could not find icon: ";
            ss<<AppDataDir();
            ss<<"/icons/";
            ss<<href;
            _missingIconsSL.push_back(href);
            WriteToStatusLog(ss.str(), true, true);
        }


        if(_symbolTextures->find("pinred.png") == _symbolTextures->end() ) {
            //didn't find default symbol even
            bool foundRepeat2 = false;
            for(int jj = 0 ; jj < _missingIconsSL.size() ; jj++)
            {
                if(_missingIconsSL[jj].compare("pinred.png") == 0)
                {
                    foundRepeat2 = true;
                }
            }
            if(foundRepeat2 == false)
            {
                std::stringstream ss;
                ss<<"could not find icon: ";
                ss<<AppDataDir();
                ss<<"/icons/";
                ss<<"pinred.png";
                _missingIconsSL.push_back("pinred.png");
                WriteToStatusLog(ss.str(), true, true);
            }
        }
        else{
            textureToBind = (*(_symbolTextures.get()))["pinred.png"];
        }

    }
    else
    {
        // found the icon!!
        textureToBind = (*(_symbolTextures.get()))[href];
    }

    return textureToBind;
}
//#include "qopenglframebufferobject.h"
//#include "GL/glxext.h"
//#include "GLES3/gl3.h"
bool StreetLineManager::SetupOpenglForEntities(GLuint program)
{
#if defined(RESPONDAR)
        //were ready to render, allocate  the street label buffers, vertices are already loaded
        //just need to make the opengl calls in the render loop
        //setup vertex buffer for street labels
   //     float* ptrVertices = &(*(_streetLabelVertices.get()))[0];
        //cout<<"ptrvert="<<ptrVertices<<endl;
SortStreetLineTypes();//add all street lines to one list for now until we have filtering going

    //float* ptrVerticesStreetLines = &(*(_streetLinesVertices.get()))[0];
    //_arrayBufStreetLines.create();
    //_arrayBufStreetLines.bind();
    //_arrayBufStreetLines.allocate(ptrVerticesStreetLines, _streetLinesVertices->size() *  sizeof(float));
    //_indexBufStreetLines.create();
    //_indexBufStreetLines.bind();
    //unsigned int* ptrIndicesStreetLines = &(*(_streetLinesIndices.get()))[0];
    //_indexBufStreetLines.allocate(ptrIndicesStreetLines, _streetLinesIndices->size() * sizeof(unsigned int));

    //set up buffers that contains street line triangulation data
    float* ptrVerticesStreetLinesTriang = &(*(_streetLinesVerticesTriang.get()))[0];
    glGenBuffers(1, &_arrayBufStreetLinesTriang);
    glBindBuffer(GL_ARRAY_BUFFER, _arrayBufStreetLinesTriang);
    glBufferData(GL_ARRAY_BUFFER, _streetLinesVerticesTriang->size() *  sizeof(float), ptrVerticesStreetLinesTriang, GL_STATIC_DRAW);
    //_arrayBufStreetLinesTriang.create();
    //_arrayBufStreetLinesTriang.bind();
    //_arrayBufStreetLinesTriang.allocate(ptrVerticesStreetLinesTriang, _streetLinesVerticesTriang->size() *  sizeof(float));
    
    unsigned int* ptrIndicesStreetLinesTriang = &(*(_streetLinesIndicesTriang.get()))[0];
    glGenBuffers(1, &_indexBufStreetLinesTriang);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufStreetLinesTriang);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _streetLinesIndicesTriang->size() * sizeof(unsigned int), ptrIndicesStreetLinesTriang, GL_STATIC_DRAW);
    //_indexBufStreetLinesTriang.create();
    //_indexBufStreetLinesTriang.bind();
    
    //_indexBufStreetLinesTriang.allocate(ptrIndicesStreetLinesTriang, _streetLinesIndicesTriang->size() * sizeof(unsigned int));


//        glGenBuffers(1, &_arrayBufStreetLabels);
 //       glBindBuffer(GL_ARRAY_BUFFER, _arrayBufStreetLabels);
 //       glBufferData(GL_ARRAY_BUFFER, _streetLabelVertices->size() * sizeof(float), &(*_streetLabelVertices.get())[0], GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, testVecVerts->size() * sizeof(float), &(*testVecVerts.get())[0], GL_STATIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);


       // unsigned int* ptrIndices = &(*(_streetLabelIndices.get()))[0];
       // glGenBuffers(1, &_indexBufStreetLabels);
       // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufStreetLabels);

       // glBufferData(GL_ELEMENT_ARRAY_BUFFER, _streetLabelIndices->size() * sizeof(unsigned int), ptrIndices, GL_STATIC_DRAW);

#else
    //GLuint fbo;
//    int width = 512;
//    int height = 512;
//    QOpenGLFramebufferObjectFormat format;
//    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
//    format.setSamples(4);
//    QOpenGLFramebufferObject* fbo = new QOpenGLFramebufferObject(width, height, format);
//    fbo->
//    GLuint color_texture;
//    GLuint depth_texture;

//    glGenFramebuffers(1, &fbo);
//    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//    glGenTextures(1, &color_texture);
//    glBindTexture(GL_TEXTURE_2D, color_texture);
//    glTexStorage2D(GL_TEXTURE_2D, 9, GL_RGBA8, 512, 512);

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//    glGenTextures(1, &depth_texture);
//    glBindTexture(GL_TEXTURE_2D, depth_texture);
//    glTexStorage2D(GL_TEXTURE_2D, 9, GL_DEPTH_COMPONENT32F, 512, 512);

    //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

    //glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    //static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};
    //glDrawBuffers(1, draw_buffers);



    SortStreetLineTypes();//add all street lines to one list for now until we have filtering going

    //set up buffer that contains street line data
    float* ptrVerticesStreetLines = &(*(_streetLinesVertices.get()))[0];
    _arrayBufStreetLines.create();
    _arrayBufStreetLines.bind();
    _arrayBufStreetLines.allocate(ptrVerticesStreetLines, _streetLinesVertices->size() *  sizeof(float));
    _indexBufStreetLines.create();
    _indexBufStreetLines.bind();
    unsigned int* ptrIndicesStreetLines = &(*(_streetLinesIndices.get()))[0];
    _indexBufStreetLines.allocate(ptrIndicesStreetLines, _streetLinesIndices->size() * sizeof(unsigned int));

    //set up buffers that contains street line triangulation data
    float* ptrVerticesStreetLinesTriang = &(*(_streetLinesVerticesTriang.get()))[0];
    _arrayBufStreetLinesTriang.create();
    _arrayBufStreetLinesTriang.bind();
    _arrayBufStreetLinesTriang.allocate(ptrVerticesStreetLinesTriang, _streetLinesVerticesTriang->size() *  sizeof(float));
    _indexBufStreetLinesTriang.create();
    _indexBufStreetLinesTriang.bind();
    unsigned int* ptrIndicesStreetLinesTriang = &(*(_streetLinesIndicesTriang.get()))[0];
    _indexBufStreetLinesTriang.allocate(ptrIndicesStreetLinesTriang, _streetLinesIndicesTriang->size() * sizeof(unsigned int));


    //were ready to render, allocate  the street label buffers, vertices are already loaded
    //just need to make the opengl calls in the render loop
    //setup vertex buffer for street labels
    //float* ptrVertices = &(*(_streetLabelVertices.get()))[0];
    //cout<<"ptrvert="<<ptrVertices<<endl;

#endif
        //todo: add all the elements from all the vectors of street lines to 1 vector
        //todo: add the color for each street type by setting the texel location to the colorgradient.png
        //  that way we only have to call 1 gldraw command.
        //todo: create street line buffer, bind allocate
        //todo: create a indexbuffer for street lines/allocate
    //std::cout<<"size of minor roads="<<_vecStreetLinesMinor->size()<<std::endl;
    return true;
}
void StreetLineManager::SortStreetLineTypes()
{
    //todo: for now just copy all the indices from each type to the final render indices buffer
    for(unsigned int i = 0 ; i < _vecStreetLinesMinor->size() ; i++)
    {
        _streetLinesIndices->push_back((*(_vecStreetLinesMinor.get()))[i]);
    }
    for(unsigned int i = 0 ; i < _vecStreetLinesService->size() ; i++)
    {
        _streetLinesIndices->push_back((*(_vecStreetLinesService.get()))[i]);
    }
    for(unsigned int i = 0 ; i < _vecStreetLinesSecondary->size() ; i++)
    {
        _streetLinesIndices->push_back((*(_vecStreetLinesSecondary.get()))[i]);
    }
    for(unsigned int i = 0 ; i < _vecStreetLinesTertiary->size() ; i++)
    {
        _streetLinesIndices->push_back((*(_vecStreetLinesTertiary.get()))[i]);
    }
    for(unsigned int i = 0 ; i < _vecStreetLinesRail->size() ; i++)
    {
        _streetLinesIndices->push_back((*(_vecStreetLinesRail.get()))[i]);
    }
    for(unsigned int i = 0 ; i < _vecStreetLinesTrunk->size() ; i++)
    {
        _streetLinesIndices->push_back((*(_vecStreetLinesTrunk.get()))[i]);
    }
    for(unsigned int i = 0 ; i < _vecStreetLinesPrimary->size() ; i++)
    {
        _streetLinesIndices->push_back((*(_vecStreetLinesPrimary.get()))[i]);
    }
    for(unsigned int i = 0 ; i < _vecStreetLinesMotorway->size() ; i++)
    {
        _streetLinesIndices->push_back((*(_vecStreetLinesMotorway.get()))[i]);
    }
}



