//#define CLOCK_REALTIME
#define AUTOSCALETEXT
#define UNIFORMBUFFSIZE 65536
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "vector_tile.pb.h"
#pragma GCC diagnostic pop

#if defined(__APPLE__)
#include "vector_tile_compression.hpp"
#include "geodatacachesettings.h"
#include "objcUtilityWrapper.h"
extern GLuint textureLocation;
#else
#include "vectortile/vector_tile_compression.hpp"
#include <geodatacachesettings.h>
#include </usr/include/png.h>
#include <QOpenGLTexture>
#endif

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <sstream>
#include <fstream>


#include "geovectileparser.h"

#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"

#include "vectortile.h"
#include "rafootprint.h"
#include "utilities.h"

#include <math.h>
#include "geotileservergl.h"

#define COORDCOUNT 16385 // four corners of tile
#include "LFClient.h"

#include <OpenGLES/ES3/gl.h>

#include <sys/stat.h>
extern boost::mutex glbPlacemarkMutex;
boost::mutex missingIconMutex;
std::vector<std::string> _missingIcons;

extern GLuint textureLocation;
void VectorTile::SetUserSettings()
{

    //boost::unique_lock<boost::mutex> scoped_lock(mutex);
     using namespace boost::filesystem;
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

    std::string clearCache(_userSettingsData->_stringSettings["CLEARCACHEFOLDER"]);
    if(clearCache.compare("TRUE") == 0)
    {
        _clearCacheFolder = true;
    }
    else
        _clearCacheFolder = false;

    std::string dataDump(_userSettingsData->_stringSettings["VECTORDATADUMP"]);
    if(dataDump.compare("TRUE") == 0)
    {
        _vectorDataDump = true;
    }
    else
        _vectorDataDump = false;


    std::string debugtn(_userSettingsData->_stringSettings["DEBUGTRANSNAME"]);
    if(debugtn.compare("TRUE") == 0)
    {
        _debugTransName = true;
    }
    else
        _debugTransName = false;

    if(!(_userSettingsData->_intSettings.find("STLABELCUTOFF1") == _userSettingsData->_intSettings.end())){
        int icutoff1 = _userSettingsData->_intSettings["STLABELCUTOFF1"];
        _streetLabelCutoff1 = static_cast<double>(icutoff1) / 1000.0;
    }
    else
        this->_streetLabelCutoff1 =11.0;

    //scale2
    if(!(_userSettingsData->_intSettings.find("STLABELSCALE2") == _userSettingsData->_intSettings.end())){
        int iscale = _userSettingsData->_intSettings["STLABELSCALE2"];
        _streetLabelScale2 = static_cast<double>(iscale) / 1000.0;
    }
    else
        this->_streetLabelScale1 =2.0;


    //cutoff2
    if(!(_userSettingsData->_intSettings.find("STLABELCUTOFF2") == _userSettingsData->_intSettings.end())){
        int icutoff2 = _userSettingsData->_intSettings["STLABELCUTOFF2"];
        _streetLabelCutoff2 = static_cast<double>(icutoff2) / 1000.0;
    }
    else
        this->_streetLabelCutoff2 = 3.7;

    //scale3
    if(!(_userSettingsData->_intSettings.find("STLABELSCALE3") == _userSettingsData->_intSettings.end())){
        int iscale = _userSettingsData->_intSettings["STLABELSCALE3"];
        _streetLabelScale3 = static_cast<double>(iscale) / 1000.0;
    }
    else
        this->_streetLabelScale3 =2.0;


    //cutoff 3
    if(!(_userSettingsData->_intSettings.find("STLABELCUTOFF3") == _userSettingsData->_intSettings.end())){
        int icutoff3 = _userSettingsData->_intSettings["STLABELCUTOFF3"];
        _streetLabelCutoff3 = static_cast<double>(icutoff3) / 1000.0;
    }
    else
        this->_streetLabelCutoff3 = 1.3;

    //scale4
    if(!(_userSettingsData->_intSettings.find("STLABELSCALE4") == _userSettingsData->_intSettings.end())){
        int iscale = _userSettingsData->_intSettings["STLABELSCALE4"];
        _streetLabelScale4 = static_cast<double>(iscale) / 1000.0;
    }
    else
        this->_streetLabelScale4 =1.0;


    //cutoff 4
    if(!(_userSettingsData->_intSettings.find("STLABELCUTOFF4") == _userSettingsData->_intSettings.end())){
        int icutoff = _userSettingsData->_intSettings["STLABELCUTOFF4"];
        _streetLabelCutoff4 = static_cast<double>(icutoff) / 1000.0;
    }
    else
        this->_streetLabelCutoff4 = 1.3;

    //scale5
    if(!(_userSettingsData->_intSettings.find("STLABELSCALE5") == _userSettingsData->_intSettings.end())){
        int iscale = _userSettingsData->_intSettings["STLABELSCALE5"];
        _streetLabelScale5 = static_cast<double>(iscale) / 1000.0;
    }
    else
        this->_streetLabelScale5 =0.75;
}
void VectorTile::AddSymbol()
{
    //lfclient adding a custom symbol from a kml file
}

bool compareSymbolDist(boost::shared_ptr<symbol> i1, boost::shared_ptr<symbol> i2)
{
    //go furthest to closest for symbol rendering order so no overlapping symbols
    return (i1->GetDistToVehicle() > i2->GetDistToVehicle());
}


void VectorTile::SortEntities(double vehLat, double vehLon){
    UpdateSortOrderNum();//++ the sortnumber and set, meaning that we won't try to update disttoveh of symbols till later, puts it down on the list

    //BBOARD: USE symbboardL1 2 4 8 16 symbols here too
    //update all symbol to veh distances
    int symbolCount = _symbols.size();
    for(int i = 0 ; i < symbolCount ; i++)
    {
        _symbols[i]->UpdateDistToVehicle(vehLat, vehLon);
    }
    sort(_symbols.begin(), _symbols.end(), compareSymbolDist);//sort in ascending sortordernum
}

VectorTile::~VectorTile()
{
#if !defined(__APPLE__)

    if(_arrayBufStreetLabels.isCreated())
        _arrayBufStreetLabels.destroy();
    if(_indexBufStreetLabels.isCreated())
        _indexBufStreetLabels.destroy();
    
    if(_uniformBuffer)
    {
        glDeleteBuffers(1, &_uniformBuffer);
    }
    free(_uniformBufferData);
#endif
}

#if defined(__APPLE__)
VectorTile::VectorTile(int x, int y, int zoomLevel, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
                       boost::shared_ptr<CoordinateConverter> coordConverter, boost::shared_ptr<std::map<std::string,
                       GLuint > > symbolTextures, boost::shared_ptr<placemarkCollection> pmCollection,
                       boost::shared_ptr<std::map<std::string, boost::shared_ptr<iconInfo> > > poiMap, boost::shared_ptr<std::map<std::string,
                       boost::shared_ptr<iconInfo> > > placeMap, boost::shared_ptr<std::vector<boost::shared_ptr<symbol> > > customSymbolList, double streetLineWidth, int autoScaleOffsetStVecs, int autoScaleOffsetIndices, int autoScaleOffsetSize, int autoScaleStVecStride, int autoScaleIndicesStride):
_readyToSetupOpengl(false),
_readyToRender(false),
tile(x, y, zoomLevel),
_localCustomSymbolList(new std::vector<boost::shared_ptr<symbol> > ),
_vectorDataDump(false),
_debugTransName(false),
_streetLabelVertices(new std::vector<float>),
_streetLabelIndices(new std::vector<unsigned int>),
_currentStreetCutoff(0.0),
_streetLineManager(x, y, zoomLevel, usettingsData, logger, coordConverter, symbolTextures, streetLineWidth, false),
_streetLineManagerCollar(x, y, zoomLevel, usettingsData, logger, coordConverter, symbolTextures, streetLineWidth + 2, true),
_autoScaleOffsetSize(autoScaleOffsetSize),
_autoScaleOffsetStVecs(autoScaleOffsetStVecs),
_autoScaleOffsetIndices(autoScaleOffsetIndices),
_autoScaleStVecStride(autoScaleStVecStride),
_autoScaleIndicesStride(autoScaleIndicesStride),
_currentUBIndicesIndices(0),
_currentUBStIndices(0),
_dumpStreetVecData(false),
_uniformBuffer(999),
_oldBindingPoint(-1)
#else
VectorTile::VectorTile(int x, int y, int zoomLevel, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
                       boost::shared_ptr<CoordinateConverter> coordConverter, boost::shared_ptr<std::map<std::string,
                       boost::shared_ptr<QOpenGLTexture> > > symbolTextures, boost::shared_ptr<placemarkCollection> pmCollection,
                       boost::shared_ptr<std::map<std::string, boost::shared_ptr<iconInfo> > > poiMap, boost::shared_ptr<std::map<std::string,
                       boost::shared_ptr<iconInfo> > > placeMap, boost::shared_ptr<std::vector<boost::shared_ptr<symbol> > > customSymbolList):
                       //int autoScaleOffsetStVecs, int autoScaleOffsetIndices, int autoScaleOffsetSize, int autoScaleStVecStride, int autoScaleIndicesStride):

    _readyToSetupOpengl(false),
    _readyToRender(false),
    tile(x, y, zoomLevel),
    _localCustomSymbolList(new std::vector<boost::shared_ptr<symbol> > ),
    _vectorDataDump(false),
    _debugTransName(false),
    _streetLabelVertices(new std::vector<float>),
    _streetLabelIndices(new std::vector<unsigned int>),
    _arrayBufStreetLabels(QOpenGLBuffer::VertexBuffer),
    _indexBufStreetLabels(QOpenGLBuffer::IndexBuffer),
    _currentStreetCutoff(0.0)
#endif
{
    _currentUBOffsetStVec = _autoScaleOffsetStVecs;//these will get incremented as we add data
    _currentUBOffsetIndices = _autoScaleOffsetIndices;//these will get incremented as we add data
    _uniformBufferData = (unsigned char*)malloc(UNIFORMBUFFSIZE);
    for(int i = 0 ; i < UNIFORMBUFFSIZE ; i++)
    {
        _uniformBufferData[i] = 0x00;
    }
    _glbCustomSymbolList = customSymbolList;
    _poiIconMap = poiMap;
        _placeIconMap = placeMap;
    LoggableConfigurableObject::Init(usettingsData, logger, "VectorTile");
    _placemarkCollection = pmCollection;
    _coordConverter = coordConverter;
    _symbolTextures = symbolTextures;
    //fillIconMap();
   // _x = x;
   // _y = y;
   // _zoomLevel = zoomLevel;
//    _boundingBox = tile2boundingBox(x, y, zoomLevel);
    //ADDKML: CALL THE ONNOTIFYCUSTOMSYMBOLS FUNCTION
    OnNotifyCustomSymbolChange();
    if(_dumpStreetVecData){
        std::stringstream ss;
        ss<<"vectorfile-"<<x<<"-"<<y<<".txt";
        std::string fname(ss.str());
        _vectorFile.open(fname);
    }
}
/**this function gets called from LFClient whenever there is a change in custom icons from a kml file
 for example, if the user wants to add a kmz file with custom symbols, this vector tile needs to know
 a new icon/symbol was added to the global list.  This function will automatically get called, and
 this vector tile needs to go through the global list, and add whichever symbols are inside this tile
 to it's render list.
 */
