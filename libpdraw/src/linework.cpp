#include "linework.h"
#include <QMatrix4x4>


QMatrix4x4 _modelViewMatrixLW;
QMatrix4x4 _normalMatrixLW;
QMatrix4x4 _modelViewProjectionMatrixLW;
QMatrix4x4 _projectionMatrix;

extern int lwpositionLocation;
extern int lwTexcoordLocation ;

extern int lwModVwMatrixLocation ;
extern int lwLineColorLocation ;

bool insideResetVert = false;
bool insideUserResetVert = false;
bool fakeRoiChange = false;
void Linework::SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData){
    _usettingsdata = usettingsData;
    if(!_usettingsdata->_stringSettings["ICONFOLDER"].empty())
        this->_iconFolder = _usettingsdata->_stringSettings["ICONFOLDER"];

//    if ( m.find("f") == m.end() ) {
//      // not found
//    } else {
//      // found
//    }
    if(!(_usettingsdata->_intSettings.find("STREETCOLORR") == _usettingsdata->_intSettings.end()))
    {
        int iRed = _usettingsdata->_intSettings["STREETCOLORR"];
        double dRed = static_cast<double>(iRed)/255.0;
        this->_streetColorRed = dRed;
    }
    else
        this->_streetColorRed = 1.0;

    if(!(_usettingsdata->_intSettings.find("STREETCOLORG") == _usettingsdata->_intSettings.end()))
    {
        int iGreen = _usettingsdata->_intSettings["STREETCOLORG"];
        double dGreen = static_cast<double>(iGreen)/255.0;
        this->_streetColorGreen = dGreen;
    }
    else
        this->_streetColorGreen = 1.0;

    if(!(_usettingsdata->_intSettings.find("STREETCOLORB") == _usettingsdata->_intSettings.end()))
    {
        int iBlue = _usettingsdata->_intSettings["STREETCOLORB"];
        double dBlue = static_cast<double>(iBlue)/255.0;
        this->_streetColorBlue = dBlue;
    }
    else
        this->_streetColorBlue = 1.0;

    if(!(_usettingsdata->_intSettings.find("STREETALPHA") == _usettingsdata->_intSettings.end()))
    {
        int iAlpha = _usettingsdata->_intSettings["STREETALPHA"];
        double dAlpha = static_cast<double>(iAlpha)/255.0;
        this->_streetColorAlpha = dAlpha;
    }
    else
        this->_streetColorAlpha = 1.0;

    if(!(_usettingsdata->_intSettings.find("STREETWIDTH") == _usettingsdata->_intSettings.end()))
    {
        int iWidth = _usettingsdata->_intSettings["STREETWIDTH"];
        //double dBlue = static_cast<double>(iBlue)/255.0;
        this->_lineWidth = static_cast<double>(iWidth);
    }
    else
        this->_lineWidth = 6.0;


//    if(!_usettingsdata->_stringSettings["STREETCOLORG"].empty())
//        this->_streetColorGreen = _usettingsdata->_stringSettings["STREETCOLORG"];
//    if(!_usettingsdata->_stringSettings["STREETCOLORB"].empty())
//        this->_streetColorBlue = _usettingsdata->_stringSettings["STREETCOLORB"];
}

void Linework::OnPendingRoiChange()
{

}


void Linework::OnRoiChange(double lowLat, double lowLon, double upperLat, double upperLon) {

}

void Linework::OnRoiChangeSwap()
{
    boost::mutex::scoped_lock scoped_lock(_mutexBackBuffColorSorted);
    if(_backBuffColorSortedPolylines.size() > 0)
        _colorSortedPolylines.clear();
    else
        std::cout<<"bypassed lineclear"<<std::endl;
    for(int i = _backBuffColorSortedPolylines.size() - 1 ; i >= 0 ; i--){
        _colorSortedPolylines.push_back(_backBuffColorSortedPolylines[i]);
        _backBuffColorSortedPolylines.pop_back();//remove from bbuff list, it's now ready to be rendered
    }
    _allocateBuffs = true;//we have a new set of street lines so we have to allocate the gpu for them
    _pendingSwap = false;//did a swap last, don't need to worry about resyncing
}