void VectorTile::OnNotifyCustomSymbolChange()
{

    _localCustomSymbolList->clear();
   // symbol desired x:2642 desiredy5853 zoomlevel14
     //                     symbol desired x:660 desiredy1463 zoomlevel12
                          //symbol desired x:1321 desiredy2926 zoomlevel13
    //symbol desired x:330 desiredy731 zoomlevel11
    
//
//    if(_x == 2642 && _y == 5853)
//        cout<<"stop"<<endl;//zl14
//    if(_x == 660 && _y == 1463)
//        cout<<"stop"<<endl;//zl12
//    if(_x == 1321 && _y == 2926)
//        cout<<"stop"<<endl;//zl13//here's the problem
//    if(_x == 330 && _y == 731)
//        cout<<"stop"<<endl;//zl11
    
    //todo: go through glbCustomsymbol list, look for any symbols that are inside this vectortile
    // and add that symbol to the _localCustomSymbolList.  That list is what gets rendered.
    // _glbCustomSymbolList;//global list
    //   boost::shared_ptr<std::vector<boost::shared_ptr<symbol> > > _localCustomSymbolList;
    int count = 0;
    //TODO: THIS NEEDS TO BE CHANGED, ONLY ADD THE SYMBOLS THAT ARE INSIDE THIS TILE!!
    //cout<<"creating "<<_glbCustomSymbolList->size()<<" custom icons"<<endl;
    for(int i = 0 ; i < _glbCustomSymbolList->size() ;i++)
    {
        boost::shared_ptr<symbol> sym = (*(_glbCustomSymbolList.get()))[i];
        double symLat = sym->GetLat();
        double symLon = sym->GetLon();
        bool insideTile = IsPointInside(symLat, symLon);
        int desiredx = long2tilex(symLon, _zoomLevel);
        int desiredy = lat2tiley(symLat, _zoomLevel);
        //std::stringstream ss3;
        //ss3<<"--------symbol desired x:"<<desiredx<<" desiredy"<<desiredy<<" zoomlevel"<<_zoomLevel;
        //WriteToStatusLog(ss3.str(), false, true);
        //if(insideTile == true)
        {
            
            _localCustomSymbolList->push_back(sym);
            //std::stringstream ss;
            //ss<<"*****MATCHsymLat:"<<symLat<<" symLon:"<<symLon<<" not inside x:"<<_x<<" y:"<<_y;
            //ss<<"not inside tile: "<<
            //WriteToStatusLog(ss.str(), false, true);
            //std::stringstream ss2;
            //ss2<<"*****MATCHboundingbox n"<<_boundingBox.north<<" s"<<_boundingBox.south<<" e"<<_boundingBox.east<<" w"<<_boundingBox.west;
            //WriteToStatusLog(ss2.str(), false, true);
        }
        //        else
        //        {
        //            std::stringstream ss;
        //            ss<<"symLat:"<<symLat<<" symLon:"<<symLon<<" not inside x:"<<_x<<" y:"<<_y;
        //            //ss<<"not inside tile: "<<
        //            WriteToStatusLog(ss.str(), false, true);
        //            std::stringstream ss2;
        //            ss2<<"boundingbox n"<<_boundingBox.north<<" s"<<_boundingBox.south<<" e"<<_boundingBox.east<<" w"<<_boundingBox.west;
        //            WriteToStatusLog(ss2.str(), false, true);
        //        }
        // else
        //   cout<<"not insdie"<<endl;
        //if symbol is indside this bounding box then add it
        //_boundingBox.
        //BoundingBox _boundingBox;
        
    }
    //WriteToStatusLog("vec tile notified of custom symbol changes", false, true);
}
#if defined(__APPLE__)
bool VectorTile::RenderOrthographicEntities(GLuint program, bool limitMet, boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight, RIMatrix4x4 projection, std::vector<double>& alreadyDrawnIcons, bool billboardStyle, bool streetShieldsOn, int labelDensity){
#else
bool VectorTile::RenderOrthographicEntities(QOpenGLShaderProgram* program, bool limitMet, boost::shared_ptr<SViewData> viewData,
                                            double windowWidth, double windowHeight, QMatrix4x4 projection){
#endif
    _viewData = viewData;
    //todo: setup opengl entities for custom symbols here.  Check the customsymbolist size
    //as a trigger for when to do this.  if(checkedsize != customsymbollist.size)
    //              then checkedsize = customsymbollist.size
    //              go through list of customsymbols, check which ones belong to this tile
    //              and create the opengl entities for that symbol
    
    //render anything that doesn't get smaller as it's further away ie symbols
    //orthographic projection simplifys our life with symbols, they will be shown
    //square to the screen with no worries about warping of images as they are
    //displayed further from the center of the screen.
    
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
#if !defined(__APPLE__)

    glEnable(GL_MULTISAMPLE);

    bool createdGLEntities = false;
    if(!_readyToRender)
        return false;
//    if(!_readyToSetupOpengl && !_readyToRender){
//        return createdGLEntities; //not loaded and not ready yet, don't render
//    }
//    else if(limitMet && _readyToSetupOpengl && !_readyToRender){
//        //don't attempt to setup opengl stuff cause we're clogging up the pipeline,
//        //just return, and eventually we'll get to this
//        return createdGLEntities;
//    }
//    else if(_readyToSetupOpengl && !_readyToRender ){
//        bool openglSetupComplete = SetupOpenglForEntities();
//        if(openglSetupComplete)
//        {
//            createdGLEntities = true;//let lfclient know that we created some entities so it
//            //can keep track and limit the amount of them done so we don't bog down the cpu
//            _readyToRender = true;
//        }
//        return createdGLEntities;
//    }
#endif
    double xveh = 0.00;
    double yveh = 0.00;
    double zveh = 0.00;
    _coordConverter->GeodeticToCartesian(viewData->dVehicleLat, viewData->dVehicleLon, viewData->dVehicleAltitude, xveh, yveh, zveh);
    for(int i = 0 ; i < _symbols.size() ; i++)
    {
        boost::shared_ptr<symbol> sym = _symbols[i];
        if(sym)
        {
            POIENUM symtype = sym->GetIconType();
            boost::mutex::scoped_lock lock(glbPlacemarkMutex);
            if(_placemarkCollection->isVisible(symtype))
            {//if this symbol is a street shield, then check the flag to display or not based on gui switch
                if(sym->GetIconType() == SHIELD_HIGHWAY || sym->GetIconType() == SHIELD_WHITERECT || sym->GetIconType() == SHIELD_INTERSTATE){
                    if(streetShieldsOn){
                        sym->render(viewData, windowWidth, windowHeight, program, xveh, yveh, zveh, projection, alreadyDrawnIcons, billboardStyle, labelDensity);
                    }
                }
                else{
                    //BBOARD: find which level we are on and render the billboarded street labels. ie symbboardL1
                    //          symbboardL2 etc
                    sym->render(viewData, windowWidth, windowHeight, program, xveh, yveh, zveh, projection, alreadyDrawnIcons, billboardStyle, labelDensity);
                }
            }
        }
    }
    for(int i = 0 ; i < _glbCustomSymbolList->size(); i++ )
    {
        //WriteToStatusLog("found some custom symbols", false, true);
        boost::shared_ptr<symbol> sym = (*(_glbCustomSymbolList.get()))[i];
        POIENUM symtype = sym->GetIconType();
        boost::mutex::scoped_lock lock(glbPlacemarkMutex);
        if(_placemarkCollection->isVisible(symtype))
        {
            sym->render(viewData, windowWidth, windowHeight, program, xveh, yveh, zveh, projection, alreadyDrawnIcons, billboardStyle, labelDensity);
        }
    }
    //    for(int i = 0 ; i < _localCustomSymbolList->size(); i++ )
    //    {
    //        //WriteToStatusLog("found some custom symbols", false, true);
    //        boost::shared_ptr<symbol> sym = (*(_localCustomSymbolList.get()))[i];
    //        POIENUM symtype = sym->GetIconType();
    //        if(_placemarkCollection->isVisible(symtype))
    //        {
    //            sym->render(viewData, windowWidth, windowHeight, program, xveh, yveh, zveh, projection);
    //        }
    //    }
    return true;
}
std::string VectorTile::findStringForKey(std::string keyString, vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat)
{
    int indexForKey =-1;
    std::string returnString;
    
    for (std::size_t k=0;k<static_cast<std::size_t>(layer.keys_size());++k)
    {
        std::string const& key = layer.keys(k);
        if(key.compare(keyString) == 0)
        {
            //this is the index we want to look into values field for the name
            indexForKey = k;
        }
        
    }
    
    for (std::size_t m=0; m < static_cast<std::size_t>(feat.tags_size()) ; m += 2)
    {
        uint32_t tagkey = feat.tags(m);
        uint32_t tagvalue = feat.tags(m + 1);
        if(tagkey == indexForKey)
        {
            vector_tile::Tile_Value val = layer.values(tagvalue);
            if(val.has_string_value())
            {
                returnString = val.string_value();
            }
            if(val.has_int_value())
            {
                std::stringstream ss;
                ss<<"intval:"<<val.int_value();
                returnString = ss.str();
            }
            if(val.has_bool_value())
            {
                std::stringstream ss;
                ss<<"boolval:"<<val.bool_value();
                returnString = ss.str();
            }
            if(val.has_double_value())
            {
                std::stringstream ss;
                ss<<"doubleval:"<<val.double_value();
                returnString = ss.str();
            }
            if(val.has_float_value())
            {
                std::stringstream ss;
                ss<<"floatval:"<<val.float_value();
                returnString = ss.str();
            }
            if(val.has_sint_value())
            {
                std::stringstream ss;
                ss<<"sintval:"<<val.sint_value();
                returnString = ss.str();
            }
            if(val.has_uint_value())
            {
                std::stringstream ss;
                ss<<"uint:"<<val.uint_value();
                returnString = ss.str();
            }
        }
    }
    return returnString;
}

boost::shared_ptr<iconInfo> VectorTile::addPoi(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry)
{
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
    
    boost::shared_ptr<iconInfo> iinfo = (*(_poiIconMap.get()))[classification];
    if(!iinfo)
        iinfo.reset(new iconInfo(PM_UNKNOWN, "pinred.png"));
    std::string iconhref  = iinfo->_iconFileName;
    ptrIconStyleIcon->set_href(iconhref);
    
    iconStyle->set_icon(ptrIconStyleIcon);
    ptrStyle->set_iconstyle(iconStyle);
    
    name = findStringForKey("name", layer, feat);
    _points = parser.parsePlace( vecGeometry, _x, _y, _zoomLevel, extent, ptrStyle, name);
    for(int i = 0 ; i < _points->size() ; i++)
    {
        boost::shared_ptr<Kml::Point> p = (*(_points.get()))[i];
        std::string str = p->GetIconHref();
    }
    return iinfo;
}

///add elevations to all points in kml::vectors points
void VectorTile::AssignElevations(boost::shared_ptr<Kml::Vectors> vecs)
{
    vector<boost::shared_ptr<Kml::Point> > pts = vecs->GetStreetVectors();
    int ptsSize = pts.size();
    boost::shared_ptr<Kml::Point> p1 = pts[0];
    boost::shared_ptr<Kml::Point> p2 = pts[ptsSize - 1];
    //TODO DO GEO TO CART ON EACH POINT WHILE WE'RE AT IT
    for(int i = 0 ; i < ptsSize ; i++)
    {
        boost::shared_ptr<Kml::Point> p1 = pts[i];
        double lat = p1->GetLat();
        double lon = p1->GetLon();
        double elev = _EML->GetElevation(lat, lon);
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        _coordConverter->GeodeticToCartesian(lat, lon, elev, x, y, z);
        p1->SetAlt(elev);
        p1->SetX(x);
        p1->SetY(y);
        p1->SetZ(z);
    }
}
void VectorTile::addTransNameBillboard(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry)
{
    std::string name;
        std::string classification;
        
        using kmldom::KmlFactory;
        //place a symbol at each location
        geovectileparser parser(_userSettingsData, _statusLogger);
        classification = findStringForKey("class", layer, feat);
        //cout<<"class="<<classification<<endl;
        name = findStringForKey("name", layer, feat);
        //cout<<"streetname = "<<name<<endl;
        int extent = layer.extent();
        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecStreets(new std::vector<boost::shared_ptr<Kml::Vectors> >);
        bool outsideTileN = false;
        bool outsideTileS = false;
        bool outsideTileE = false;
        bool outsideTileW = false;
        parser.parseLineString(vecGeometry, _x, _y, _zoomLevel, extent, vecStreets, outsideTileN, outsideTileS, outsideTileE, outsideTileW);
    boost::shared_ptr<Kml::Vectors> vecStreetsL1 = (*(vecStreets.get()))[0];
    boost::shared_ptr<symbol> sym1x = Assign1xBillboardSymbol(vecStreetsL1, name, SL_STREETLABELX1);//find midpoint of vector and create the symbol at 1x ie center of vectors
    
    //x2 billboards
    boost::shared_ptr<Kml::Vectors> vecStreetsL2;//(new std::vector<boost::shared_ptr<Kml::Vectors> >);
    boost::shared_ptr<Kml::Vectors> vecStreetsL3;//(new std::vector<boost::shared_ptr<Kml::Vectors> >);
    SplitStreetVector(vecStreetsL1, vecStreetsL2, vecStreetsL3, _coordConverter);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[2], vecStreetsL2);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[3], vecStreetsL3);
    
    //x4 billboards
    boost::shared_ptr<Kml::Vectors> vecStreetsL4;
    boost::shared_ptr<Kml::Vectors> vecStreetsL5;
    boost::shared_ptr<Kml::Vectors> vecStreetsL6;
    boost::shared_ptr<Kml::Vectors> vecStreetsL7;
    SplitStreetVector(vecStreetsL2, vecStreetsL4, vecStreetsL5, _coordConverter);
    SplitStreetVector(vecStreetsL3, vecStreetsL6, vecStreetsL7, _coordConverter);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[4], vecStreetsL4);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[5], vecStreetsL5);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[6], vecStreetsL6);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[7], vecStreetsL7);
    
    //x8 billboards
    boost::shared_ptr<Kml::Vectors> vecStreetsL8;
    boost::shared_ptr<Kml::Vectors> vecStreetsL9;
    boost::shared_ptr<Kml::Vectors> vecStreetsL10;
    boost::shared_ptr<Kml::Vectors> vecStreetsL11;
    boost::shared_ptr<Kml::Vectors> vecStreetsL12;
    boost::shared_ptr<Kml::Vectors> vecStreetsL13;
    boost::shared_ptr<Kml::Vectors> vecStreetsL14;
    boost::shared_ptr<Kml::Vectors> vecStreetsL15;
    SplitStreetVector(vecStreetsL4, vecStreetsL8, vecStreetsL9, _coordConverter);
    SplitStreetVector(vecStreetsL5, vecStreetsL10, vecStreetsL11, _coordConverter);
    SplitStreetVector(vecStreetsL6, vecStreetsL12, vecStreetsL13, _coordConverter);
    SplitStreetVector(vecStreetsL7, vecStreetsL14, vecStreetsL15, _coordConverter);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[8],vecStreetsL8);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[9],vecStreetsL9);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[10],vecStreetsL10);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[11],vecStreetsL11);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[12],vecStreetsL12);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[13],vecStreetsL13);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[14],vecStreetsL14);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[15],vecStreetsL15);
    
    //x16 billboards
    boost::shared_ptr<Kml::Vectors> vecStreetsL16;
    boost::shared_ptr<Kml::Vectors> vecStreetsL17;
    boost::shared_ptr<Kml::Vectors> vecStreetsL18;
    boost::shared_ptr<Kml::Vectors> vecStreetsL19;
    boost::shared_ptr<Kml::Vectors> vecStreetsL20;
    boost::shared_ptr<Kml::Vectors> vecStreetsL21;
    boost::shared_ptr<Kml::Vectors> vecStreetsL22;
    boost::shared_ptr<Kml::Vectors> vecStreetsL23;
    boost::shared_ptr<Kml::Vectors> vecStreetsL24;
    boost::shared_ptr<Kml::Vectors> vecStreetsL25;
    boost::shared_ptr<Kml::Vectors> vecStreetsL26;
    boost::shared_ptr<Kml::Vectors> vecStreetsL27;
    boost::shared_ptr<Kml::Vectors> vecStreetsL28;
    boost::shared_ptr<Kml::Vectors> vecStreetsL29;
    boost::shared_ptr<Kml::Vectors> vecStreetsL30;
    boost::shared_ptr<Kml::Vectors> vecStreetsL31;
    SplitStreetVector(vecStreetsL8, vecStreetsL16, vecStreetsL17, _coordConverter);
    SplitStreetVector(vecStreetsL9, vecStreetsL18, vecStreetsL19, _coordConverter);
    SplitStreetVector(vecStreetsL10, vecStreetsL20, vecStreetsL21, _coordConverter);
    SplitStreetVector(vecStreetsL11, vecStreetsL22, vecStreetsL23, _coordConverter);
    SplitStreetVector(vecStreetsL12, vecStreetsL24, vecStreetsL25, _coordConverter);
    SplitStreetVector(vecStreetsL13, vecStreetsL26, vecStreetsL27, _coordConverter);
    SplitStreetVector(vecStreetsL14, vecStreetsL28, vecStreetsL29, _coordConverter);
    SplitStreetVector(vecStreetsL15, vecStreetsL30, vecStreetsL31, _coordConverter);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[16], vecStreetsL16);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[17], vecStreetsL17);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[18], vecStreetsL18);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[19], vecStreetsL19);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[20], vecStreetsL20);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[21], vecStreetsL21);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[22], vecStreetsL22);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[23], vecStreetsL23);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[24], vecStreetsL24);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[25], vecStreetsL25);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[26], vecStreetsL26);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[27], vecStreetsL27);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[28], vecStreetsL28);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[29], vecStreetsL29);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[30], vecStreetsL30);
    AssignBillboardSymbolLevel(sym1x->_cartesianBillboardPositions[31], vecStreetsL31);
    
    //BBOARD: WE NEED TO TAKE VECSTREETS AND START SPLITTING IN HALVES SO WE CAN GET X1 X2 X4 8 16 LABELS

}
    void VectorTile::AssignBillboardSymbolLevel(boost::shared_ptr<symbolCartesianPoint>& cartesianPoint, boost::shared_ptr<Kml::Vectors> vecs){
        double mpx = 0.00;
        double mpy = 0.00;
        double mpz = 0.00;
        //boost::shared_ptr<symbolCartesianPoint> cartesianPoint;
        vecs->FindMidPoint(mpx, mpy, mpz);
        cartesianPoint.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//        switch (billboardLevel) {
//
//                //x2 label positions
//            case 2:
//                sym1x->_cartesianL2.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//            case 3:
//                sym1x->_cartesianL3.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//               break;
//
//                //x4 label positions
//            case 4:
//                 sym1x->_cartesianL4.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//            case 5:
//                 sym1x->_cartesianL5.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//            case 6:
//                 sym1x->_cartesianL6.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//            case 7:
//                 sym1x->_cartesianL7.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//
//                //x8 label positions
//            case 8:
//                sym1x->_cartesianL8.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//            case 9:
//                sym1x->_cartesianL9.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//
//            case 10:
//                sym1x->_cartesianL10.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//
//            case 11:
//                sym1x->_cartesianL11.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//
//            case 12:
//                sym1x->_cartesianL12.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//
//            case 13:
//                sym1x->_cartesianL13.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//
//            case 14:
//                sym1x->_cartesianL14.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//
//            case 8:
//                sym1x->_cartesianL8.reset(new symbolCartesianPoint(mpx, mpy, mpz));
//                break;
//
//            default:
//                break;
//        }
        //cartesianPoint.reset(new symbolCartesianPoint(mpx, mpy, mpz));
    }
    boost::shared_ptr<symbol> VectorTile::Assign1xBillboardSymbol(boost::shared_ptr<Kml::Vectors> vecs, std::string name, POIENUM billboardLevel)
    {
        //for(int i = 0 ; i < vecStreets->size() ; i++)
        
        boost::shared_ptr<symbol> sym;//(new symbol());
            //boost::shared_ptr<Kml::Vectors> vecs = (*(vecStreets.get()))[i];
            AssignElevations(vecs);//assign elevations and cartesian xyz
            std::string vecLabel = vecs->GetLabel();
            
            boost::shared_ptr<iconInfo> iinfo(new iconInfo(billboardLevel, ""));// = (*(_poiIconMap.get()))[classification];
            
            //pick a lat lon of a vector point somewhere towards the middle of the street line, then place a symbol with no icon there
            //change the logic in symbolmgr to look for SL_STREETLABEL, and when found don't use a flagpole, no icon and lower the text
            
            double xcart = 0.00;
            double ycart = 0.00;
            double zcart = 0.00;
            
            //TODO NOW WE USE LAT/LON OF POINT ON STREET VECTOR
            //int pointIndex = vecs->GetSize() / 2;
            double mpx = 0.00;
            double mpy = 0.00;
            double mpz = 0.00;
            
            double mplat = 0.00;
            double mplon = 0.00;
            double mpelev = 0.00;
            vecs->FindMidPoint(mpx, mpy, mpz);
            _coordConverter->CartesianToGeodetic(mpx, mpy, mpz, mplat, mplon, mpelev);
            
            double lat = mplat;
            double lon = mplon;
            double elev = mpelev;
            std::string nm = name;

            GLuint whiteTexture = findTextureBasedOnHref("white.png");
            GLuint texture = whiteTexture;//findTextureBasedOnHref(icon);

            if( !nm.empty() && texture )//&& !(icon.compare("pinred.png") == 0))//pinred filter doesn't work??
            {
                sym.reset(new symbol(mplat, mplon, mpx, mpy, mpelev, texture, _openglFont, nm, _userSettingsData,
                                                         _statusLogger, whiteTexture, iinfo));
                _symbols.push_back(sym);
                //BBOARD: RE-USE THE ALIGNMENT VECTORS FROM PATHWAY->AUTOALIGNED TEXT THAT HAVE BEEN SECTIONED OFF BY LEVEL IE L1 L2 L4
                //  USE THE VECS->FINDMIDPOINT FUNCTION FOR THEM AND ADD THE SYMBOLS TO THE APPROPRIATE LIST
            }
            else{
                //cout<<"icon name="<<icon<<endl;
            }
        return sym;
        
    }
    void VectorTile::AssignElevationsAndXYZ(boost::shared_ptr<Kml::Vectors> vecs)
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
    /////////////////
    void VectorTile::addTransNameLineString(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry)
    {

        std::string name;
        std::string classification;
        
        using kmldom::KmlFactory;
        //place a symbol at each location
        geovectileparser parser(_userSettingsData, _statusLogger);
        classification = findStringForKey("class", layer, feat);
        std::string ref = findStringForKey("ref", layer, feat);
        std::string network = findStringForKey("network", layer, feat);
        if(classification.compare("motorway")==0)
        {
            cout<<"found motorway ref = "<<ref<<" network="<<network<<endl;
        }
        name = findStringForKey("name", layer, feat);
        int extent = layer.extent();
        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecStreets(new std::vector<boost::shared_ptr<Kml::Vectors> >);
        bool outsideTileN = false;
        bool outsideTileS = false;
        bool outsideTileE = false;
        bool outsideTileW = false;

        //TODO: THESE 2 CHECKS NEED TO EVENTUALLY COME OUT!!!!!!!!!!!!!!! OTHERWISE THESE 2 STREETS
        //WON'T SHOW UP

        parser.parseLineString(vecGeometry, _x, _y, _zoomLevel, extent, vecStreets, outsideTileN, outsideTileS, outsideTileE, outsideTileW);
//        if(name.compare("Cumming Avenue") == 0 )
//        {
//            if(_x == 4000 && _y == 5781)
//                return;
//            else
//                std::cout<<"cumming avenue xy"<<_x<<" "<<_y<<endl;
//        }
//        if(name.find("11th St") != string::npos && _x == 4001 && _y == 5780)
//
//        {
//            return;
//            std::cout<<"xy"<<_x<<" "<<_y<<endl;
//        }
        //if the line goes outside the tile, we have to do some additional collision detects with other adjacent tiles
        //autoscale: we will now want to add the street vector points to our buffer that uniform buffer will have access to
        //      maybe add all points to a vector, take note of the vectorsize at beginning, that will be our start index
        //      we will need that later for the shader indices array.  Then send that start point to pathway, so that
        //      when we setup our street labels, the x of the center point value will be the index into that array
        //      make sure we don't exceed our size



        GLint whiteTexture = findTextureBasedOnHref("white.png");
        double lat1 = 0.00;
        double lon1 = 0.00;
        double alt1 = 0.00;
        double xcart1 = 0.00;
        double ycart1 = 0.00;
        double zcart1 = 0.00;
        
        double lat2 = 0.00;
        double lon2 = 0.00;
        double alt2 = 0.00;
        double xcart2 = 0.00;
        double ycart2 = 0.00;
        double zcart2 = 0.00;
        
        //GLint symTexture = findTextureBasedOnHref("white.png");
        for(int i = 0 ; i < vecStreets->size() ; i++)//usually only 1 street linestring returned so this normally 1
        {
            if(i == 1)
            {
                //cout<<"we got more than 1 street!!!"<<endl;
            }
            boost::shared_ptr<Kml::Vectors> vecs = (*(vecStreets.get()))[i];
            AssignElevationsAndXYZ(vecs);
            std::string vecLabel = vecs->GetLabel();
            //autoscale: pathway will now need to know the index of street_vertices.streetIndices to use
            //  the value of streetIndices will tell us the index into street_vertices.translation where our street data
            //  for this pathway starts.

            int vecssize = vecs->GetSize();
            vector<boost::shared_ptr<Kml::Point> > pts = vecs->GetStreetVectors();
            int ptsSize = pts.size();
            //autoscale: need to add xyz to uniform buffer and add end indices,
            //  we start with indices 0, so if we add 3 points then we need to add 3 to the indices array, that way were guaranteed
            //  that when we get an indices from the streetlabeltext, we will have index + 1 indices in the array to get the size.

            //we need to check if  vec3 translation[2000];  in our shader code will not be surpassed here, ie we can only have
            //2000 max vertices to add to the uniformbuffer/_currentUBOffsetStVec.
            //take _currentUBStIndices and add ptsSize, if this goes beyond 2000 we can't add this street label

            //check that we don't go beyond the cpu allocated buffer size, also
            //check that we aren't going to go beyond the vec3 translation[2000] limit in our shader
            //also check we don't go beyond the int streetIndices[501] variable in the shader
            //if any of these fail we need to abort adding this street, we can always try to make these values bigger later
            if(ptsSize * 16 + _currentUBOffsetStVec < UNIFORMBUFFSIZE && _currentUBStIndices + ptsSize < 800 && _currentUBIndicesIndices < 200)
            {

                _vectorFile<<ptsSize<<endl;
                //if(_streetLabelTempCounter < 500)//20 )//limit to 5 street labels for now
                //{
                    boost::shared_ptr<pathway> pw(new pathway(_userSettingsData, _statusLogger, _coordConverter, vecs, whiteTexture, _openglFont, _viewData, _EML,
                                                            name, _currentUBIndicesIndices, outsideTileN, outsideTileS, outsideTileE, outsideTileW));
                    *((int*)(_uniformBufferData + _currentUBOffsetIndices)) = _currentUBStIndices;//set starting point for this streetline
                    for(int i = 0 ; i < ptsSize ; i++)
                    {
                        //float val = 123.234;
                        boost::shared_ptr<Kml::Point> pt = pts[i];

                        //inject a test point to make it more realistic like a slight curve in a street
                        ((float*)(_uniformBufferData + _currentUBOffsetStVec))[0] = pt->GetX();
                        ((float*)(_uniformBufferData + _currentUBOffsetStVec))[1] = pt->GetY();
                        ((float*)(_uniformBufferData + _currentUBOffsetStVec))[2] = pt->GetZ();

                        _currentUBStIndices++;//keep track of index where this street will end so we can add to the street_vertices.streetIndices
                        _currentUBOffsetStVec += _autoScaleStVecStride;

                        if(_dumpStreetVecData)
                        {
                            _vectorFile<<pt->GetX()<<endl;
                            _vectorFile<<pt->GetY()<<endl;
                            _vectorFile<<pt->GetZ()<<endl;
//                            QVector4D pointVec(pt->GetX(), pt->GetY(), pt->GetZ(), 1.0);
//                            QVector4D resVec = globalmvp * pointVec;
//                            float ndcx = resVec.x()/resVec.w();
//                            float ndcy = resVec.y()/resVec.w();
//                            if(ndcx > -1.0 && ndcx < 1.0 && ndcy > -1.0 && ndcy < 1.0)
//                            {
//                                cout<<"found point at x,y"<<ndcx<<" "<<ndcy<<" "<<endl;
//                            }
                        }
                    }
                    //_currentUniformBufferIndexStVec

                    //this is the val the shader will use along with attribute to tell us what street data to use with which text string
                    //this value tells us where the next street label vertices will begin.

                    _currentUBOffsetIndices += _autoScaleIndicesStride;//this is the mem loc where the next street_vertices.streetIndices will go
                    //_currentUBOffsetIndices _currentUBStIndices;

                    //this is the int index into the street_vertices.indices array.  ie if this is the second
                    //street label we are storing, _currentUBIndicesIndices would be 1, then 2 after this line
                    //meaning we store the index into the streetvertices vecd3 array at this index inside street_vertices.indices
                    //  that value will be passed to the text attribute, so when we attempt to write a street label, we get a text vertex
                    //  that vertex will have this index value.  When the shader hits it, it will look into street_vertices.indices[1]
                    //  that will give us the index into the street_vertices.translation(streetvertex) array to look where our data starts

                    _currentUBIndicesIndices++;
                    _pathways.push_back(pw);
                    if(outsideTileN)
                        _pathwaysOutsideN.push_back(pw);
                    if(outsideTileS)
                        _pathwaysOutsideS.push_back(pw);
                    if(outsideTileE)
                        _pathwaysOutsideE.push_back(pw);
                    if(outsideTileW)
                        _pathwaysOutsideW.push_back(pw);
               // }
            //_streetLabelTempCounter++;//remove later!!
                //}
            }
            else{
                cout<<"too many streets to add !"<<endl;
            }
        }
    }
    //////////////////