Linework::Linework():
    _camX(0.00),
    _camY(0.00),
    _camZ(0.00),
    _generateLineworkLists(false),
    _generateUserPolylineLists(false),
    _iconFolder(""),
    _signalSwap(false),
    _pendingSwap(false),
    _streetColorRed(1.0),
    _streetColorGreen(1.0),
    _streetColorBlue(1.0),
    _lineWidth(3.0),
    _allocateBuffs(false)


{
    _polylinesSortedByColor.reset(new RISStreetColoredPlineCollection );
    _backBuffPolylinesSortedByColor.reset(new RISStreetColoredPlineCollection );

    _userPolylinesSortedByColor.reset(new RISStreetColoredPlineCollection );
    _backBuffuserPolylinesSortedByColor.reset(new RISStreetColoredPlineCollection );
    _userPlinePlacemarkCollection.reset(new RISPlacemarkCollection );
    _pRoiMan.reset();
}

void Linework::Init(boost::shared_ptr<ROIMan> pRoiMan, QOpenGLShaderProgram *programRGBLinework){
    _pRoiMan = pRoiMan;
    _pRoiMan->AddRoiListener(this);
    _programRGBLinework = programRGBLinework;
}
Linework::~Linework(){
}

bool Linework::PendingSwapFromListener(){
    std::stringstream ss;
    ss<<"StreetVectorsView: returning pending swap = ";
    ss<<_pendingSwap;
    WriteToStatusLog(ss.str(), false, true);
    return _pendingSwap;
}


void Linework::AddPlacemarks(boost::shared_ptr<RISPlacemarkCollection> placemarkCollection, bool roiRefreshable)
{
    boost::mutex::scoped_lock scoped_lock(_mutexUnsortedPolylines);
    placemarkCollection->SetRoiRefreshable(roiRefreshable);
    for(int i = _unsortedPolylines.size() - 1 ; i >= 0 ; i--)
    {
        boost::shared_ptr<RISPlacemarkCollection> ptrPMCollection = _unsortedPolylines[i];
        if(ptrPMCollection->GetRoiRefreshable() == true)
        {
            //this is from the previous roi
            _unsortedPolylines.erase(_unsortedPolylines.begin() + i);
        }
    }
    _unsortedPolylines.push_back(placemarkCollection);
    _generateLineworkLists = true;
}