//void VectorTile::addTransNameLineString(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry)
//{
//    std::string name;
//    std::string classification;
//
//    using kmldom::KmlFactory;
//    //place a symbol at each location
//    geovectileparser parser(_userSettingsData, _statusLogger);
//    classification = findStringForKey("class", layer, feat);
//    //cout<<"class="<<classification<<endl;
//    name = findStringForKey("name", layer, feat);
//    //cout<<"streetname = "<<name<<endl;
//    int extent = layer.extent();
//    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecStreets(new std::vector<boost::shared_ptr<Kml::Vectors> >);
//    parser.parseLineString(vecGeometry, _x, _y, _zoomLevel, extent, vecStreets);
//#if defined(__APPLE__)
//    GLuint whiteTexture = findTextureBasedOnHref("white.png");
//#else
//    boost::shared_ptr<QOpenGLTexture> whiteTexture = findTextureBasedOnHref("white.png");
//#endif
//    double lat1 = 0.00;
//    double lon1 = 0.00;
//    double alt1 = 0.00;
//    double xcart1 = 0.00;
//    double ycart1 = 0.00;
//    double zcart1 = 0.00;
//
//    double lat2 = 0.00;
//    double lon2 = 0.00;
//    double alt2 = 0.00;
//    double xcart2 = 0.00;
//    double ycart2 = 0.00;
//    double zcart2 = 0.00;
//#if defined(__APPLE__)
//    GLuint symTexture = findTextureBasedOnHref("white.png");
//#else
//    boost::shared_ptr<QOpenGLTexture> symTexture = findTextureBasedOnHref("white.png");
//#endif
//    for(int i = 0 ; i < vecStreets->size() ; i++)
//    {
//        boost::shared_ptr<Kml::Vectors> vecs = (*(vecStreets.get()))[i];
//        AssignElevations(vecs);
//        //cout<<"vecs size="<<vecs->GetSize()<<endl;
//        std::string vecLabel = vecs->GetLabel();
//        //_coordConverter->GeodeticToCartesian();
//        boost::shared_ptr<pathway> pw(new pathway(_userSettingsData, _statusLogger, _coordConverter, vecs, whiteTexture, _openglFont, _viewData, _EML, name));
//        //pw->SetText(vecLabel);
//        //pw->SetEML(_EML);
//        int vecssize = vecs->GetSize();
//        vector<boost::shared_ptr<Kml::Point> > pts = vecs->GetStreetVectors();
//        int ptsSize = pts.size();
//        boost::shared_ptr<Kml::Point> p1 = pts[0];
//        boost::shared_ptr<Kml::Point> p2 = pts[ptsSize - 1];
//
//        boost::shared_ptr<iconInfo> iinfo(new iconInfo(PM_CUSTOM,"white.png"));
//        lat1 = p1->GetLat();
//        lon1 = p1->GetLon();
//        alt1 = p1->GetAlt();
//        _coordConverter->GeodeticToCartesian(lat1, lon1, alt1, xcart1, ycart1, zcart1);
//        //boost::shared_ptr<symbol> sym1(new symbol(lat1, lon1, xcart1, ycart1, alt1, symTexture, _openglFont, "" , _userSettingsData,
//                                                 // _statusLogger, whiteTexture, iinfo));
//        //_symbols.push_back(sym1);
//
//        lat2 = p2->GetLat();
//        lon2 = p2->GetLon();
//        alt2 = p2->GetAlt();
//        _coordConverter->GeodeticToCartesian(lat2, lon2, alt2, xcart2, ycart2, zcart2);
//       // boost::shared_ptr<symbol> sym2(new symbol(lat2, lon2, xcart2, ycart2, alt2, symTexture, _openglFont, "" , _userSettingsData,
//       //                                           _statusLogger, whiteTexture, iinfo));
//        // _symbols.push_back(sym2);
//
//        // iconInfo(POIENUM pmType, std::string iconFilename):
//        //   _pmType(pmType),
//        //   _iconFileName(iconFilename)
//
//
//        //create 2 symbols one at each end so we see the whole line segment not just 2 lines stitched together
//        _pathways.push_back(pw);
//    }
//
//    //boost::shared_ptr<symbol> sym(new symbol(lat, lon, xcart, ycart, elev, texture, _openglFont, nm , _userSettingsData,
//    //   _statusLogger, whiteTexture, iinfo));
//    //_symbols.push_back(sym);
//}
void VectorTile::addStreetLine(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry)
{
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
    //boost::shared_ptr<Kml::Vectors> vecStreetLines(new std::vector<boost::shared_ptr<Kml::Vectors> >);
    
    bool outsideTileN = false;
    bool outsideTileS = false;
    bool outsideTileE = false;
    bool outsideTileW = false;
    //parser.parseLineString(vecGeometry, _x, _y, _zoomLevel, extent, vecStreets, outsideTileN, outsideTileS, outsideTileE, outsideTileW);
    parser.parseLineString(vecGeometry, _x, _y, _zoomLevel, extent, vecStreetLines, outsideTileN, outsideTileS, outsideTileE, outsideTileW);

    if(classification.compare("minor") == 0)
    {
        _vecStreetLinesMinor.push_back(vecStreetLines);
    }
    else if(classification.compare("service") == 0)
    {
        _vecStreetLinesService.push_back(vecStreetLines);
    }
    else if(classification.compare("secondary") == 0)
    {
        _vecStreetLinesSecondary.push_back(vecStreetLines);
    }
    else if(classification.compare("tertiary") == 0)
    {
        _vecStreetLinesTertiary.push_back(vecStreetLines);
    }
    else if(classification.compare("rail") == 0)
    {
        _vecStreetLinesRail.push_back(vecStreetLines);
    }
    else if(classification.compare("trunk") == 0)
    {
        _vecStreetLinesTrunk.push_back(vecStreetLines);
    }
    else if(classification.compare("primary") == 0)
    {
        _vecStreetLinesPrimary.push_back(vecStreetLines);
    }
    else if(classification.compare("motorway") == 0)
    {
        _vecStreetLinesMotorway.push_back(vecStreetLines);
    }
    else
        std::cout<<"missing street type: "<<classification<<std::endl;
    //std::cout<<"streetvec size="<<vecStreetLines->size()<<std::endl;
    //return iinfo;
}
boost::shared_ptr<iconInfo> VectorTile::addPlace(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry)
{
    std::string name;
    std::string classification;
    
    using kmldom::KmlFactory;
    //place a symbol at each location
    geovectileparser parser(_userSettingsData, _statusLogger);
    int extent = layer.extent();
    
    //cout<<_points->size();
    KmlFactory* factory = KmlFactory::GetFactory();
    kmldom::StylePtr ptrStyle = factory->CreateStyle();
    
    kmldom::IconStylePtr iconStyle = factory->CreateIconStyle();
    kmldom::IconPtr ptrIcon = factory->CreateIcon();
    kmldom::IconStyleIconPtr ptrIconStyleIcon = factory->CreateIconStyleIcon();
    classification = findStringForKey("class", layer, feat);
    boost::shared_ptr<iconInfo> iinfo;
    if(!classification.empty()){
        iinfo = (*(_placeIconMap.get()))[classification];
    }
    else
    {
        iinfo = (*(_placeIconMap.get()))["undefined"];
    }
    if(isCity(classification))
    {
        ptrIconStyleIcon->set_href("city.png");
    }
    else{
        ptrIconStyleIcon->set_href("poi.png");
    }
    iconStyle->set_icon(ptrIconStyleIcon);
    ptrStyle->set_iconstyle(iconStyle);
    
    name = findStringForKey("name", layer, feat);
    //cout<<"addplace preparseplace vecgeomsize ="<<vecGeometry->size()<<endl;
    _points = parser.parsePlace( vecGeometry, _x, _y, _zoomLevel, extent, ptrStyle, name);
    for(int i = 0 ; i < _points->size() ; i++)
    {
        boost::shared_ptr<Kml::Point> p = (*(_points.get()))[i];
        //cout<<"adding point ***"<<p->GetLat()<< endl;
        std::string str = p->GetIconHref();
    }
    //cout<<"adding point ***"<<endl;
    //cout<<"addplace postparseplace"<<endl;
    return iinfo;
}

bool VectorTile::isCity(std::string classification)
{
    //    'city'    Settlement of about 100,000 or more people.
    //    'town'    Urban or rural settlement of about 10,000-100,000 people
    //    'village'    Usually rural settlement of less than about 10,000
    //    'hamlet'    Rural settlement with a population of about 200 or fewer
    //    'suburb'    A distinct section of an urban settlement such as an annexed town, historical district, or large & important neighborhood.
    //    'neighbourhood'    A named part of a larger settlement
    //    'island'
    //    'islet'    A very small island.
    //    'archipelago'    Collective name for a group of islands
    //    'residential'    Named residential areas, including subdivisions and apartment complexes
    //    'aboriginal_lands'    Reservations and other aboriginal lands
    if(classification.compare("city") == 0 || classification.compare("town") == 0 ||
       classification.compare("village") == 0 || classification.compare("hamlet") == 0 ||
       classification.compare("suburb") == 0 || classification.compare("neighbourhood") == 0 ||
       classification.compare("island") == 0 || classification.compare("islet") == 0 ||
       classification.compare("archipelago") == 0 || classification.compare("residential") == 0 ||
       classification.compare("aboriginal_lands") == 0 )
        return true;
    else
        return false;
}
//find opengl texture by looking up href ie pinred.png etc.
#if defined(__APPLE__)
GLuint VectorTile::findTextureBasedOnHref(std::string href)
#else
boost::shared_ptr<QOpenGLTexture> VectorTile::findTextureBasedOnHref(std::string href)
#endif
{
    //std::vector<std::string> missingIcons;
    //todo: in here just look in the _texture map<string, qopengltexture> use href as the lookup
#if defined(__APPLE__)
	GLuint textureToBind;// = 0x0;
#else
    boost::shared_ptr<QOpenGLTexture> textureToBind;// = 0x0;
	textureToBind.reset();
#endif
    
    //textureToBind = _symbolTextures["pinred.png"];
    
    //todo: use _symbolTextures and lookup name from href
    //symbolTextures[href] is the texture we want
    //check if it exists first if not, use default of _pinretexturera
    if ( _symbolTextures->find(href) == _symbolTextures->end() )
    {
        boost::mutex::scoped_lock lock(missingIconMutex);
        
        // not found desired symbol
        bool foundRepeat = false;
        for(int jj = 0 ; jj < _missingIcons.size() ; jj++)
        {
            if(_missingIcons[jj].compare(href) == 0)
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
            _missingIcons.push_back(href);
            WriteToStatusLog(ss.str(), true, true);
        }
        
        
        if(_symbolTextures->find("pinred.png") == _symbolTextures->end() ) {
            //didn't find default symbol even
            bool foundRepeat2 = false;
            for(int jj = 0 ; jj < _missingIcons.size() ; jj++)
            {
                if(_missingIcons[jj].compare("pinred.png") == 0)
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
                _missingIcons.push_back("pinred.png");
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
void VectorTile::SetUniformBufferBindingPoint(GLuint bindingPoint)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, _ubBindingPoint, _uniformBuffer);
    _ubBindingPoint = bindingPoint;
}

extern GLuint gbl_u_SamplerLocation ;
extern GLuint gbl_u_lblDensityLevelLocationAutoScale;
bool VectorTile::RenderPerspectiveEntities(GLuint program, bool limitMet, boost::shared_ptr<SViewData> viewData, RIMatrix4x4 projection, double pixelSize, OVERLAYTYPE overlayType, bool billboardStyle, double textScale, int labelDensity)

{
    _viewData = viewData;
    double vehx, vehy, vehz = 0.0;
    _coordConverter->GeodeticToCartesian(viewData->dVehicleLat, viewData->dVehicleLon, viewData->dVehicleAltitude, vehx, vehy, vehz);
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
           bool openglSetupComplete = SetupOpenglForEntities(program);
           if(openglSetupComplete)
           {
               createdGLEntities = true;//let lfclient know that we created some entities so it
               //can keep track and limit the amount of them done so we don't bog down the cpu
               _readyToRender = true;
           }
           return createdGLEntities;
       }
    if(overlayType == OT_STREETLABEL)
    {
    //STREETLABELS: MAKE STREETLABEL ARRAYBUF CURRENT, MAKE TEXT ATLAS CURRENT
#if defined(__APPLE__)
#if defined(AUTOSCALETEXT)
        if(_oldBindingPoint != _ubBindingPoint)
        {
            //we have a new binding point to bind to for our street data
            if(_ubBindingPoint != -1)//if it is 1, that means we reset to unused
                SetUniformBufferBindingPoint(_ubBindingPoint);
            _oldBindingPoint = _ubBindingPoint;
        }
        //autoscale: need to bind the new shader program DONE
        //  the vector tile has all the street data that the uniformblock will have.
        //  so we need to switch the binding point.  We need to get the uniformblock index which the shader knows
        //  and we need to have this streetline buffer set to a binding point.  Then just switch the uniformblock
        //  binding point to the binding point that this vector tile has its street data set to.
        glBindBuffer(GL_ARRAY_BUFFER, _arrayBufStreetLabels);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufStreetLabels);
        
        //_arrayBufStreetLabels.bind();
        //_indexBufStreetLabels.bind();
        std::shared_ptr<std::vector<unsigned int> > labelIndices(new vector<unsigned int>);
        _openglFont->bindFont();//ie bind font.png texture
        int offset = 0;
        //"in vec2 texcoord;                                                          \n"
        //"in vec4 position;                                                          \n"
        int vertexCoordLoc = glGetAttribLocation(program,"position");
        
        // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
        int textureCoordLoc = glGetAttribLocation(program,"texcoord");
        int centerCoordLoc = glGetAttribLocation(program,"center");
        
        glVertexAttribPointer ( vertexCoordLoc, 3, GL_FLOAT,
                               GL_FALSE, 8 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));//_tileGridVertices
        glEnableVertexAttribArray ( vertexCoordLoc );
        
        glVertexAttribPointer ( textureCoordLoc, 2, GL_FLOAT,
                               //GL_FALSE, 5 * sizeof(GLfloat), &(_tileGridVertices[3]) );//_tileGridVertices
                               GL_FALSE, 8 * sizeof(GLfloat),  (void*)(3 * sizeof(GLfloat)));//_tileGridVertices
        glEnableVertexAttribArray ( textureCoordLoc );
        
        glVertexAttribPointer ( centerCoordLoc, 3, GL_FLOAT,
                               //GL_FALSE, 5 * sizeof(GLfloat), &(_tileGridVertices[3]) );//_tileGridVertices
                               GL_FALSE, 8 * sizeof(GLfloat),  (void*)(5 * sizeof(GLfloat)));//_tileGridVertices
        glEnableVertexAttribArray ( centerCoordLoc );
        
        
       
       
       
        for(int ilabelLevel = labelDensity ; ilabelLevel < (2 * labelDensity)  ; ilabelLevel++)
        {
            //the cases should be labelDensity == 1, 2, 4, 8
        
            //DENSITY: NEED TO PUT THIS IN A LOOP BASED ON IF WE'RE LOOKING AT 1X 2X 4X LABEL DENSITY ETC
            //DENSITY: FOR MODE 2/3 LABELS, WE'LL HAVE TO CLEAR/GETLABELINDICES IN BETWEEN DRAW CALLS
            _streetLabelIndices->clear();
            if(billboardStyle == false)//we're not drawing billboards as street labels, we're drawing these which lay on the ground
            {
                
                for(int i = 0 ; i < _pathways.size() ; i++)
                {
                    boost::shared_ptr<pathway> pw = _pathways[i];
                    //DENSITY: WE SHOULD ADD A CALL SAYING WHAT LABELSET WE'RE TRYING TO DRAW IE 1 OR 2(IE 1/4 POS LABEL) 3(IE 3/4 POSOITION LABEL)
                    pw->GetLabelIndices(_streetLabelIndices, _viewData, textScale, ilabelLevel, vehx, vehy, vehz);
                }

            }
            
            if(_streetLabelIndices->size() == 0)
            {
                continue;
            }
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            unsigned int* ptrIndices = &(*(_streetLabelIndices.get()))[0];
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufStreetLabels);
            glUniform1i(gbl_u_lblDensityLevelLocationAutoScale, ilabelLevel);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _streetLabelIndices->size() * sizeof(unsigned int), ptrIndices, GL_STATIC_DRAW);
            glDrawElements(
                GL_TRIANGLES,      // mode
                (GLuint)_streetLabelIndices->size(),    // count

                //(GLuint)testVecIndices->size(),    // count
                GL_UNSIGNED_INT,   // type
                (void*)0           // element array buffer offset
                );
        }