int Linework::enableStreetVectors(bool enable)
{
    for(int i = 0 ; i < _colorSortedPolylines.size() ; i++)
    {
        boost::shared_ptr<RISStreetColoredPlineCollection > ppline = _colorSortedPolylines[i];
        for(int j = 0 ; j < ppline->_coloredPlines.size() ; j++)
        {
            boost::shared_ptr<RISStreetColoredPline> colorCollection = ppline->_coloredPlines[j];
            for(int k = 0 ; k < colorCollection->_polyinesByColor.size() ; k++)
            {

                boost::shared_ptr<Kml::Vectors> pl = colorCollection->_polyinesByColor[k];
                KmlPathDetails kmld1 = *(pl->_kmlPathDetails.get());//*(path.get());
                //KmlPathDetails kmld2 = *(path.get());//*(pl->_kmlPathDetails.get());
                //if(kmld1 == kmld2){
                //
                if(pl->_kmlPathDetails->_filename.compare("") == 0)
                {
                    //plines with filename came from a kml file, not street vectors from server.
                    //its a street vector, hide is enable = false, show if enable = true
                    //f(enable)
                    //{
                        pl->_hide = !enable;
                    //}
//                    else
//                    {
//                        pl->_hide = true;
//                    }
                }
            }
        }
    }
    //delete from placemark collection, so it hides after next roi change
    for(int i = _unsortedPolylines.size() - 1 ; i >= 0 ; i--)
    {
        boost::shared_ptr<RISPlacemarkCollection> ptrPMCollection = _unsortedPolylines[i];
        //todo: fix
        {
            //this is from the previous roi
            ptrPMCollection->_hide = !enable;
        }
    }
}
 void Linework::ShowKMLLines(boost::shared_ptr<KmlPathDetails> path)
 {

     for(int i = 0 ; i < _colorSortedPolylines.size() ; i++)
     {
         boost::shared_ptr<RISStreetColoredPlineCollection > ppline = _colorSortedPolylines[i];
         for(int j = 0 ; j < ppline->_coloredPlines.size() ; j++)
         {
             boost::shared_ptr<RISStreetColoredPline> colorCollection = ppline->_coloredPlines[j];
             for(int k = 0 ; k < colorCollection->_polyinesByColor.size() ; k++)
             {

                 boost::shared_ptr<Kml::Vectors> pl = colorCollection->_polyinesByColor[k];
                 KmlPathDetails kmld1 = *(pl->_kmlPathDetails.get());//*(path.get());
                 KmlPathDetails kmld2 = *(path.get());//*(pl->_kmlPathDetails.get());
                 if(kmld1 == kmld2){
                     pl->_hide = false;
                 }
             }
         }
     }
     //delete from placemark collection, so it hides after next roi change
     for(int i = _unsortedPolylines.size() - 1 ; i >= 0 ; i--)
     {
         boost::shared_ptr<RISPlacemarkCollection> ptrPMCollection = _unsortedPolylines[i];
         //todo: fix
         {
             //this is from the previous roi
             ptrPMCollection->_hide = false;
         }
     }
 }

void Linework::HideKMLLines(boost::shared_ptr<KmlPathDetails> path)
{
    for(int i = 0 ; i < _colorSortedPolylines.size() ; i++)
    {
        boost::shared_ptr<RISStreetColoredPlineCollection > ppline = _colorSortedPolylines[i];
        //if(ppline->_hide == false)
        //{
        for(int j = 0 ; j < ppline->_coloredPlines.size() ; j++)
        {
                boost::shared_ptr<RISStreetColoredPline> colorCollection = ppline->_coloredPlines[j];
                for(int k = 0 ; k < colorCollection->_polyinesByColor.size() ; k++)
                {

                    boost::shared_ptr<Kml::Vectors> pl = colorCollection->_polyinesByColor[k];
                    KmlPathDetails kmld1 = *(pl->_kmlPathDetails.get());//*(path.get());
                    KmlPathDetails kmld2 = *(path.get());//*(pl->_kmlPathDetails.get());
                    if(kmld1 == kmld2){
                    //if(pl->_kmlPathDetails == path){
                        //hide it
                        pl->_hide = true;
                    }
                }
        }
    }
                    //////////////////////////////////////////////////////
//    for(int i = _colorSortedPolylines.size() -1 ; i >= 0 ; i--)
//    {
//        //hide from render vector/list so it hides immediately
//        boost::shared_ptr<RISStreetColoredPlineCollection> pl = _colorSortedPolylines[i];
//        //if(pl->_uid == uniqueID)
//        //todo: fix
//        if(false)
//        {
//            pl->_hide = true;
//            WriteToStatusLog("hiding kmz line", false, true);
//            //_colorSortedPolylines.erase(_colorSortedPolylines.begin() + i);
//        }

//    }

    //delete from placemark collection, so it hides after next roi change
    for(int i = _unsortedPolylines.size() - 1 ; i >= 0 ; i--)
    {
        boost::shared_ptr<RISPlacemarkCollection> ptrPMCollection = _unsortedPolylines[i];
        //if(ptrPMCollection->_uid == uniqueID)
        //todo: fix
        if(false)
        {
            //this is from the previous roi
            ptrPMCollection->_hide = true;
            //WriteToStatusLog("hiding kmz line again", false, true);
            //_unsortedPolylines.erase(_unsortedPolylines.begin() + i);
        }
    }
}