//        _streetLabelIndices->clear();
//        if(billboardStyle == false)//we're not drawing billboards as street labels, we're drawing these which lay on the ground
//        {
//            for(int i = 0 ; i < _pathways.size() ; i++)
//            {
//                boost::shared_ptr<pathway> pw = _pathways[i];
//                //DENSITY: WE SHOULD ADD A CALL SAYING WHAT LABELSET WE'RE TRYING TO DRAW IE 1 OR 2(IE 1/4 POS LABEL) 3(IE 3/4 POSOITION LABEL)
//                pw->GetLabelIndices(_streetLabelIndices, _viewData, textScale, 3);
//            }
//        }
//        glUniform1i(gbl_u_lblDensityLevelLocationAutoScale, 3);
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        ptrIndices = &(*(_streetLabelIndices.get()))[0];
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufStreetLabels);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _streetLabelIndices->size() * sizeof(unsigned int), ptrIndices, GL_STATIC_DRAW);
//        glDrawElements(
//            GL_TRIANGLES,      // mode
//            (GLuint)_streetLabelIndices->size(),    // count
//
//            //(GLuint)testVecIndices->size(),    // count
//            GL_UNSIGNED_INT,   // type
//            (void*)0           // element array buffer offset
//            );
        
#else
    glBindBuffer(GL_ARRAY_BUFFER, _arrayBufStreetLabels);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufStreetLabels);
    boost::shared_ptr<std::vector<unsigned int> > labelIndices(new vector<unsigned int>);
    
    _openglFont->bindFont();//ie bind font.png texture//TODO: ADD BACK IN
    
    int vertexCoordLoc = glGetAttribLocation(program,"a_position");
    
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int textureCoordLoc = glGetAttribLocation(program,"a_texCoord");

    double bbn = _boundingBox.north;
    double bbs = _boundingBox.south;
    double bbe = _boundingBox.east;
    double bbw = _boundingBox.west;
    
    glVertexAttribPointer ( vertexCoordLoc, 3, GL_FLOAT,
                           GL_FALSE, 5 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));//_tileGridVertices
    glEnableVertexAttribArray ( vertexCoordLoc );
    
    glVertexAttribPointer ( textureCoordLoc, 2, GL_FLOAT,
                           //GL_FALSE, 5 * sizeof(GLfloat), &(_tileGridVertices[3]) );//_tileGridVertices
                           GL_FALSE, 5 * sizeof(GLfloat),  (void*)(3 * sizeof(GLfloat)));//_tileGridVertices
    glEnableVertexAttribArray ( textureCoordLoc );
    
    

    
    _streetLabelIndices->clear();
    if(billboardStyle == false)//we're not drawing billboards as street labels, we're drawing these which lay on the ground
    {
        for(int i = 0 ; i < _pathways.size() ; i++)
        {
            boost::shared_ptr<pathway> pw = _pathways[i];
            
            pw->GetLabelIndices(_streetLabelIndices, _viewData, pixelSize);
        }
    }
    
    unsigned int* ptrIndices = &(*(_streetLabelIndices.get()))[0];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufStreetLabels);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _streetLabelIndices->size() * sizeof(unsigned int), ptrIndices, GL_STATIC_DRAW);
        glDrawElements(
        GL_TRIANGLES,      // mode
        (GLuint)_streetLabelIndices->size(),    // count
        
        //(GLuint)testVecIndices->size(),    // count
        GL_UNSIGNED_INT,   // type
        (void*)0           // element array buffer offset
        );
#endif
#else
#endif
    
        //DENSITY: IF DENSITY = NORMAL, JUST CALL GLDRAW, IF DENSITY = 2X SET MODE IN SHADER TO 2, DO A DRAW THEN SET MODE TO 3 AND DO ANOTHER DRAW
    }//end if(STREETLABEL)
    if(overlayType == OT_STREETLINE)
    {
        _streetLineManager.Render(program, viewData, projection, pixelSize );
        _streetLineManagerCollar.Render(program, viewData, projection, pixelSize );
    }
    return true;
}

bool VectorTile::SetupOpenglForEntities(GLuint program)
{
    //these 3 lines need to be on main thread, other than that all the parsing can be done on
    //a seperate thread to smooth things out
    bool allSymbolsLoaded = true;
    //bool symbolLoaded = false;
    int loadLimiter = 1;
    int loadLmitCounter = 0;
    //BBOARD: NEED A LOOP FOR THE BILLBOARDED LEVEL 1 2 4 8 16 LABELS HERE TOO
    for(int i = 0 ; i < _symbols.size() ; i++)//BBOARD: IF ALLSYMBOLS LOADED == FALSE FROM PREVIOUS SYMBOL LOADS, THEN JUST RETURN
    {
        boost::shared_ptr<symbol> sym = _symbols[i];
        if(!sym->GetEntitiesInitialized())//check to see if we previously loaded symbol
        {
            loadLmitCounter++;
            sym->InitOpenGLEntities();
        }
        if(loadLmitCounter >= loadLimiter)
        {
            //lets bail on loading more symbols so we don't bog down the main render loop
            allSymbolsLoaded = false;
            break;
        }
    }
    
#if defined(__APPLE__)
    if(allSymbolsLoaded)
    {
        //were ready to render, allocate  the street label buffers, vertices are already loaded
        //just need to make the opengl calls in the render loop
        //setup vertex buffer for street labels
        float* ptrVertices = &(*(_streetLabelVertices.get()))[0];
        //cout<<"ptrvert="<<ptrVertices<<endl;
        
        
        

        glGenBuffers(1, &_arrayBufStreetLabels);
        glBindBuffer(GL_ARRAY_BUFFER, _arrayBufStreetLabels);
        glBufferData(GL_ARRAY_BUFFER, _streetLabelVertices->size() * sizeof(float), &(*_streetLabelVertices.get())[0], GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, testVecVerts->size() * sizeof(float), &(*testVecVerts.get())[0], GL_STATIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        

        unsigned int* ptrIndices = &(*(_streetLabelIndices.get()))[0];
        glGenBuffers(1, &_indexBufStreetLabels);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufStreetLabels);
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _streetLabelIndices->size() * sizeof(unsigned int), ptrIndices, GL_STATIC_DRAW);
        
        _streetLineManager.SetupOpenglForEntities(program);
        _streetLineManagerCollar.SetupOpenglForEntities(program);
        
        glGenBuffers(1, &_uniformBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffer);
        glBufferData(GL_UNIFORM_BUFFER, UNIFORMBUFFSIZE, _uniformBufferData, GL_DYNAMIC_DRAW);
    }