void Linework::DeleteKMLLines(boost::shared_ptr<KmlPathDetails> path)
{
    for(int i = 0 ; i < _colorSortedPolylines.size() ; i++)
    {
        boost::shared_ptr<RISStreetColoredPlineCollection > ppline = _colorSortedPolylines[i];
        //if(ppline->_hide == false)
        //{
        for(int j = 0 ; j < ppline->_coloredPlines.size() ; j++)
        {
            boost::shared_ptr<RISStreetColoredPline> colorCollection = ppline->_coloredPlines[j];
            for(int k = 0 ; k < colorCollection->_polyinesByColor.size() ; k++)
            {

                boost::shared_ptr<Kml::Vectors> pl = colorCollection->_polyinesByColor[k];
                KmlPathDetails kmld1 = *(path.get());
                KmlPathDetails kmld2 = *(pl->_kmlPathDetails.get());
                if(kmld1 == kmld2){
                //if(pl->_kmlPathDetails == path){
                    //hide it
                    //symGroup->_symbolGroup->erase (symGroup->_symbolGroup->begin() + j);
                    pl->_delete = true;
                    //colorCollection->_polyinesByColor.erase(colorCollection->_polyinesByColor.begin() + k);
                    break;
                }
            }
        }
    }

    ///////////////////////////////////

//    for(int i = _colorSortedPolylines.size() -1 ; i >= 0 ; i--)
//    {
//        //delete from render vector/lis
//        boost::shared_ptr<RISStreetColoredPlineCollection> pl = _colorSortedPolylines[i];
//        //if(pl->_uid == uniqueID)
//        if(false)//todo: fix
//        {
//            _colorSortedPolylines.erase(_colorSortedPolylines.begin() + i);
//        }

//    }

    //delete from placemark collection, so it won't reappear after next roi change
    for(int i = _unsortedPolylines.size() - 1 ; i >= 0 ; i--)
    {
        boost::shared_ptr<RISPlacemarkCollection> ptrPMCollection = _unsortedPolylines[i];
        //if(ptrPMCollection->_uid == uniqueID)
        if(false)//todo: fix
        {
            //this is from the previous roi
            _unsortedPolylines.erase(_unsortedPolylines.begin() + i);
        }
    }
    fakeRoiChange = true;
}

void Linework::AddUserPolylines(boost::shared_ptr<RISPlacemarkCollection> userPolylineCollection)
{
    boost::mutex::scoped_lock scoped_lock(_mutexUnsortedPolylines);
    userPolylineCollection->SetRoiRefreshable(false);
    _unsortedPolylines.push_back(userPolylineCollection);
    _generateLineworkLists = true;
    std::cout<<"pending swap="<<_pendingSwap<<std::endl;
    if(_pendingSwap == false)
        fakeRoiChange = true;

}

////sort out the plines sent in from kml engine by color, and place in _backBuffPolylinessortedbycolor
////then we will create vertices buffers for the gpu to hold on to for fast rendering
void Linework::SortToNewVectorsByColor(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > pmarks,
                                       boost::shared_ptr<RISStreetColoredPlineCollection>& sortTo,
                                       bool roiRefreshable, int uid, bool hide)
{
    sortTo.reset(new RISStreetColoredPlineCollection );
    sortTo->_roiRefreshable = roiRefreshable;
    sortTo->_uid = uid;
    sortTo->_hide = hide;
    for(int i = 0  ; i < pmarks.size() ; i++)
    {
        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > pm2 = pmarks[i];
        for(int k = 0 ; k < pm2->size() ; k++)
        {
            boost::shared_ptr<Kml::Vectors> nonSortLS = (*pm2)[k];
            bool foundMatch = false;

            for(int  j = 0 ; j < sortTo->_coloredPlines.size() ; j++){
                boost::shared_ptr<RISStreetColoredPline> colorCollection = sortTo->_coloredPlines[j];
                boost::shared_ptr<Kml::Vectors> sortedPline = colorCollection->_polyinesByColor[0];
                kmlbase::Color32 colorSorted = sortedPline->GetColor();
                kmlbase::Color32 colorNonSorted = nonSortLS->GetColor();
                if((colorSorted.get_red() == colorNonSorted.get_red())&&(colorSorted.get_green() == colorNonSorted.get_green())&&
                   (colorSorted.get_blue() == colorNonSorted.get_blue()) &&
                   (colorSorted.get_alpha() == colorNonSorted.get_alpha()))
                {
                    //these linestring colors match
                    //add to the current mutablearray
                    foundMatch = true;
                    std::vector<boost::shared_ptr<Kml::Point> > pts = nonSortLS->GetStreetVectors();
                    bool insideFP = false;
                        colorCollection->_polyinesByColor.push_back(nonSortLS);
                }
            }
            if(foundMatch == false){
                //need to add a new mutablearray for this color
                boost::shared_ptr<RISStreetColoredPline> newColoredCollection(new RISStreetColoredPline) ;
                newColoredCollection->_polyinesByColor.push_back(nonSortLS);
                sortTo->_coloredPlines.push_back(newColoredCollection);
            }
        }
    }
}