#else
        if(allSymbolsLoaded)
        {
            //were ready to render, allocate  the street label buffers, vertices are already loaded
            //just need to make the opengl calls in the render loop
            //setup vertex buffer for street labels
            float* ptrVertices = &(*(_streetLabelVertices.get()))[0];
            //cout<<"ptrvert="<<ptrVertices<<endl;
            _arrayBufStreetLabels.create();
            _arrayBufStreetLabels.bind();
            _arrayBufStreetLabels.allocate(ptrVertices, _streetLabelVertices->size() *  sizeof(float));
            _indexBufStreetLabels.create();
            _indexBufStreetLabels.bind();
             unsigned int* ptrIndices = &(*(_streetLabelIndices.get()))[0];
             _indexBufStreetLabels.allocate(ptrIndices, _streetLabelIndices->size() * sizeof(unsigned int));
        }
#endif
    std::cout<<"size of minor roads="<<_vecStreetLinesMinor.size()<<std::endl;
    return allSymbolsLoaded;
    
}
#include <fstream>
void VectorTile::OutputTileData(const vector_tile::Tile& newtile, std::string filename)
{
    ofstream outbin;
    filename.erase(filename.size() - 1);
    filename.erase(filename.size() - 1);
    filename.erase(filename.size() - 1);
    filename.append("txt");
    //outbin.open(filename);//, ios::out|ios::as);
    outbin.open(filename);//, ios::out|ios::as);
    for (std::size_t j=0;j<static_cast<std::size_t>(newtile.layers_size());j++)
    {
        vector_tile::Tile_Layer const& layer = newtile.layers(j);
        //cout<<layer.name()<<endl;
        outbin << "layer: " << layer.name() << "\n";
        outbin << "  version: " << layer.version() << "\n";
        outbin << "  extent: " << layer.extent() << "\n";
        outbin << "  keys: ";
        for (std::size_t k=0;k < static_cast<std::size_t>(layer.keys_size());k++)
        {
            std::string const& key = layer.keys(k);
            outbin << key;
            if (k<static_cast<std::size_t>(layer.keys_size())-1) {
                outbin << ",";
            }
        }
        outbin << "\n";
        outbin << "  values: ";
        for (std::size_t l=0;l<static_cast<std::size_t>(layer.values_size());l++)
        {
            vector_tile::Tile_Value const & value = layer.values(l);
            if (value.has_string_value()) {
                outbin << value.string_value();
            } else if (value.has_int_value()) {
                outbin << value.int_value();
            } else if (value.has_double_value()) {
                outbin << value.double_value();
            } else if (value.has_float_value()) {
                outbin << value.float_value();
            } else if (value.has_bool_value()) {
                outbin << value.bool_value();
            } else if (value.has_sint_value()) {
                outbin << value.sint_value();
            } else if (value.has_uint_value()) {
                outbin << value.uint_value();
            } else {
                outbin << "null";
            }
            if (l<static_cast<std::size_t>(layer.values_size())-1) {
                outbin << ",";
            }
        }
        outbin << "\n";
        for (std::size_t l=0;l<static_cast<std::size_t>(layer.features_size());l++)
        {
            vector_tile::Tile_Feature const & feat = layer.features(l);
            if (feat.has_id()) {
                outbin << "  feature: " << feat.id() << "\n";
            } else {
                outbin << "  feature: (no id set)\n";
            }
            outbin << "    type: ";
            unsigned feat_type = feat.type();
            if (feat_type == 0) {
                outbin << "Unknown";
            } else if (feat_type == 1) {
                outbin << "Point";
            } else if (feat_type == 2) {
                outbin << "LineString";
            } else if (feat_type == 3) {
                outbin << "Polygon";
            }
            outbin << "\n";
            outbin << "    tags: ";
            for (std::size_t m=0;m<static_cast<std::size_t>(feat.tags_size());m++)
            {
                uint32_t tag = feat.tags(m);
                outbin << tag;
                if (m<static_cast<std::size_t>(feat.tags_size())-1) {
                    outbin << ",";
                }
            }
            outbin << "\n";
            outbin << "    geometries: ";
            for (std::size_t m=0;m<static_cast<std::size_t>(feat.geometry_size());m++)
            {
                uint32_t geom = feat.geometry(m);
                outbin << geom;
                if (m<static_cast<std::size_t>(feat.geometry_size())-1) {
                    outbin << ",";
                }
            }
            outbin << "\n";
        }
        outbin << "\n";
    }
    outbin.close();
}
extern std::string GetDataDirectory();
void VectorTile::DataLoadFunction()
{
    WriteToStatusLog("loading vector data", false, false);
    boost::asio::io_service io_service2_;
    std::stringstream vtsTileSS;
    std::stringstream pbfFileName;
    
    //pbf filename for icons/placemarks we parse ourselves
    pbfFileName<<_pngFolder<<_zoomLevel<<"-"<<_x<<"-"<<_y<<".pbf";
    vtsTileSS<<"/data/v3/"<<_zoomLevel<<"/"<<_x<<"/"<<_y<<".pbf";
    std::string pbfss(vtsTileSS.str());
    std::string pbf(pbfFileName.str());
    
    //TODO: SEE IF FILE ALREADY EXISTS IN CURRENT FILE SYSTEM, IF YES THEN JUST OPEN THAT AND BYPASS GOING TO THE WEB
    
    std::string iconFolderPath = GetDataDirectory();
    iconFolderPath.append("/");
    iconFolderPath.append(pbf);
    std::string errorMsg;
    if(!FileExists(iconFolderPath, errorMsg))
    {
        //std::string rismsg("can't find file ");
        //rismsg.append(fullPath);
        //WriteToStatusLog(errorMsg, true, true);
        //this->WriteToStatusLog(rismsg, true, true);
        //return 0x00;
        
        
        //if file doesn't exist, lets go to the tileserver to grab it
        geotileservergl tileserver2(io_service2_, _tileserverIP, _tileserverPort, pbfss, pbf, _userSettingsData, _statusLogger);//TS: GONNA NEED TO GRAB ADDRESS FROM SETTINGS.INI FILE
        io_service2_.run();
        io_service2_.stop();
    }
    else{
        std::cout<<"found file "<<iconFolderPath<<" from data cache"<<endl;
    }
    
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    std::stringstream ss;
    
    bool verbose = true;
    
    try
    {
#if defined(__APPLE__)
        LFObjCFunctionsCPPWrapper ocWrapper;
	std::stringstream ss2;
        std::string filename = pbfFileName.str();
    	std::string pbfFullPath = ocWrapper.GetPbfPath(filename);
        std::ifstream stream(pbfFullPath.c_str(),std::ios_base::in|std::ios_base::binary);
#else
        std::stringstream ss2;
        std::string filename = pbfFileName.str();
        std::ifstream stream(filename.c_str(),std::ios_base::in|std::ios_base::binary);
#endif
        if (!stream.is_open())
        {
            throw std::runtime_error("could not open: '" + filename + "'");
        }
        
        // we are using lite library, so we copy to a string instead of using ParseFromIstream
        std::string message(std::istreambuf_iterator<char>(stream.rdbuf()),(std::istreambuf_iterator<char>()));
        stream.close();
        
        // now attempt to open protobuf
        vector_tile::Tile tile;
        bool is_zlib = mapnik::vector_tile_impl::is_zlib_compressed(message);
        bool is_gzip = mapnik::vector_tile_impl::is_gzip_compressed(message);
        if (is_zlib || is_gzip)
        {
            if (is_zlib)
            {
                //std::cout << "message: zlib compressed\n";
            }
            else if (is_gzip)
            {
                //std::cout << "message: gzip compressed\n";
            }
            std::string uncompressed;
            mapnik::vector_tile_impl::zlib_decompress(message,uncompressed);
            if (!tile.ParseFromString(uncompressed))
            {
                std::clog << "failed to parse compressed protobuf\n";
            }
        }
        else
        {
            //std::cout << "message: appears not to be compressed\n";
            if (!tile.ParseFromString(message))
            {
                std::clog << "failed to parse protobuf\n";
            }
        }
        
        if(_vectorDataDump){
            OutputTileData(tile, pbfFileName.str());
        }
        //iterate through all layers for this tile
        for (std::size_t j=0;j<static_cast<std::size_t>(tile.layers_size());++j)
        {
            vector_tile::Tile_Layer const& layer = tile.layers(j);
            
            //outbin << "layer: " << layer.name() << "\n";
            //outbin << "  version: " << layer.version() << "\n";
            //outbin << "  extent: " << layer.extent() << "\n";
            //outbin << "  keys: ";
            //iterate through all features for this layer
            for (std::size_t l=0;l < static_cast<std::size_t>(layer.features_size());++l)
            {
                vector_tile::Tile_Feature const & feat = layer.features(l);
                unsigned feat_type = feat.type();
                
                boost::shared_ptr<std::vector<uint32_t> > vecGeometry(new std::vector<uint32_t>);
                for (std::size_t m=0;m<static_cast<std::size_t>(feat.geometry_size());++m)
                {
                    uint32_t geom = feat.geometry(m);

                    vecGeometry->push_back(geom);
                }
                if(layer.name().compare("transportation") == 0)
                {
                    std::cout<<"found"<<endl;
                    //addStreetLine(layer, feat, vecGeometry);
                    _streetLineManager.addStreetLine(layer, feat, vecGeometry);
                    _streetLineManagerCollar.addStreetLine(layer, feat, vecGeometry);

                }
                if(layer.name().compare("place") == 0 && feat_type == 1)//this is a point, lets display it
                {
                    boost::shared_ptr<iconInfo> iinfo = addPlace(layer, feat, vecGeometry);
                    //_points now contains the data, cycle through it and add the symbols
                    for(int i = 0 ; i < _points->size() ; i++)
                    {
                        double xcart = 0.00;
                        double ycart = 0.00;
                        double zcart = 0.00;
                        boost::shared_ptr<Kml::Point> pt = (*(_points.get()))[i];
                        double lat = pt->GetLat();
                        double lon = pt->GetLon();
                        double elev = _EML->GetElevation(lat, lon);
                        std::string nm = pt->GetName();
                        std::string icon = pt->GetIconHref();
#if defined(__APPLE__)
                        GLuint texture = findTextureBasedOnHref(icon);
                        GLuint whiteTexture = findTextureBasedOnHref("white.png");
#else
                        boost::shared_ptr<QOpenGLTexture> texture = findTextureBasedOnHref(icon);
                        boost::shared_ptr<QOpenGLTexture> whiteTexture = findTextureBasedOnHref("white.png");
#endif
                        _coordConverter->GeodeticToCartesian(lat, lon, elev, xcart, ycart, zcart);
                        if(!nm.empty() && texture && !(icon.compare("pinred.png") == 0))//pinred filter doesn't work??
                        {
                            boost::shared_ptr<symbol> sym(new symbol(lat, lon, xcart, ycart, elev, texture, _openglFont, nm , _userSettingsData,
                                                                     _statusLogger, whiteTexture, iinfo));
                            _symbols.push_back(sym);
                        }
                        else{
                        }
                    }
                    
                }
                if(layer.name().compare("poi") == 0 )//&& feat_type == 1)//this is a point lets display it
                {
                    boost::shared_ptr<iconInfo> iinfo = addPoi(layer, feat, vecGeometry);//sets up _points so we can read it here
                    for(int i = 0 ; i < _points->size() ; i++)
                    {
                        double xcart = 0.00;
                        double ycart = 0.00;
                        double zcart = 0.00;
                        boost::shared_ptr<Kml::Point> pt = (*(_points.get()))[i];
                        double lat = pt->GetLat();
                        double lon = pt->GetLon();
                        double elev = _EML->GetElevation(lat, lon);
                        std::string icon = pt->GetIconHref();
                        std::string nm = pt->GetName();
#if defined(__APPLE__)
			GLuint whiteTexture = findTextureBasedOnHref("white.png");
                        GLuint texture = findTextureBasedOnHref(icon);
#else
                        boost::shared_ptr<QOpenGLTexture> whiteTexture = findTextureBasedOnHref("white.png");
                        boost::shared_ptr<QOpenGLTexture> texture = findTextureBasedOnHref(icon);
#endif
                        _coordConverter->GeodeticToCartesian(lat, lon, elev, xcart, ycart, zcart);
                        if( !nm.empty() && texture && !(icon.compare("pinred.png") == 0))//pinred filter doesn't work??
                        {
                            boost::shared_ptr<symbol> sym(new symbol(lat, lon, xcart, ycart, elev, texture, _openglFont, nm, _userSettingsData,
                                                                     _statusLogger, whiteTexture, iinfo));
                            _symbols.push_back(sym);
                        }
                        else{
                            //cout<<"icon name="<<icon<<endl;
                        }
                    }
                    
                }
                if(layer.name().compare("transportation_name") == 0 )//&& feat_type == 1)//this is a point lets display it
                {
                    
                    
                    std::string name;
                    std::string classification;

                    using kmldom::KmlFactory;
                    //place a symbol at each location
                    //geovectileparser parser(_userSettingsData, _statusLogger);
                    classification = findStringForKey("class", layer, feat);
                    std::string ref = findStringForKey("ref", layer, feat);
                    std::string network = findStringForKey("network", layer, feat);
                    //cout<<"class="<<classification<<"ref="<<ref<<"network="<<network<<endl;
                    //if(classification.compare("motorway")==0 || classification.compare("tertiary") == 0 || classification.compare("trunk") == 0 || classification.compare("secondary") == 0)
                    if(!ref.empty())
                    {
                        boost::shared_ptr<iconInfo> iinfo = addStreetShield(layer, feat, vecGeometry);

                        //std::shared_ptr<iconInfo> iinfo = addPlace(layer, feat, vecGeometry);
                        //_points now contains the data, cycle through it and add the symbols
                        for(int i = 0 ; i < _points->size() ; i++)
                        {
                            double xcart = 0.00;
                            double ycart = 0.00;
                            double zcart = 0.00;
                            boost::shared_ptr<Kml::Point> pt = (*(_points.get()))[i];
                            double lat = pt->GetLat();
                            double lon = pt->GetLon();
                            double elev = _EML->GetElevation(lat, lon);
                            std::string nm = pt->GetName();
                            cout<<"RECT STreet name "<<nm<<endl;
                            std::string icon = pt->GetIconHref();
    //#if defined(RESPONDAR)
                            GLuint texture = findTextureBasedOnHref(iinfo->_iconFileName);
                            GLuint whiteTexture = findTextureBasedOnHref("white.png");
//                    #else
//                                            std::shared_ptr<QOpenGLTexture> texture = findTextureBasedOnHref(iinfo->_iconFileName);
//                                            std::shared_ptr<QOpenGLTexture> whiteTexture = findTextureBasedOnHref("white.png");
//                    #endif
                            _coordConverter->GeodeticToCartesian(lat, lon, elev, xcart, ycart, zcart);
                            if(!nm.empty() && texture && !(icon.compare("pinred.png") == 0))//pinred filter doesn't work??
                            {
                                boost::shared_ptr<symbol> sym(new symbol(lat, lon, xcart, ycart, elev, texture, _openglFont, nm , _userSettingsData,
                                                                         _statusLogger, whiteTexture, iinfo));
                                if(sym)
                                    _symbols.push_back(sym);//BBOARD: I THINK THIS IS FOR SHIELDS
                            }
                            else{
                            }
                        }
                    }
                    else
                    {
                        addTransNameBillboard(layer, feat, vecGeometry);
                        addTransNameLineString(layer, feat, vecGeometry);
                    }
                }
            }
        }
        *((int*)(_uniformBufferData + _currentUBOffsetIndices)) = _currentUBStIndices;
        if(_vectorDataDump){
            // outbin.close();//, ios::out|ios::as);
            // OutputTileData(tile, pbfFileName.str());
        }
        //we're now done with the pbf file, if tilecacheclear is set, then delete the file
        
        if(_clearCacheFolder)
        {
            std::string strError;
            bool success = DeleteFile(pbfFileName.str(), strError);
            if(!success)
            {
                WriteToStatusLog(strError, true, true);
            }
        }
        
    }
    catch (std::exception const& ex)
    {
        //        std::clog << "error: " << ex.what() << "\n";
        //        //return -1;
    }
    
    for(int i = 0 ; i < _pathways.size() ; i++)
    {
        //todo: here we want to create an arraybuffer, send a link to it to each pathway(streettext)
        //  each pathway will add it's vertices/texel info to that array.  ie each vector tile will
        //  have a big arraybuffer containing all the street text/label data ie northfacing text
        //  southfacing text etc.  Then when we call render for each pathway, they will append the indices
        //  to a list that the shader will use to render whichever version of the streetlabel we want.
        boost::shared_ptr<pathway> pw = _pathways[i];
        pw->AddLabelVertices(_streetLabelVertices);
    }

    for(int i = 0 ; i < _pathways.size() ; i++)
    {
        boost::shared_ptr<pathway> pw1 = _pathways[i];
        for(int j = i + 1; j < _pathways.size() ; j++)
        {
            boost::shared_ptr<pathway> pw2 = _pathways[j];
            pw1->DetectLabelCollisions(pw2);
        }
    }
    _readyToSetupOpengl = true;
}
    boost::shared_ptr<iconInfo> VectorTile::addStreetShield(vector_tile::Tile_Layer const& layer, vector_tile::Tile_Feature const & feat, boost::shared_ptr<std::vector<uint32_t> > vecGeometry)
    {
        using kmldom::KmlFactory;
        KmlFactory* factory = KmlFactory::GetFactory();
        kmldom::StylePtr ptrStyle = factory->CreateStyle();
        std::string classification = findStringForKey("class", layer, feat);
        std::string name = findStringForKey("name", layer, feat);
        std::string ref = findStringForKey("ref", layer, feat);
        std::string network = findStringForKey("network", layer, feat);
        boost::shared_ptr<iconInfo> iinfo = (*(_poiIconMap.get()))["interstate"];
        //if(classification.compare("motorway") == 0)
        {
            iinfo = (*(_poiIconMap.get()))["whiterectangle"];
            if(network.compare("us-interstate") == 0)
            {
                iinfo = (*(_poiIconMap.get()))["interstate"];
            }
            if(network.compare("us-highway") == 0)
            {
                iinfo = (*(_poiIconMap.get()))["highway"];
            }
            if(network.compare("us-state") == 0)
                iinfo = (*(_poiIconMap.get()))["highway"];
        }
        int extent = layer.extent();

        geovectileparser parser(_userSettingsData, _statusLogger);
        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecStreets(new std::vector<boost::shared_ptr<Kml::Vectors> >);
        bool outsideTileN = false;//does this line go outside the tile boundary
        bool outsideTileS = false;//does this line go outside the tile boundary
        bool outsideTileE = false;//does this line go outside the tile boundary
        bool outsideTileW = false;//does this line go outside the tile boundary
        parser.parseLineString(vecGeometry, _x, _y, _zoomLevel, extent, vecStreets, outsideTileN, outsideTileS, outsideTileE, outsideTileW);

        std::vector<boost::shared_ptr<StreetSegment> > streetSegments;//to be filled in from function

        boost::shared_ptr<Kml::Vectors>  vecs = (*(vecStreets.get()))[0];
        AssignElevationsAndXYZ(vecs);
        std::vector<boost::shared_ptr<Kml::Point> > vecpoints = vecs->GetStreetVectors();
        //std::shared_ptr<std::vector<std::shared_ptr<Kml::Point> > > sppoints(new std::vector<std::shared_ptr<Kml::Point> > );
        //(*(sppoints.get())) = vecpoints;
        double totalLength = CreateStreetSegmentVector(_coordConverter, vecpoints, streetSegments);
        double centerx, centery, centerz, lat, lon;
        findCenterLocationSegments(streetSegments, centerx, centery, centerz, lat, lon);

       // std::vector<std::shared_ptr<Kml::Point> > points;
        boost::shared_ptr<Kml::Point> shieldPoint(new Kml::Point);
        ////////////////

        kmldom::IconStylePtr iconStyle = factory->CreateIconStyle();
        kmldom::IconPtr ptrIcon = factory->CreateIcon();
        kmldom::IconStyleIconPtr ptrIconStyleIcon = factory->CreateIconStyleIcon();

        iconStyle->set_icon(ptrIconStyleIcon);
        ptrStyle->set_iconstyle(iconStyle);

        shieldPoint->SetX(centerx);
        shieldPoint->SetY(centery);
        shieldPoint->SetZ(centerz);
        shieldPoint->SetLat(lat);
        shieldPoint->SetLon(lon);
        shieldPoint->SetAlt(centerz);
        shieldPoint->SetStyle(ptrStyle);
        shieldPoint->SetName(ref);

        _points.reset(new std::vector<boost::shared_ptr<Kml::Point> >);
        _points->push_back(shieldPoint);

        return iinfo;
    }
int VectorTile::SetEML(boost::shared_ptr<ElevationManagerLocal> eml) {
    _EML = eml;
    _streetLineManager.SetEML(eml);
    _streetLineManagerCollar.SetEML(eml);
    return 1;
}