void Linework::resetVerticesBuffers()
{
    int converterUniqueId = -1;
    if(insideResetVert)
        return;
    insideResetVert = true;


    CoordinateConverter coordConverter = GetLatestCoordConverter();
    {
        boost::mutex::scoped_lock scoped_lock(_mutexUnsortedPolylines);
        boost::mutex::scoped_lock scoped_lock2(_mutexBackBuffColorSorted);
        _backBuffColorSortedPolylines.clear();
        for(int i = _unsortedPolylines.size() - 1 ; i >= 0 ; i--)
        {
            resetSingleVerticesBuffer(_unsortedPolylines[i]);
        }
    }
    //_latestUsedConverterID = coordConverter.GetUniqueID();
    if(!fakeRoiChange)
        _pendingSwap = true;
    insideResetVert = false;
}

void Linework::resetSingleVerticesBuffer(boost::shared_ptr<RISPlacemarkCollection> copyFrom)
{
    if(insideUserResetVert)
        return;
    insideUserResetVert = true;
    int converterUniqueId = -1;
    int lineVertIndex = 0;
    boost::shared_ptr<RISStreetColoredPline> colorCollection;
    {
        CoordinateConverter coordConverter;
        if(fakeRoiChange)
        {
            coordConverter = GetCoordConverterAtIndex(0);
        }
        else
        {
            coordConverter = GetLatestCoordConverter();
        }

        {
            //coordConverter = GetLatestCoordConverter();
            boost::shared_ptr<RISStreetColoredPlineCollection > sortTo;

            bool roirefresh = copyFrom->GetRoiRefreshable();
            int uid = copyFrom->_uid;
            bool hide = copyFrom->_hide;
            SortToNewVectorsByColor(copyFrom->_plines, sortTo, roirefresh, uid, hide);//clear _polylinesSortedByColor and add vertices

             _backBuffColorSortedPolylines.push_back(sortTo);
            for(int j = 0 ; j < sortTo->_coloredPlines.size() ; j++)
            {
                colorCollection = sortTo->_coloredPlines[j];
                int linestringCount = colorCollection->_polyinesByColor.size();
                int coordCount = 0;
                colorCollection->_lineStringIndices.clear();
                for(int i = 0 ; i < linestringCount ; i++)
                {
                    boost::shared_ptr<Kml::Vectors> pline= colorCollection->_polyinesByColor[i];
                    coordCount += pline->GetSize();//number of points
                    colorCollection->_lineStringIndices.push_back(coordCount);
                }
                colorCollection->_totalLineStringSize = coordCount;

                if(colorCollection->_lineVertices != 0x00)
                    delete[] colorCollection->_lineVertices;
                colorCollection->_lineVertices = new VertexDataLW[coordCount];
                lineVertIndex = 0;
                int sz = 0;
                    sz = colorCollection->_polyinesByColor.size();

                for(int i = 0; i < sz; i++)
                {
                    boost::shared_ptr<Kml::Vectors> pline = colorCollection->_polyinesByColor[i];
                    for(int j = 0 ; j < pline->GetSize() ; j++)
                    {
                        double vertx = 0.00;
                        double verty = 0.00;
                        double vertz = 0.00;
                        boost::shared_ptr<Kml::Point> pt = pline->GetStreetVectors()[j];
                        double lat = pt->GetLat();
                        double lon = pt->GetLon();
                        double elev = pt->GetAlt();

                        coordConverter.GeodeticToCartesian(lat, lon, elev, vertx, verty, vertz);
                        colorCollection->_lineVertices[lineVertIndex].position.setX((float)vertx);
                        colorCollection->_lineVertices[lineVertIndex].position.setY((float)verty);
                        colorCollection->_lineVertices[lineVertIndex].position.setZ((float)vertz);

                        //sets the color, for right now it's just a pixel point
                        colorCollection->_lineVertices[lineVertIndex].texCoord.setX(0.0);
                        colorCollection->_lineVertices[lineVertIndex].texCoord.setY(0.0);
                        lineVertIndex++;
                    }
                }
                colorCollection->_arraySize = lineVertIndex;
            }
        }
    }
    insideUserResetVert = false;
}

void Linework::triggerNewROI(){
    //we hit a new roi, so we need to recalc all the street vertices etc..
    boost::thread workerThread(&Linework::resetVerticesBuffers, this);
}
void Linework::render(double camX, double camY, double camZ, boost::shared_ptr<SViewData> viewData, double frameWidth,
                                double frameHeight, double displayWidth, double displayHeight, QOpenGLShaderProgram* program, double alpha)
{
    if(program == 0x00){
        return;//don't try to render unless the shader is active (we called init)
    }

    //double aspect = std::abs(frameWidth / frameHeight);
    _viewData = viewData;
    //_projectionMatrix.perspective(_viewData->dfov, aspect, 0.1f, 10000000.0f);
    _viewData = viewData;
    _camX = camX;
    _camY = camY;
    _camZ = camZ;

    if(_generateLineworkLists == true )
    {
        //todo: make this multithreaded again as soon as we find out what roi sync issue is
        //boost::thread workerThread(&Linework::resetVerticesBuffers, this);
        resetVerticesBuffers();
        _generateLineworkLists = false;
        if(fakeRoiChange == true)
        {

            boost::mutex::scoped_lock scoped_lock(_mutexBackBuffColorSorted);
            _colorSortedPolylines.clear();
            for(int i = _backBuffColorSortedPolylines.size() - 1 ; i >= 0 ; i--){
                _colorSortedPolylines.push_back(_backBuffColorSortedPolylines[i]);
                _backBuffColorSortedPolylines.pop_back();//remove from bbuff list, it's now ready to be rendered
            }
            _allocateBuffs = true;//we have a new set of street lines so we have to allocate the gpu for them


            //OnRoiChangeSwap();
            fakeRoiChange = false;
        }
    }
    QVector4D linecolor;

    for(int i = 0 ; i < _colorSortedPolylines.size() ; i++)
    {
        boost::shared_ptr<RISStreetColoredPlineCollection > ppline = _colorSortedPolylines[i];
        //if(ppline->_hide == false)
        //{
            for(int j = 0 ; j < ppline->_coloredPlines.size() ; j++)
            {
                boost::shared_ptr<RISStreetColoredPline> colorCollection = ppline->_coloredPlines[j];

                if(colorCollection->_lineVertices)
                {
                    if(_allocateBuffs){
                        colorCollection->_arrayBuf.create();
                        colorCollection->_arrayBuf.bind();
                        colorCollection->_arrayBuf.allocate(colorCollection->_lineVertices, colorCollection->_arraySize * sizeof(VertexDataLW));

                    }
                    else
                        colorCollection->_arrayBuf.bind();

                    boost::shared_ptr<Kml::Vectors> pl = colorCollection->_polyinesByColor[0];
                    //colorCollection->_polyinesByColor

//                    if(pl->_kmlPathDetails->_element.compare("Route A") == 0)
//                    {
//                        std::cout<<"found route a";
//                    }
                    bool ispolygon = pl->GetIsPolygon();
                    kmlbase::Color32 color = pl->GetColor();
                    QVector4D c;
                    if(ppline->_roiRefreshable == false)
                    {

                        int r = color.get_red() / 255;
                        int g = color.get_green() / 255;
                        int b = color.get_blue() / 255;

                        c.setX(r);
                        c.setY(g);
                        c.setZ(b);
                        if(ispolygon)
                        {
                            c.setX(1.0);
                            c.setY(0.0);
                            c.setZ(0.0);
                            c.setW(0.25);
                        }
                        else
                            c.setW(alpha);
                        //c.setW(alpha);
                        //program->setUniformValue("lineColor", c);
                    }
                    else
                    {
                        //QVector4D c;

                        int r = color.get_red();
                        c.setX(_streetColorRed);
                        c.setY(_streetColorGreen);
                        c.setZ(_streetColorBlue);
                        if(ispolygon)
                            c.setW(0.25);
                        else
                            c.setW(_streetColorAlpha);
                        //program->setUniformValue("lineColor", c);
                    }

                    //BL: don't do this each frame,use vbo
                    program->setUniformValue("lineColor", c);
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
                    ////////new end

                    int startIndices = 0;
                    int endIndices=0;

                    //////////////////////
                    //boost::shared_ptr<Kml::Vectors> pl = colorCollection->_polyinesByColor[k];
                    KmlPathDetails kmld1 = *(pl->_kmlPathDetails.get());//*(path.get());
                    //KmlPathDetails kmld2 = *(path.get());//*(pl->_kmlPathDetails.get());
                    //if(kmld1 == kmld2){
                    //if(pl->_kmlPathDetails == path){
                        //hide it

                    //    pl->_hide = false;
                    //}

                    ///////////////////////////
                    if(ppline->_hide == false)
                    {
                        for(int i = 0 ; i < colorCollection->_lineStringIndices.size() ; i++)
                        {

                            endIndices = colorCollection->_lineStringIndices[i];
                            GLint start = (GLint)startIndices;
                            GLsizei count = (GLint)(endIndices - startIndices);
                            if(ispolygon)
                            {
                                glLineWidth(4.0);
                                //glLineWidth(1.0);
                            }
                            else
                                //glLineWidth(1.0);
                                glLineWidth(_lineWidth);
                            //glLineWidth(6.0);
                            boost::shared_ptr<Kml::Vectors> pl2 = colorCollection->_polyinesByColor[i];
                            bool hideByPath = pl2->_hide;
                            bool deleteByPath = pl2->_delete;
                            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                            glEnable(GL_MULTISAMPLE);
                           // glBlendEquation(GL_MAX);
                            // glBlendFunc(GL_ONE,GL_ONE);
                            glEnable(GL_BLEND);

                            glEnable(GL_LINE_SMOOTH);
                            //glEnable(GL_POLYGON_SMOOTH);
                            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
                            //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

                            if(!hideByPath && !deleteByPath)
                            {
                                if(ispolygon)
                                {
                                    glDrawArrays(GL_LINE_LOOP, start, count);
                                }
                                else
                                {//BL: use gldrawelements, and use a vbo, cut down on the amount this gets
                                    //called to once, use an array with the indices to draw.
                                    glDrawArrays(GL_LINE_STRIP, start, count);
                                }
                                //glDrawArrays(GL_POLYGON, start, count);
                            }
                            else
                            {
                                //std::cout<<"hidden path";//remove
                            }

                            startIndices = endIndices;
                        }
                    }

                }
            }
       // }
    }
    glDisable(GL_LINE_SMOOTH);
    //glBlendEquation(GL_FUNC_ADD);
     //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    _allocateBuffs = false;
}
