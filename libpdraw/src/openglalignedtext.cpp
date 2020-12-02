#include "openglalignedtext.h"
#include <math.h>
#include "utilities.h"

///calculate length of linestring in x/y cartesian coordinates
double openglalignedtext::CalculateStreetVectorLength(boost::shared_ptr<Kml::Vectors> lineString){
    //using namespace Kml;
    int length = 0;
    //NSMutableArray* points = lineString.coordinateArray;
    std::vector<boost::shared_ptr<Kml::Point> > points = lineString->GetStreetVectors();// coordinateArray;
    int size = points.size();// count];

    if(size >= 2)
    {
        for(int i = 1 ; i < size ; i++)
        {
            boost::shared_ptr<Kml::Point> coord1 = points[i];
            boost::shared_ptr<Kml::Point> coord2 = points[i-1];// objectAtIndex:i-1];
            double x1, y1, z1 = 0.00;
            double x2, y2, z2 = 0.00;
            _coordConverter->GeodeticToCartesian( coord1->GetLat(), coord1->GetLon(), coord1->GetAlt(), x1, y1, z1);
            _coordConverter->GeodeticToCartesian( coord2->GetLat(), coord2->GetLon(), coord2->GetAlt(), x2, y2, z2);

            double deltaX = abs(x1 - x2);
            double deltaY = abs(y1 - y2);
            double segLength = sqrt((deltaX * deltaX) + (deltaY * deltaY));
            length += segLength;
        }
    }
    return length;
}


#include "utilities.h"
double openglalignedtext::FindRotationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double charLocation, double charsize,
                                               double& charx, double& chary, double& charalt)
{
    //take the vector of street segments, find out which line segment we are in, use the two endpoints to calculate a
    //rotation and return it, also calculate position of the character
    boost::shared_ptr<StreetSegment> currentSegment;
    currentSegment.reset();
    bool found = false;
    double rotation = 0.00;
    int count = segments.size();
    ///find which segment of the linestring the char in question is located in
    for(int i = 0 ; i < count &&  found == false ; i++)
    {
        boost::shared_ptr<StreetSegment> seg = segments[i];
        if(charLocation > seg->GetStartPoint() && charLocation < seg->GetEndPoint())
        {
            currentSegment = segments[i];
            found = true;
        }
    }
    ///if we found the current segment of the linestring this char is in
    if(currentSegment)
    {
        //we have the segment we are in, now use the start and end points to calculate the rotation

        double deltaX = currentSegment->GetX2() - currentSegment->GetX1();
        double deltaY = currentSegment->GetY2() - currentSegment->GetY1();

        //rotation = radiansToDegrees(atan2(deltaX, deltaY));
        rotation = radiansToDegrees(atan2(deltaY, deltaX));
        // how far along the segment line do we place the char
        double ratio = (charLocation - currentSegment->GetStartPoint()) / (currentSegment->GetEndPoint() - currentSegment->GetStartPoint());
        //get x, y location of where char should go along current segment
        charx = (( currentSegment->GetX2() - currentSegment->GetX1())* ratio) + currentSegment->GetX1();
        chary = (( currentSegment->GetY2() - currentSegment->GetY1())* ratio) + currentSegment->GetY1();

        //need to take charlocation (distance from beginning of segment to letter), and calculate
        //charx/y which is the x,y along the segment
        charalt = currentSegment->GetElevation();

        //TODO: PRORATE THIS BETWEEN START AND ENDPOINT OF SEGMENT
        double elevstart = _EML->GetElevation(currentSegment->GetLat1(), currentSegment->GetLon1());
        double elevend  = _EML->GetElevation(currentSegment->GetLat2(), currentSegment->GetLon2());
        double deltaElev = elevend - elevstart;
        double diff = ratio * deltaElev;
        charalt = elevstart + diff;
        //cout<<"start="<<elevstart<<" end ="<<elevend<<" elevchar="<<charalt<<endl;
    }
    else{
    }
    return rotation;
}

///create a vector of street segments, which are basically a set of lines with a start x,y and end x,y
/// but also gives it a start and endpoint for distance.  ie 2 street segments, 1 of them is 50 units long
/// the other is 100, the start and end of the first segment is 0 and 50, the start and end of the second
/// segment is 50 150
double openglalignedtext::CreateStreetSegmentVector(std::vector<boost::shared_ptr<Kml::Point> > points, std::vector<boost::shared_ptr<StreetSegment> >& streetSegments){
    double streetLength = 0.00;
    double endPoint = 0.0;
    for(int j = 0 ; j < points.size()-1 ; j++)
    {
        boost::shared_ptr<Kml::Point> point1 = points[j];
        double x1, y1, z1 = 0.00;
        double lat1 = point1->GetLat();
        double lon1 = point1->GetLon();
        double alt1 = point1->GetAlt();
        _coordConverter->GeodeticToCartesian( lat1, lon1, alt1, x1, y1, z1);

        point1->SetX(x1);
        point1->SetY(y1);
        point1->SetZ(z1);

        boost::shared_ptr<Kml::Point> point2 = points[j + 1];
        double x2, y2, z2 = 0.00;
        double lat2 = point2->GetLat();
        double lon2 = point2->GetLon();
        double alt2 = point2->GetAlt();
        _coordConverter->GeodeticToCartesian( lat2, lon2, alt2, x2, y2, z2);
        point2->SetX(x2);
        point2->SetY(y2);
        point2->SetZ(z2);

        boost::shared_ptr<StreetSegment> seg(new StreetSegment(x1, y1, x2, y2, lat1, lon1, lat2, lon2, alt1, endPoint));
        endPoint = seg->GetEndPoint();
        streetSegments.push_back(seg);
        streetLength = endPoint;
    }
    return streetLength;
}

/// find out which street segment contains the center location of the entire linestring of streetsegmens, then get the x, y location
/// of the actual center point of the linestring.
void openglalignedtext::findCenterLocationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double& x, double& y, double& z){
    int count = segments.size();
    boost::shared_ptr<StreetSegment> lastSegment = segments[count-1];
    double endpoint = lastSegment->GetEndPoint();
    double midway = endpoint / 2.0;
    bool foundLocation = false;
    for(int i = 0 ; i < count && foundLocation == false; i++)
    {
        boost::shared_ptr<StreetSegment> seg = segments[i];
        if(midway > seg->GetStartPoint() && midway < seg->GetEndPoint())
        {
            //this segment contains the midpoint
            double ratio = (midway - seg->GetStartPoint()) / (seg->GetEndPoint() - seg->GetStartPoint());
            x = (( seg->GetX2() - seg->GetX1())* ratio) + seg->GetX1();
            y = (( seg->GetY2() - seg->GetY1())* ratio) + seg->GetY1();
            z = seg->GetElevation();
            foundLocation = true;
        }
    }
}

///calculate center x,y coordinate of center of linestring so we can base that for our text/street label
void openglalignedtext::CalculatePlineCenterCoord(boost::shared_ptr<Kml::Vectors> pline, double& centerx, double& centery, double& centerz){
    double length = CalculatePlineLength(pline);
    double halfWay = length / 2.0;
    bool foundSegment = false;
    boost::shared_ptr<Kml::Point>  coord1 = pline->GetStreetVectors()[0];
    boost::shared_ptr<Kml::Point>  coord2;
    coord2.reset();

    //at this point, we found the segment the center point lies in, or the center doesn't lie in this
    //pline, which is an exception and shouldn't happen
    pline->GetStreetVectors();
    std::vector<boost::shared_ptr<StreetSegment> > streetSegments;//to be filled in from function
    double totalLength = CreateStreetSegmentVector(pline->GetStreetVectors(), streetSegments);
    findCenterLocationSegments(streetSegments, centerx, centery, centerz);
}

/// get distance in cartesian/x,y coordinate units between 2 points that have a lat/lon
double openglalignedtext::GetDist2Points(boost::shared_ptr<Kml::Point> coord, boost::shared_ptr<Kml::Point> coord2)
{
    double x1 = 0.00;
    double y1 = 0.00;
    double z1 = 0.00;

    double x2 = 0.00;
    double y2 = 0.00;
    double z2 = 0.00;
    double lat1 = coord->GetLat();//.latitude;
    double lon1 = coord->GetLon();//.longitude;
    double alt1 = coord->GetAlt();//.elevation;

    double lat2 = coord2->GetLat();//.latitude;
    double lon2 = coord2->GetLon();//.longitude;
    double alt2 = coord2->GetAlt();//.elevation;

    _coordConverter->GeodeticToCartesian( lat1, lon1, alt1, x1, y1, z1);
    _coordConverter->GeodeticToCartesian( lat2, lon2, alt2, x2, y2, z2);

    //calculate distance between 2 points
    double deltax = x1 - x2;
    double deltay = y1 - y2;
    return sqrt(deltax*deltax + deltay*deltay);
}

///calculate total length ofline string
double openglalignedtext::CalculatePlineLength(boost::shared_ptr<Kml::Vectors> pline)
{
    double totalPlineLength = 0.00;
    boost::shared_ptr<Kml::Point>  coord1 = pline->GetStreetVectors()[0];
    for(int plineIndex = 1 ; plineIndex < pline->GetStreetVectors().size() ; plineIndex++)
    {
        boost::shared_ptr<Kml::Point>  coord2 = pline->GetStreetVectors()[plineIndex];
        totalPlineLength += GetDist2Points(coord1, coord2);
        coord1 = coord2;//move up to the next  pline
    }
    return totalPlineLength;
}
void openglalignedtext::SetUserSettings()
{

    //scale 1
    if(!(_userSettingsData->_intSettings.find("STLABELNOSHOW") == _userSettingsData->_intSettings.end())){
        int inoshow = _userSettingsData->_intSettings["STLABELNOSHOW"];
        _streetLabelScaleNoShow = static_cast<double>(inoshow) / 1000.0;
    }
    else
        this->_streetLabelScaleNoShow =5.0;


    //scale 1
    if(!(_userSettingsData->_intSettings.find("STLABELSCALE1") == _userSettingsData->_intSettings.end())){
        int iscale = _userSettingsData->_intSettings["STLABELSCALE1"];
        _streetLabelScale1 = static_cast<double>(iscale) / 1000.0;
    }
    else
        this->_streetLabelScale1 =4.0;


    //cutoff1
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

    //cout<<"inside setusersettings"<<endl;
}

openglalignedtext::openglalignedtext(boost::shared_ptr<openglfont> font, double textScale, boost::shared_ptr<UserSettingsData> usettingsData,
                                     boost::shared_ptr<StatusLogger> logger, boost::shared_ptr<CoordinateConverter> coordConverter,
                                     boost::shared_ptr<ElevationManagerLocal> EML, std::string text,  boost::shared_ptr<Kml::Vectors> spVector):
_textScale(textScale),
//_numOfTextChars(0),
_vectorArrayOffset(0),
_streetLabelCutoff1(0.00),
_streetLabelCutoff2(0.00),
_streetLabelCutoff3(0.00),
_loadingComplete(false)
{
    _alignmentVector = spVector;
    _text = text;
    _EML = EML;
    _coordConverter = coordConverter;
    LoggableConfigurableObject::Init(usettingsData, logger, "openglalignedtext");
    _openglFont = font;
}

void openglalignedtext::GetLabelIndices(boost::shared_ptr<std::vector<unsigned int> > indices, boost::shared_ptr<SViewData> vehicleSixDof, double pixelSize)
{

    if(pixelSize > _streetLabelScaleNoShow)
        return;
    //take dotproduct between vehicle heading and streetline vector(startpoint -> endpoint)
    // divide by magnitude(veh heading) * magnitude(streetline vector)

    double vehLat1 = vehicleSixDof->dVehicleLat;
    double vehLon1 = vehicleSixDof->dVehicleLon;

    double vehLat2 = vehicleSixDof->dTargetLat;
    double vehLon2 = vehicleSixDof->dTargetLon;

    //lets create this into a vector Veh<deltalat, deltalon> is a unit vector pointing from vehicle in direction of heading/starepoint
    double deltaLatVeh = vehLat2 - vehLat1;
    double deltaLonVeh = vehLon2 - vehLon1;


    //now create a vector in direction of street linestring (take startpoint -> endpoint, disregard points in between for now
    std::vector<boost::shared_ptr<Kml::Point> > ls = _alignmentVector->GetStreetVectors();
    int size = ls.size();
    boost::shared_ptr<Kml::Point> p1 = ls[0];
    boost::shared_ptr<Kml::Point> p2 = ls[size - 1];

    //lets create this into a vector Line<deltalat, deltalon> is a vector pointing from startpoint to endpoint
    //of linestring
    double deltaLatLine = p2->GetLat() - p1->GetLat();
    double deltaLonLine = p2->GetLon() - p1->GetLon();

    //this tells us if the street label is facing us or we need to flip it
    double zcross = deltaLonVeh * deltaLatLine - deltaLatVeh * deltaLonLine;

    if(pixelSize > _streetLabelCutoff1)//cutoff 1 is the biggest number of cutoffs, meaning above this we are zoomed out
    {
        if(zcross < 0.00)
        {
            for(int i = 0 ; i < _forwardFacingIndicesL1.size() ; i++)
            {
                indices->push_back(_forwardFacingIndicesL1[i]);
            }
        }
        else
        {
            for(int i = 0 ; i < _reverseFacingIndicesL1.size() ; i++)
            {
                indices->push_back(_reverseFacingIndicesL1[i]);
            }
        }
    }
    else if(pixelSize < _streetLabelCutoff1 && pixelSize > _streetLabelCutoff2)
    {
        if(zcross < 0.00)
        {
            for(int i = 0 ; i < _forwardFacingIndicesL2.size() ; i++)
            {
                indices->push_back(_forwardFacingIndicesL2[i]);
            }
        }
        else
        {
            for(int i = 0 ; i < _reverseFacingIndicesL2.size() ; i++)
            {
                indices->push_back(_reverseFacingIndicesL2[i]);
            }
        }
    }
    else if(pixelSize < _streetLabelCutoff2 && pixelSize > _streetLabelCutoff3)
    {
        if(zcross < 0.00)
        {
            for(int i = 0 ; i < _forwardFacingIndicesL3.size() ; i++)
            {
                indices->push_back(_forwardFacingIndicesL3[i]);
            }
        }
        else
        {
            for(int i = 0 ; i < _reverseFacingIndicesL3.size() ; i++)
            {
                indices->push_back(_reverseFacingIndicesL3[i]);
            }
        }
    }
    else if(pixelSize > _streetLabelCutoff4)
    {
        if(zcross < 0.00)
        {
            for(int i = 0 ; i < _forwardFacingIndicesL4.size() ; i++)
            {
                indices->push_back(_forwardFacingIndicesL4[i]);
            }
        }
        else
        {
            for(int i = 0 ; i < _reverseFacingIndicesL4.size() ; i++)
            {
                indices->push_back(_reverseFacingIndicesL4[i]);
            }
        }
    }
    else
    {
        if(zcross < 0.00)
        {
            for(int i = 0 ; i < _forwardFacingIndicesL5.size() ; i++)
            {
                indices->push_back(_forwardFacingIndicesL5[i]);
            }
        }
        else
        {
            for(int i = 0 ; i < _reverseFacingIndicesL5.size() ; i++)
            {
                indices->push_back(_reverseFacingIndicesL5[i]);
            }
        }
    }


}

void openglalignedtext::AddLabelVerticesSingle(boost::shared_ptr<std::vector<float> > vecLabelVertices, std::vector<boost::shared_ptr<StreetSegment> > segments,
                                               std::vector<unsigned int>& indices, double streetLength, float scale)
{

    _vectorArrayOffset = vecLabelVertices->size() / 5;
    std::vector<boost::shared_ptr<Kml::Point> > ls = _alignmentVector->GetStreetVectors();

    //double generalScale = 1.0;//todo: fix
    double stringLength = CalculateStringLength(_text); //TODO: CALL NEW CLASS
    //return ;

    double textSize = scale * stringLength;
    double currentLocation = (streetLength - textSize) / 2;
    if(streetLength < textSize)
        return ;


    //use openglfont to grab spacing between text etc, and create an arraybuff
    //where each item has texture coords along with vertex coords.
    //int totalAdvance = CalculateTotalAdvance();


    // Center the text at the origin
    int fontWidth = _openglFont->GetFontWidth();
    int fontHeight = _openglFont->GetFontHeight();
    for(int i = 0 ; i < _text.size() ; i ++)
    {
        int index = i * 4;
        unsigned char cc = _text[i];

        double charX = 0.00;
        double charY = 0.00;
        double charAlt = 0.00;

        //float labelx = 0.00;
        //float labely = 0.00;
        //float labelz = 0.00;

        double rotation = FindRotationSegments(segments, currentLocation, 0.00, charX, charY, charAlt);
        double rotRadians = degreeToRadians(rotation);

        //todo: I don't think we use the normalized values, remove??
        double lowerNormalizedx = 0.0;
        double lowerNormalizedy = 0.0;
        double upperNormalizedx = 0.0;
        double upperNormalizedy = 0.0;

        int width = 0;
        int height = 0;
        int advance = 0;
        int originX = 0;
        int originY = 0;
        int charx = 0;
        int chary = 0;
        _openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);

        //        // Measure how wide the text is
        //        for (var i = 0; i < text.length; i++) {
        //          totalAdvance += font.characters[text[i]].advance;
        //        }

        //        // Center the text at the origin
        //        var x = -totalAdvance / 2;
        //        var y = font.size / 2;
        //        var vertices = [];

        //        // Add a quad for each character
        //        for (var i = 0; i < text.length; i++) {
        //          var c = font.characters[text[i]];

        //          // p0 --- p1
        //          // | \     |
        //          // |   \   |
        //          // |     \ |
        //          // p2 --- p3

        //origin x,y is the distance from center of char to leading edge?
        //charx chary is location right on transname line
        float x = static_cast<float>(charX);
        float y = (static_cast<float>(charY));

        //originX *= scale;
        //originY *= scale;
        //width *= scale;
        //height *= scale;
        float x0 = (x - originX * scale ) ;
        float y0 = ( y + originY * scale) ;
        float s0 = static_cast<float>(charx) / static_cast<float>(fontWidth);
        float t0 = static_cast<float>(fontHeight - chary) / static_cast<float>(fontHeight);

        float x1 = (x - (originX * scale) + (width * scale)) ;
        float y1 = (y + originY * scale) ;
        float s1 = static_cast<float>(charx + width) / static_cast<float>(fontWidth);
        float t1 = static_cast<float>(fontHeight - chary) / static_cast<float>(fontHeight);

        float x2 = (x - originX * scale) ;
        float y2 = (y + (originY - height) * scale) ;
        float s2 = static_cast<float>(charx) / static_cast<float>(fontWidth);
        float t2 = static_cast<float>(fontHeight - chary - height) / static_cast<float>(fontHeight);

        float x3 = (x - (originX * scale) + (width * scale)) ;
        float y3 = (y + (originY - height) * scale) ;
        float s3 = static_cast<float>(charx + width) / static_cast<float>(fontWidth);
        float t3 = static_cast<float>(fontHeight - chary - height) / static_cast<float>(fontHeight);


        //rotate the char
        float xdelta0 = x0 - x;
        float ydelta0 = y0 - (y);
        float newx0 = (xdelta0 * cos(rotRadians))  - (ydelta0 * sin(rotRadians))  + x;
        float newy0 = (xdelta0 * sin(rotRadians))  + (ydelta0 * cos(rotRadians))  + y;

        float xdelta1 = x1 - x;
        float ydelta1 = y1 - (y);
        float newx1 = (xdelta1 * cos(rotRadians))  - (ydelta1 * sin(rotRadians))  + x;
        float newy1 = (xdelta1 * sin(rotRadians))  + (ydelta1 * cos(rotRadians))  + y;

        float xdelta2 = x2 - x;
        float ydelta2 = y2 - (y);
        float newx2 = (xdelta2 * cos(rotRadians))  - (ydelta2 * sin(rotRadians))  + x;
        float newy2 = (xdelta2 * sin(rotRadians))  + (ydelta2 * cos(rotRadians))  + y;


        float xdelta3 = x3 - x;
        float ydelta3 = y3 - (y);
        float newx3 = (xdelta3 * cos(rotRadians))  - (ydelta3 * sin(rotRadians))  + x;
        float newy3 = (xdelta3 * sin(rotRadians))  + (ydelta3 * cos(rotRadians))  + y;
#if defined(__APPLE__)

        //in ios y needs to be flipped
        t0 = 1.0 - t0;
        t1 = 1.0 - t1;
        t2 = 1.0 - t2;
        t3 = 1.0 - t3;
#endif


        vecLabelVertices->push_back((float) newx0); //fill this in with the values given by font for w/h of letter
        vecLabelVertices->push_back((float) newy0);
        vecLabelVertices->push_back((float) charAlt);
        //we're just using a pixel since this is a solid color
        vecLabelVertices->push_back(s0);
        vecLabelVertices->push_back(t0);

        vecLabelVertices->push_back((float) newx1); //fill this in with the values given by font for w/h of letter
        vecLabelVertices->push_back((float) newy1);
        vecLabelVertices->push_back((float) charAlt);
        //we're just using a pixel since this is a solid color
        vecLabelVertices->push_back(s1);
        vecLabelVertices->push_back(t1);

        vecLabelVertices->push_back((float) newx2 ); //fill this in with the values given by font for w/h of letter
        vecLabelVertices->push_back((float) newy2 );
        vecLabelVertices->push_back((float) charAlt);
        //we're just using a pixel since this is a solid color
        vecLabelVertices->push_back(s2);
        vecLabelVertices->push_back(t2);


        vecLabelVertices->push_back((float) newx3 ); //fill this in with the values given by font for w/h of letter
        vecLabelVertices->push_back((float) newy3 );
        vecLabelVertices->push_back((float) charAlt);
        //we're just using a pixel since this is a solid color
        vecLabelVertices->push_back(s3);
        vecLabelVertices->push_back(t3);

        x += advance ;

        indices.push_back(2 + index + _vectorArrayOffset);
        indices.push_back(3 + index + _vectorArrayOffset);
        indices.push_back(0 + index + _vectorArrayOffset);
        indices.push_back(3 + index + _vectorArrayOffset);
        indices.push_back(1 + index + _vectorArrayOffset);
        indices.push_back(0 + index + _vectorArrayOffset);

        //double letterScale = 1.0;
        currentLocation += (scale * advance);

    }
}

//int openglalignedtext::CalculateTotalAdvance()
//{
//    int totalAdvance = 0;
//    for(int i = 0 ; i < _text.size() ; i ++)
//    {
//        double lowerNormalizedx = 0.0;
//        double lowerNormalizedy = 0.0;
//        double upperNormalizedx = 0.0;
//        double upperNormalizedy = 0.0;
//        int width = 0;
//        int height = 0;
//        int advance = 0;
//        int originX = 0;
//        int originY = 0;
//        int charx = 0;
//        int chary = 0;
//        unsigned char cc = _text[i];
//        _openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
//        totalAdvance += advance;
//    }
//    return totalAdvance;
//}

void openglalignedtext::AddLabelVertices(boost::shared_ptr<std::vector<float> > vecLabelVertices)
{

    std::vector<boost::shared_ptr<Kml::Point> > ls = _alignmentVector->GetStreetVectors();
    std::vector<boost::shared_ptr<StreetSegment> > streetSegments;
    double streetLength = CreateStreetSegmentVector(ls, streetSegments);

    AddLabelVerticesSingle(vecLabelVertices, streetSegments, _forwardFacingIndicesL1, streetLength, _streetLabelScale1);
    AddLabelVerticesSingle(vecLabelVertices, streetSegments, _forwardFacingIndicesL2, streetLength, _streetLabelScale2);
    AddLabelVerticesSingle(vecLabelVertices, streetSegments, _forwardFacingIndicesL3, streetLength, _streetLabelScale3);
    AddLabelVerticesSingle(vecLabelVertices, streetSegments, _forwardFacingIndicesL4, streetLength, _streetLabelScale4);
    AddLabelVerticesSingle(vecLabelVertices, streetSegments, _forwardFacingIndicesL5, streetLength, _streetLabelScale5);

    ReversePointList(ls);
    std::vector<boost::shared_ptr<StreetSegment> > streetSegmentsReverse;
    streetLength = CreateStreetSegmentVector(ls, streetSegmentsReverse);

    AddLabelVerticesSingle(vecLabelVertices, streetSegmentsReverse, _reverseFacingIndicesL1, streetLength, _streetLabelScale1);
    AddLabelVerticesSingle(vecLabelVertices, streetSegmentsReverse, _reverseFacingIndicesL2, streetLength, _streetLabelScale2);
    AddLabelVerticesSingle(vecLabelVertices, streetSegmentsReverse, _reverseFacingIndicesL3, streetLength, _streetLabelScale3);
    AddLabelVerticesSingle(vecLabelVertices, streetSegmentsReverse, _reverseFacingIndicesL4, streetLength, _streetLabelScale4);
    AddLabelVerticesSingle(vecLabelVertices, streetSegmentsReverse, _reverseFacingIndicesL5, streetLength, _streetLabelScale5);

    _loadingComplete = true;
}

//I dont' think we use this??  Vector tile contains the vbo and indices
//void openglalignedtext::RenderChar(double xpos, double ypos, double zpos, std::string character, double scale,
//                                   double rotation, bool tiltUp, double shiftX, double shiftY, double shiftZ, bool centerX,
//                                   double xrot, double yrot, double zrot, boost::shared_ptr<SViewData> viewData, double camx,
//                                   double camy, double camz, GLuint program, vmath::mat4 projectionMatrix,
//                                   int index, double xcharOffset)
//{

    //_program = program;
//    _arrayBuf.bind();//use model defined in initsymbol geometry
//
//    int offset = 0;
//    int vertexLocation = _program->attributeLocation("a_position");
//    _program->enableAttributeArray(vertexLocation);
//    _program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexDataLW));
//
//    // Offset for texture coordinate
//    offset += sizeof(QVector3D);
//
//    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
//    int texcoordLocation = _program->attributeLocation("a_texcoord");
//    _program->enableAttributeArray(texcoordLocation);
//    _program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexDataLW));
//
//
//    QMatrix4x4 modelViewMatrix;
//    modelViewMatrix.setToIdentity();
//
//    //camera rotations for icon
//    modelViewMatrix.rotate(viewData->dCameraRoll, 0.0f, 0.0f, 1.0f);
//
//    modelViewMatrix.rotate(-viewData->dCameraPitch, 1.0f, 0.0f, 0.0f);
//    modelViewMatrix.rotate(viewData->dCameraHeading, 0.0f, 1.0f, 0.0f);
//
//    //rotate so that z is down
//    modelViewMatrix.rotate( -90, 1.0f, 0.0f, 0.0f);
//    modelViewMatrix.translate(-camx, -camy, -camz);
//
//    modelViewMatrix.translate( xpos, ypos, zpos );
//    modelViewMatrix.rotate(180.0, 1.0, 0.0,0.0);
//    modelViewMatrix.rotate( rotation  , 0.0, 0.0, 1.0);
//    modelViewMatrix.rotate(zrot  , 0.0f, 0.0f, 1.0f);
//    modelViewMatrix.translate(xcharOffset, 0.00, 0.00);
//
//    //make icon/text flip back to aim up/down towards camera if in the air
//    //////////
//    static double testScale = 100.00;
//    if(testScale > 1.0)
//    {
//        testScale -= 0.1;
//    }
//    modelViewMatrix.scale(scale, scale, scale * 1.5);
//    modelViewMatrix.rotate(90.0, 1.0f, 0.0f, 0.0f);
//    modelViewMatrix.translate( shiftX, 0.0, 0.0 );
//    _program->setUniformValue("mvp_matrix", projectionMatrix * modelViewMatrix);
//    _program->setUniformValue("texture", 0);
//
//    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_BLEND);
//
//    glDrawArrays(GL_TRIANGLES, index  * 6, 6 );
//}

#include <algorithm>
void openglalignedtext::ReversePointList(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices)
{
    std::reverse(lineVertices.begin(),lineVertices.end());
}
void openglalignedtext::AlignStreetVectorOrientation(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices)
{
    if(lineVertices.size() > 1)
    {
        boost::shared_ptr<Kml::Point> ptFirst = lineVertices[0];
        double ptFirstLat = ptFirst->GetLat();
        double ptFirstLon = ptFirst->GetLon();

        boost::shared_ptr<Kml::Point> ptLast = lineVertices[lineVertices.size() - 1];
        double ptLastLat = ptLast->GetLat();
        double ptLastLon = ptLast->GetLon();

        double vehLat = _viewData->dVehicleLat;
        double vehLon = _viewData->dVehicleLon;
        double deltaLatFirstPt = ptFirstLat - vehLat;
        double deltaLonFirstPt = ptFirstLon - vehLon;

        double deltaLatLastPt = ptLastLat - vehLat;
        double deltaLonLastPt = ptLastLon - vehLon;


        double thetaFirstPt = atan(deltaLatFirstPt/deltaLonFirstPt);
        double headingFirstPt = 0.00;
        if(ptFirstLon > vehLon)
        {
            headingFirstPt = 90.0 - thetaFirstPt;
        }
        else
        {
            headingFirstPt = 180 + 90.0 - thetaFirstPt;
        }

        double thetaLastPt = atan(deltaLatLastPt / deltaLonLastPt);
        double headingLastPt = 0.00;
        if(ptLastLon > vehLon)
        {
            headingLastPt = 90.0 - thetaLastPt;
        }
        else
        {
            headingLastPt = 180 + 90.0 - thetaLastPt;
        }
        double deltaPtHeadings = headingLastPt - headingFirstPt;
        if(deltaPtHeadings < 0.00)
        {
            deltaPtHeadings *= -1.0;//make heading diff positive
        }

        if(headingLastPt > headingFirstPt && deltaPtHeadings < 180.0)
        {
            //no need to rearrange point list, points will be calculated first to last left to right
        }
        else if(headingLastPt < headingFirstPt && deltaPtHeadings < 180.0)
        {
            //reverse
            ReversePointList(lineVertices);
        }
        else if(headingLastPt > headingFirstPt && deltaPtHeadings > 180.0)
        {
            //reverse the point list order. points are currently going last to first when going left to right
            ReversePointList(lineVertices);
        }
        else if(headingLastPt < headingFirstPt && deltaPtHeadings > 180.0)
        {
            //do nothing, list already in the right order left->right = first to last
        }
    }
}



double openglalignedtext::CalculateSymbolScale3DCamx(double camx, double camy, double camz, double symbolx,
                                                     double symboly, double symbolz, double fov, double pixelHeight)
{
    double vx = camx - symbolx;
    double vy = camy - symboly;
    double vz = camz - symbolz;

    double d = sqrt(pow(vx,2)+pow(vy,2)+pow(vz,2));
    double fovRadians = fov * (3.14 / 180.0);//convert to radians for tan function
    double fovRadiansHalf = fovRadians /2;
    double dgdscale = d * tan(fovRadiansHalf);
    return dgdscale ;//STREETLABELSCALE;
}
double openglalignedtext::CalculateStringLength(std::string text)
{

    //_openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
    //totalAdvance += advance;


    //const char* c = text.c_str();
    int size = text.size();
    double stringLength = 0.00;
    for(int i = 0 ; i < size ; i++)
    {
        // bool space = false;

        double lowerNormalizedx = 0.0;
        double lowerNormalizedy = 0.0;
        double upperNormalizedx = 0.0;
        double upperNormalizedy = 0.0;
        int width = 0;
        int height = 0;
        int advance = 0;
        int originX = 0;
        int originY = 0;
        int charx = 0;
        int chary = 0;
        char letter = text[i];
        _openglFont->getLetterTextureDimensions(letter, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
        stringLength += advance;
    }
    return stringLength;
}

//OBSOLETE!!
//void openglalignedtext::render(QOpenGLShaderProgram* program, boost::shared_ptr<SViewData> viewData, QMatrix4x4 projectionMatrix)
//{
//    //todo: send in a ptr to a vector, and viewdata etc.  figure out which version of text we want to render
//    // then add the indices to the ptrvector so that vectortile can just call gldrawelements once which
//    // will take care of rendering all the street text for streets in said vector tile.
//    double camx = 0.00;
//    double camy = 0.00;
//    double camz = 0.00;
//
//    _coordConverter->GeodeticToCartesian(viewData->dVehicleLat, viewData->dVehicleLon, viewData->dVehicleAltitude, camx, camy, camz);
//    _viewData = viewData;
//    //todo: we will now have to setup a mvp for each letter, then drawarray each letter, because as we zoom in out or move around the scene
//    // and try align text to the pathway, text size needs to be consistent.  ie if we zoom in at a street curve with a label, the line/curve
//    // will get bigger, but text size needs to stay consistent
//    //  we need to keep a consistent text size/scale as we move around the scene/zoom in/out
//    //  ie, if we are drawing text around a curve and zoom in, the street line will get larger
//    //cout<<"rendering aligned text"<<endl;
//    //set arraybuff current
//    //call openglfont with bindfonttexture
//    _openglFont->bindFont();//ie bind font.png texture
//    _arrayBuf.bind();
//
//    int offset = 0;
//    int vertexLocation = program->attributeLocation("a_position");
//    program->enableAttributeArray(vertexLocation);
//    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexDataLW));
//
//    // Offset for texture coordinate
//    offset += sizeof(QVector3D);
//
//    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
//    int texcoordLocation = program->attributeLocation("a_texcoord");
//    program->enableAttributeArray(texcoordLocation);
//    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexDataLW));
//
//    //////////////////////////new render code letter by letter align to pline
//    // SHIELDTYPES shieldType = NONE;
//    //for(int i = 0 ; i < _text.size() ; i++)
//    //{
//    //shieldType = NONE;
//    boost::shared_ptr<Kml::Vectors> pline = _alignmentVector;// objectAtIndex:i];
//    std::vector<boost::shared_ptr<Kml::Point> > ls = pline->GetStreetVectors();//lineString;
//
//    //take 1st point and last point and compare.  if 1st point has less of heading in relation
//    //to vehicle than last point, then we don't need to reverse order. else we do need to reverse
//    //the order of points
//    AlignStreetVectorOrientation(ls);
//
//    //double endPoint = 0.0;
//    std::vector<boost::shared_ptr<StreetSegment> > streetSegments;
//    double streetLength = CreateStreetSegmentVector(ls, streetSegments);
//
//    //now we have to figure out the size of each letter of text, and text letter count
//    //find full length of text, length of street vector(whole segment) .
//    //take vector length - text length divide by 2.  this gives us the start location
//    //from the beginning of street to place first letter.  Create a new function
//    //findrotation(streetsegments, textloc, charwidth) which will return the rotation needed
//    //for that particular letter, then incrent textloc, and call again.  repeat until all letters
//    //complete.  The whole while printing out the char at given rotation.
//    //shieldType = GetShieldType(pline);
//    //std::string name = pline->GetLabel();
//    boost::shared_ptr<StreetSegment> streetSeg = streetSegments[0];
//    double x1 = streetSeg->GetX1();//x1;
//    double y1 = streetSeg->GetY1();//y1;
//    double z1 = streetSeg->GetElevation();//elevation;
//
//    double generalScale = 1.0;//todo: fix CalculateSymbolScale3DCamx(camx, camy, camz,
//    // x1, y1, z1, viewData->dfov, 1.0);
//    double stringLength = CalculateStringLength(_text); //TODO: CALL NEW CLASS
//    double textSize = generalScale * stringLength;
//    double currentLocation = (streetLength - textSize) / 2;
//    int strLength = _text.size();// length];
//    //if(shieldType == NONE)
//    //{
//    // no shield just the label
//    for(int j = 0 ; j < strLength ; j++)
//    {
//        char c[2];
//        c[0] = _text[j];
//        c[1] = '\0';
//        std::string strChar(c);
//        double charX = 0.00;
//        double charY = 0.00;
//        double charAlt = 0.00;
//
//        double labelx = 0.00;
//        double labely = 0.00;
//        double labelz = 0.00;
//
//        double rotation = FindRotationSegments(streetSegments, currentLocation, 0.00, charX, charY, charAlt);
//        double letterScale = 1.0;//todo: fix CalculateSymbolScale3DCamx(camx, camy, camz,
//        //           charX, charY, charAlt, viewData->dfov, 1.0);
//
//
//
//        //            void openglalignedtext::RenderChar(double xpos, double ypos, double zpos, std::string character, double scale,
//        //                                double rotation, bool tiltUp, double shiftX, double shiftY, double shiftZ, bool centerX, double xrot, double yrot,
//        //                                double zrot,
//        //                                boost::shared_ptr<SViewData> viewData, double camx, double camy, double camz,
//        //                                               QOpenGLShaderProgram* program, QMatrix4x4 projectionMatrix, int index, double xcharOffset)
//
//        //TODO: ADD OUR NEW RENDER ROUTINE,
//        RenderChar(charX, charY, charAlt, strChar, letterScale ,
//                   rotation-90.0, false, 0.00, 0.00, 0.00, false, 0.00, 0.00,
//                   0.00, viewData, camx, camy, camz,
//                   program, projectionMatrix, j, 0.0/*currentLocation*/);
//
//
//        int charVal = _text[j];
//
//        double lowerNormalizedx = 0.0;
//        double lowerNormalizedy = 0.0;
//        double upperNormalizedx = 0.0;
//        double upperNormalizedy = 0.0;
//        int width = 0;
//        int height = 0;
//        int advance = 0;
//        int originX = 0;
//        int originY = 0;
//        int charx = 0;
//        int chary = 0;
//        _openglFont->getLetterTextureDimensions(_text[j], lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
//        float charWidth = width;
//
//        currentLocation += (letterScale * charWidth);
//    }
//
//
//    //////////////////////////end new render code letter by letter align to pline
//    //todo: now we need to create an mvp and set that uniform in the shader for each letter so we can scale dynamically
//    //the text
//    // glScalef(10.0, 10.0, 10.0);
//    //glDrawArrays(GL_TRIANGLES, 0, 6 * _numOfTextChars);
//
//    //int offset = 0;
//}

void openglalignedtext::setJustification(int justification)
{

}

void openglalignedtext::setBasePoint(int x, int y)
{

}


//#include "openglalignedtext.h"


/////calculate length of linestring in x/y cartesian coordinates
//double openglalignedtext::CalculateStreetVectorLength(boost::shared_ptr<Kml::Vectors> lineString){
//    //using namespace Kml;
//    int length = 0;
//    //NSMutableArray* points = lineString.coordinateArray;
//    std::vector<boost::shared_ptr<Kml::Point> > points = lineString->GetStreetVectors();// coordinateArray;
//    int size = points.size();// count];

//    if(size >= 2)
//    {
//        for(int i = 1 ; i < size ; i++)
//        {
//            boost::shared_ptr<Kml::Point> coord1 = points[i];
//            boost::shared_ptr<Kml::Point> coord2 = points[i-1];// objectAtIndex:i-1];
//            double x1, y1, z1 = 0.00;
//            double x2, y2, z2 = 0.00;
//            _coordConverter->GeodeticToCartesian( coord1->GetLat(), coord1->GetLon(), coord1->GetAlt(), x1, y1, z1);
//            _coordConverter->GeodeticToCartesian( coord2->GetLat(), coord2->GetLon(), coord2->GetAlt(), x2, y2, z2);

//            double deltaX = abs(x1 - x2);
//            double deltaY = abs(y1 - y2);
//            double segLength = sqrt((deltaX * deltaX) + (deltaY * deltaY));
//            length += segLength;
//        }
//    }
//    return length;
//}


//#include "utilities.h"
//double openglalignedtext::FindRotationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double charLocation, double charsize,
//                            double& charx, double& chary, double& charalt)
//{
//    //take the vector of street segments, find out which line segment we are in, use the two endpoints to calculate a
//    //rotation and return it, also calculate position of the character
//    boost::shared_ptr<StreetSegment> currentSegment;
//    currentSegment.reset();
//    bool found = false;
//    double rotation = 0.00;
//    int count = segments.size();
//    ///find which segment of the linestring the char in question is located in
//    for(int i = 0 ; i < count &&  found == false ; i++)
//    {
//        boost::shared_ptr<StreetSegment> seg = segments[i];
//        if(charLocation > seg->GetStartPoint() && charLocation < seg->GetEndPoint())
//        {
//            currentSegment = segments[i];
//            found = true;
//        }
//    }
//    ///if we found the current segment of the linestring this char is in
//    if(currentSegment)
//    {
//        //we have the segment we are in, now use the start and end points to calculate the rotation

//        double deltaX = currentSegment->GetX2() - currentSegment->GetX1();
//        double deltaY = currentSegment->GetY2() - currentSegment->GetY1();

//        //rotation = radiansToDegrees(atan2(deltaX, deltaY));
//        rotation = radiansToDegrees(atan2(deltaY, deltaX));
//        // how far along the segment line do we place the char
//        double ratio = (charLocation - currentSegment->GetStartPoint()) / (currentSegment->GetEndPoint() - currentSegment->GetStartPoint());
//        //get x, y location of where char should go along current segment
//        charx = (( currentSegment->GetX2() - currentSegment->GetX1())* ratio) + currentSegment->GetX1();
//        chary = (( currentSegment->GetY2() - currentSegment->GetY1())* ratio) + currentSegment->GetY1();

//        //need to take charlocation (distance from beginning of segment to letter), and calculate
//        //charx/y which is the x,y along the segment
//        charalt = currentSegment->GetElevation();

//        //TODO: PRORATE THIS BETWEEN START AND ENDPOINT OF SEGMENT
//        double elevstart = _EML->GetElevation(currentSegment->GetLat1(), currentSegment->GetLon1());
//        double elevend  = _EML->GetElevation(currentSegment->GetLat2(), currentSegment->GetLon2());
//        double deltaElev = elevend - elevstart;
//        double diff = ratio * deltaElev;
//        charalt = elevstart + diff;
//        //cout<<"start="<<elevstart<<" end ="<<elevend<<" elevchar="<<charalt<<endl;
//    }
//    else{
//    }
//    return rotation;
//}

/////create a vector of street segments, which are basically a set of lines with a start x,y and end x,y
///// but also gives it a start and endpoint for distance.  ie 2 street segments, 1 of them is 50 units long
///// the other is 100, the start and end of the first segment is 0 and 50, the start and end of the second
///// segment is 50 150
//double openglalignedtext::CreateStreetSegmentVector(std::vector<boost::shared_ptr<Kml::Point> > points, std::vector<boost::shared_ptr<StreetSegment> >& streetSegments){
//    double streetLength = 0.00;
//    double endPoint = 0.0;
//    for(int j = 0 ; j < points.size()-1 ; j++)
//    {
//        boost::shared_ptr<Kml::Point> point1 = points[j];
//        double x1, y1, z1 = 0.00;
//        double lat1 = point1->GetLat();
//        double lon1 = point1->GetLon();
//        double alt1 = point1->GetAlt();
//        _coordConverter->GeodeticToCartesian( lat1, lon1, alt1, x1, y1, z1);

//        point1->SetX(x1);
//        point1->SetY(y1);
//        point1->SetZ(z1);

//        boost::shared_ptr<Kml::Point> point2 = points[j + 1];
//        double x2, y2, z2 = 0.00;
//        double lat2 = point2->GetLat();
//        double lon2 = point2->GetLon();
//        double alt2 = point2->GetAlt();
//        _coordConverter->GeodeticToCartesian( lat2, lon2, alt2, x2, y2, z2);
//        point2->SetX(x2);
//        point2->SetY(y2);
//        point2->SetZ(z2);

//        boost::shared_ptr<StreetSegment> seg(new StreetSegment(x1, y1, x2, y2, lat1, lon1, lat2, lon2, alt1, endPoint));
//        endPoint = seg->GetEndPoint();
//        streetSegments.push_back(seg);
//        streetLength = endPoint;
//    }
//    return streetLength;
//}

///// find out which street segment contains the center location of the entire linestring of streetsegmens, then get the x, y location
///// of the actual center point of the linestring.
//void openglalignedtext::findCenterLocationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double& x, double& y, double& z){
//    int count = segments.size();
//    boost::shared_ptr<StreetSegment> lastSegment = segments[count-1];
//    double endpoint = lastSegment->GetEndPoint();
//    double midway = endpoint / 2.0;
//    bool foundLocation = false;
//    for(int i = 0 ; i < count && foundLocation == false; i++)
//    {
//        boost::shared_ptr<StreetSegment> seg = segments[i];
//        if(midway > seg->GetStartPoint() && midway < seg->GetEndPoint())
//        {
//            //this segment contains the midpoint
//            double ratio = (midway - seg->GetStartPoint()) / (seg->GetEndPoint() - seg->GetStartPoint());
//            x = (( seg->GetX2() - seg->GetX1())* ratio) + seg->GetX1();
//            y = (( seg->GetY2() - seg->GetY1())* ratio) + seg->GetY1();
//            z = seg->GetElevation();
//            foundLocation = true;
//        }
//    }
//}

/////calculate center x,y coordinate of center of linestring so we can base that for our text/street label
//void openglalignedtext::CalculatePlineCenterCoord(boost::shared_ptr<Kml::Vectors> pline, double& centerx, double& centery, double& centerz){
//    double length = CalculatePlineLength(pline);
//    double halfWay = length / 2.0;
//    bool foundSegment = false;
//    boost::shared_ptr<Kml::Point>  coord1 = pline->GetStreetVectors()[0];
//    boost::shared_ptr<Kml::Point>  coord2;
//    coord2.reset();

//    //at this point, we found the segment the center point lies in, or the center doesn't lie in this
//    //pline, which is an exception and shouldn't happen
//    pline->GetStreetVectors();
//    std::vector<boost::shared_ptr<StreetSegment> > streetSegments;//to be filled in from function
//    double totalLength = CreateStreetSegmentVector(pline->GetStreetVectors(), streetSegments);
//    findCenterLocationSegments(streetSegments, centerx, centery, centerz);
//}

///// get distance in cartesian/x,y coordinate units between 2 points that have a lat/lon
//double openglalignedtext::GetDist2Points(boost::shared_ptr<Kml::Point> coord, boost::shared_ptr<Kml::Point> coord2)
//{
//    double x1 = 0.00;
//    double y1 = 0.00;
//    double z1 = 0.00;

//    double x2 = 0.00;
//    double y2 = 0.00;
//    double z2 = 0.00;
//    double lat1 = coord->GetLat();//.latitude;
//    double lon1 = coord->GetLon();//.longitude;
//    double alt1 = coord->GetAlt();//.elevation;

//    double lat2 = coord2->GetLat();//.latitude;
//    double lon2 = coord2->GetLon();//.longitude;
//    double alt2 = coord2->GetAlt();//.elevation;

//    _coordConverter->GeodeticToCartesian( lat1, lon1, alt1, x1, y1, z1);
//    _coordConverter->GeodeticToCartesian( lat2, lon2, alt2, x2, y2, z2);

//    //calculate distance between 2 points
//    double deltax = x1 - x2;
//    double deltay = y1 - y2;
//    return sqrt(deltax*deltax + deltay*deltay);
//}

/////calculate total length ofline string
//double openglalignedtext::CalculatePlineLength(boost::shared_ptr<Kml::Vectors> pline)
//{
//    double totalPlineLength = 0.00;
//    boost::shared_ptr<Kml::Point>  coord1 = pline->GetStreetVectors()[0];
//    for(int plineIndex = 1 ; plineIndex < pline->GetStreetVectors().size() ; plineIndex++)
//    {
//        boost::shared_ptr<Kml::Point>  coord2 = pline->GetStreetVectors()[plineIndex];
//        totalPlineLength += GetDist2Points(coord1, coord2);
//        coord1 = coord2;//move up to the next  pline
//    }
//}
//void openglalignedtext::SetUserSettings()
//{

//    //scale 1
//    if(!(_userSettingsData->_intSettings.find("STLABELNOSHOW") == _userSettingsData->_intSettings.end())){
//        int inoshow = _userSettingsData->_intSettings["STLABELNOSHOW"];
//        _streetLabelScaleNoShow = static_cast<double>(inoshow) / 1000.0;
//    }
//    else
//        this->_streetLabelScaleNoShow =5.0;


//    //scale 1
//    if(!(_userSettingsData->_intSettings.find("STLABELSCALE1") == _userSettingsData->_intSettings.end())){
//        int iscale = _userSettingsData->_intSettings["STLABELSCALE1"];
//        _streetLabelScale1 = static_cast<double>(iscale) / 1000.0;
//    }
//    else
//        this->_streetLabelScale1 =4.0;


//    //cutoff1
//    if(!(_userSettingsData->_intSettings.find("STLABELCUTOFF1") == _userSettingsData->_intSettings.end())){
//        int icutoff1 = _userSettingsData->_intSettings["STLABELCUTOFF1"];
//        _streetLabelCutoff1 = static_cast<double>(icutoff1) / 1000.0;
//    }
//    else
//        this->_streetLabelCutoff1 =11.0;

//    //scale2
//    if(!(_userSettingsData->_intSettings.find("STLABELSCALE2") == _userSettingsData->_intSettings.end())){
//        int iscale = _userSettingsData->_intSettings["STLABELSCALE2"];
//        _streetLabelScale2 = static_cast<double>(iscale) / 1000.0;
//    }
//    else
//        this->_streetLabelScale1 =2.0;


//    //cutoff2
//    if(!(_userSettingsData->_intSettings.find("STLABELCUTOFF2") == _userSettingsData->_intSettings.end())){
//        int icutoff2 = _userSettingsData->_intSettings["STLABELCUTOFF2"];
//        _streetLabelCutoff2 = static_cast<double>(icutoff2) / 1000.0;
//    }
//    else
//        this->_streetLabelCutoff2 = 3.7;

//    //scale3
//    if(!(_userSettingsData->_intSettings.find("STLABELSCALE3") == _userSettingsData->_intSettings.end())){
//        int iscale = _userSettingsData->_intSettings["STLABELSCALE3"];
//        _streetLabelScale3 = static_cast<double>(iscale) / 1000.0;
//    }
//    else
//        this->_streetLabelScale3 =2.0;


//    //cutoff 3
//    if(!(_userSettingsData->_intSettings.find("STLABELCUTOFF3") == _userSettingsData->_intSettings.end())){
//        int icutoff3 = _userSettingsData->_intSettings["STLABELCUTOFF3"];
//        _streetLabelCutoff3 = static_cast<double>(icutoff3) / 1000.0;
//    }
//    else
//        this->_streetLabelCutoff3 = 1.3;

//    //scale4
//    if(!(_userSettingsData->_intSettings.find("STLABELSCALE4") == _userSettingsData->_intSettings.end())){
//        int iscale = _userSettingsData->_intSettings["STLABELSCALE4"];
//        _streetLabelScale4 = static_cast<double>(iscale) / 1000.0;
//    }
//    else
//        this->_streetLabelScale4 =1.0;


//    //cutoff 4
//    if(!(_userSettingsData->_intSettings.find("STLABELCUTOFF4") == _userSettingsData->_intSettings.end())){
//        int icutoff = _userSettingsData->_intSettings["STLABELCUTOFF4"];
//        _streetLabelCutoff4 = static_cast<double>(icutoff) / 1000.0;
//    }
//    else
//        this->_streetLabelCutoff4 = 1.3;

//    //scale5
//    if(!(_userSettingsData->_intSettings.find("STLABELSCALE5") == _userSettingsData->_intSettings.end())){
//        int iscale = _userSettingsData->_intSettings["STLABELSCALE5"];
//        _streetLabelScale5 = static_cast<double>(iscale) / 1000.0;
//    }
//    else
//        this->_streetLabelScale5 =0.75;

//    //cout<<"inside setusersettings"<<endl;
//}

//openglalignedtext::openglalignedtext(boost::shared_ptr<openglfont> font, double textScale, boost::shared_ptr<UserSettingsData> usettingsData,
//                                     boost::shared_ptr<StatusLogger> logger, boost::shared_ptr<CoordinateConverter> coordConverter,
//                                     boost::shared_ptr<ElevationManagerLocal> EML, std::string text,  boost::shared_ptr<Kml::Vectors> spVector):
//    _textScale(textScale),
//    //_numOfTextChars(0),
//    _vectorArrayOffset(0),
//    _streetLabelCutoff1(0.00),
//    _streetLabelCutoff2(0.00),
//    _streetLabelCutoff3(0.00),
//    _loadingComplete(false)
//{
//    _alignmentVector = spVector;
//    _text = text;
//    _EML = EML;
//    _coordConverter = coordConverter;
//    LoggableConfigurableObject::Init(usettingsData, logger, "openglalignedtext");
//    _openglFont = font;
//}

//void openglalignedtext::GetLabelIndices(boost::shared_ptr<std::vector<unsigned int> > indices, boost::shared_ptr<SViewData> vehicleSixDof, double pixelSize)
//{

//    if(pixelSize > _streetLabelScaleNoShow)
//        return;
//    //take dotproduct between vehicle heading and streetline vector(startpoint -> endpoint)
//    // divide by magnitude(veh heading) * magnitude(streetline vector)

//    double vehLat1 = vehicleSixDof->dVehicleLat;
//    double vehLon1 = vehicleSixDof->dVehicleLon;

//    double vehLat2 = vehicleSixDof->dTargetLat;
//    double vehLon2 = vehicleSixDof->dTargetLon;

//    //lets create this into a vector Veh<deltalat, deltalon> is a unit vector pointing from vehicle in direction of heading/starepoint
//    double deltaLatVeh = vehLat2 - vehLat1;
//    double deltaLonVeh = vehLon2 - vehLon1;


//    //now create a vector in direction of street linestring (take startpoint -> endpoint, disregard points in between for now
//     std::vector<boost::shared_ptr<Kml::Point> > ls = _alignmentVector->GetStreetVectors();
//     int size = ls.size();
//     boost::shared_ptr<Kml::Point> p1 = ls[0];
//     boost::shared_ptr<Kml::Point> p2 = ls[size - 1];

//     //lets create this into a vector Line<deltalat, deltalon> is a vector pointing from startpoint to endpoint
//     //of linestring
//     double deltaLatLine = p2->GetLat() - p1->GetLat();
//     double deltaLonLine = p2->GetLon() - p1->GetLon();

//     //this tells us if the street label is facing us or we need to flip it
//     double zcross = deltaLonVeh * deltaLatLine - deltaLatVeh * deltaLonLine;

//     if(pixelSize > _streetLabelCutoff1)
//     {
//         if(zcross < 0.00)
//         {
//            for(int i = 0 ; i < _forwardFacingIndicesL1.size() ; i++)
//            {
//                indices->push_back(_forwardFacingIndicesL1[i]);
//            }
//         }
//        else
//        {
//            for(int i = 0 ; i < _reverseFacingIndicesL1.size() ; i++)
//            {
//                indices->push_back(_reverseFacingIndicesL1[i]);
//            }
//        }
//     }
//     else if(pixelSize < _streetLabelCutoff1 && pixelSize > _streetLabelCutoff2)
//     {
//         if(zcross < 0.00)
//         {
//            for(int i = 0 ; i < _forwardFacingIndicesL2.size() ; i++)
//            {
//                indices->push_back(_forwardFacingIndicesL2[i]);
//            }
//         }
//        else
//        {
//            for(int i = 0 ; i < _reverseFacingIndicesL2.size() ; i++)
//            {
//                indices->push_back(_reverseFacingIndicesL2[i]);
//            }
//        }
//     }
//     else if(pixelSize < _streetLabelCutoff2 && pixelSize > _streetLabelCutoff3)
//     {
//         if(zcross < 0.00)
//         {
//            for(int i = 0 ; i < _forwardFacingIndicesL3.size() ; i++)
//            {
//                indices->push_back(_forwardFacingIndicesL3[i]);
//            }
//         }
//        else
//        {
//            for(int i = 0 ; i < _reverseFacingIndicesL3.size() ; i++)
//            {
//                indices->push_back(_reverseFacingIndicesL3[i]);
//            }
//        }
//     }
//     else if(pixelSize > _streetLabelCutoff4)
//     {
//         if(zcross < 0.00)
//         {
//            for(int i = 0 ; i < _forwardFacingIndicesL4.size() ; i++)
//            {
//                indices->push_back(_forwardFacingIndicesL4[i]);
//            }
//         }
//        else
//        {
//            for(int i = 0 ; i < _reverseFacingIndicesL4.size() ; i++)
//            {
//                indices->push_back(_reverseFacingIndicesL4[i]);
//            }
//        }
//     }
//     else
//     {
//         if(zcross < 0.00)
//         {
//            for(int i = 0 ; i < _forwardFacingIndicesL5.size() ; i++)
//            {
//                indices->push_back(_forwardFacingIndicesL5[i]);
//            }
//         }
//        else
//        {
//            for(int i = 0 ; i < _reverseFacingIndicesL5.size() ; i++)
//            {
//                indices->push_back(_reverseFacingIndicesL5[i]);
//            }
//        }
//     }


//}

//void openglalignedtext::AddLabelVerticesSingle(boost::shared_ptr<std::vector<float> > vecLabelVertices, std::vector<boost::shared_ptr<StreetSegment> > segments,
//                                               std::vector<unsigned int>& indices, double streetLength, float scale)
//{

//    _vectorArrayOffset = vecLabelVertices->size() / 5;
//    std::vector<boost::shared_ptr<Kml::Point> > ls = _alignmentVector->GetStreetVectors();

//    //double generalScale = 1.0;//todo: fix
//    double stringLength = CalculateStringLength(_text); //TODO: CALL NEW CLASS
////return ;

//    double textSize = scale * stringLength;
//    double currentLocation = (streetLength - textSize) / 2;
//    if(streetLength < textSize)
//        return ;


//    //use openglfont to grab spacing between text etc, and create an arraybuff
//    //where each item has texture coords along with vertex coords.
//    //int totalAdvance = CalculateTotalAdvance();


//    // Center the text at the origin
//    int fontWidth = _openglFont->GetFontWidth();
//    int fontHeight = _openglFont->GetFontHeight();
//    for(int i = 0 ; i < _text.size() ; i ++)
//    {
//        int index = i * 4;
//        unsigned char cc = _text[i];

//        double charX = 0.00;
//        double charY = 0.00;
//        double charAlt = 0.00;

//        //float labelx = 0.00;
//        //float labely = 0.00;
//        //float labelz = 0.00;

//        double rotation = FindRotationSegments(segments, currentLocation, 0.00, charX, charY, charAlt);
//        double rotRadians = degreeToRadians(rotation);

//        //todo: I don't think we use the normalized values, remove??
//        double lowerNormalizedx = 0.0;
//        double lowerNormalizedy = 0.0;
//        double upperNormalizedx = 0.0;
//        double upperNormalizedy = 0.0;

//        int width = 0;
//        int height = 0;
//        int advance = 0;
//        int originX = 0;
//        int originY = 0;
//        int charx = 0;
//        int chary = 0;
//        _openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);

//        //        // Measure how wide the text is
//        //        for (var i = 0; i < text.length; i++) {
//        //          totalAdvance += font.characters[text[i]].advance;
//        //        }

//        //        // Center the text at the origin
//        //        var x = -totalAdvance / 2;
//        //        var y = font.size / 2;
//        //        var vertices = [];

//        //        // Add a quad for each character
//        //        for (var i = 0; i < text.length; i++) {
//        //          var c = font.characters[text[i]];

//        //          // p0 --- p1
//        //          // | \     |
//        //          // |   \   |
//        //          // |     \ |
//        //          // p2 --- p3

//        //origin x,y is the distance from center of char to leading edge?
//        //charx chary is location right on transname line
//        float x = static_cast<float>(charX);
//        float y = (static_cast<float>(charY));

//        //originX *= scale;
//        //originY *= scale;
//        //width *= scale;
//        //height *= scale;
//        float x0 = (x - originX * scale ) ;
//        float y0 = ( y + originY * scale) ;
//        float s0 = static_cast<float>(charx) / static_cast<float>(fontWidth);
//        float t0 = static_cast<float>(fontHeight - chary) / static_cast<float>(fontHeight);

//        float x1 = (x - (originX * scale) + (width * scale)) ;
//        float y1 = (y + originY * scale) ;
//        float s1 = static_cast<float>(charx + width) / static_cast<float>(fontWidth);
//        float t1 = static_cast<float>(fontHeight - chary) / static_cast<float>(fontHeight);

//        float x2 = (x - originX * scale) ;
//        float y2 = (y + (originY - height) * scale) ;
//        float s2 = static_cast<float>(charx) / static_cast<float>(fontWidth);
//        float t2 = static_cast<float>(fontHeight - chary - height) / static_cast<float>(fontHeight);

//        float x3 = (x - (originX * scale) + (width * scale)) ;
//        float y3 = (y + (originY - height) * scale) ;
//        float s3 = static_cast<float>(charx + width) / static_cast<float>(fontWidth);
//        float t3 = static_cast<float>(fontHeight - chary - height) / static_cast<float>(fontHeight);


//        //rotate the char
//        float xdelta0 = x0 - x;
//        float ydelta0 = y0 - (y);
//        float newx0 = (xdelta0 * cos(rotRadians))  - (ydelta0 * sin(rotRadians))  + x;
//        float newy0 = (xdelta0 * sin(rotRadians))  + (ydelta0 * cos(rotRadians))  + y;

//        float xdelta1 = x1 - x;
//        float ydelta1 = y1 - (y);
//        float newx1 = (xdelta1 * cos(rotRadians))  - (ydelta1 * sin(rotRadians))  + x;
//        float newy1 = (xdelta1 * sin(rotRadians))  + (ydelta1 * cos(rotRadians))  + y;

//        float xdelta2 = x2 - x;
//        float ydelta2 = y2 - (y);
//        float newx2 = (xdelta2 * cos(rotRadians))  - (ydelta2 * sin(rotRadians))  + x;
//        float newy2 = (xdelta2 * sin(rotRadians))  + (ydelta2 * cos(rotRadians))  + y;


//        float xdelta3 = x3 - x;
//        float ydelta3 = y3 - (y);
//        float newx3 = (xdelta3 * cos(rotRadians))  - (ydelta3 * sin(rotRadians))  + x;
//        float newy3 = (xdelta3 * sin(rotRadians))  + (ydelta3 * cos(rotRadians))  + y;



//        vecLabelVertices->push_back((float) newx0); //fill this in with the values given by font for w/h of letter
//        vecLabelVertices->push_back((float) newy0);
//        vecLabelVertices->push_back((float) charAlt);
//        //we're just using a pixel since this is a solid color
//        vecLabelVertices->push_back(s0);
//        vecLabelVertices->push_back(t0);

//        vecLabelVertices->push_back((float) newx1); //fill this in with the values given by font for w/h of letter
//        vecLabelVertices->push_back((float) newy1);
//        vecLabelVertices->push_back((float) charAlt);
//        //we're just using a pixel since this is a solid color
//        vecLabelVertices->push_back(s1);
//        vecLabelVertices->push_back(t1);

//        vecLabelVertices->push_back((float) newx2 ); //fill this in with the values given by font for w/h of letter
//        vecLabelVertices->push_back((float) newy2 );
//        vecLabelVertices->push_back((float) charAlt);
//        //we're just using a pixel since this is a solid color
//        vecLabelVertices->push_back(s2);
//        vecLabelVertices->push_back(t2);


//        vecLabelVertices->push_back((float) newx3 ); //fill this in with the values given by font for w/h of letter
//        vecLabelVertices->push_back((float) newy3 );
//        vecLabelVertices->push_back((float) charAlt);
//        //we're just using a pixel since this is a solid color
//        vecLabelVertices->push_back(s3);
//        vecLabelVertices->push_back(t3);

//        x += advance ;

//        indices.push_back(2 + index + _vectorArrayOffset);
//        indices.push_back(3 + index + _vectorArrayOffset);
//        indices.push_back(0 + index + _vectorArrayOffset);
//        indices.push_back(3 + index + _vectorArrayOffset);
//        indices.push_back(1 + index + _vectorArrayOffset);
//        indices.push_back(0 + index + _vectorArrayOffset);

//        //double letterScale = 1.0;
//        currentLocation += (scale * advance);

//    }
//}

////int openglalignedtext::CalculateTotalAdvance()
////{
////    int totalAdvance = 0;
////    for(int i = 0 ; i < _text.size() ; i ++)
////    {
////        double lowerNormalizedx = 0.0;
////        double lowerNormalizedy = 0.0;
////        double upperNormalizedx = 0.0;
////        double upperNormalizedy = 0.0;
////        int width = 0;
////        int height = 0;
////        int advance = 0;
////        int originX = 0;
////        int originY = 0;
////        int charx = 0;
////        int chary = 0;
////        unsigned char cc = _text[i];
////        _openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
////        totalAdvance += advance;
////    }
////    return totalAdvance;
////}

//void openglalignedtext::AddLabelVertices(boost::shared_ptr<std::vector<float> > vecLabelVertices)
//{

//    std::vector<boost::shared_ptr<Kml::Point> > ls = _alignmentVector->GetStreetVectors();
//    std::vector<boost::shared_ptr<StreetSegment> > streetSegments;
//    double streetLength = CreateStreetSegmentVector(ls, streetSegments);

//    AddLabelVerticesSingle(vecLabelVertices, streetSegments, _forwardFacingIndicesL1, streetLength, _streetLabelScale1);
//    AddLabelVerticesSingle(vecLabelVertices, streetSegments, _forwardFacingIndicesL2, streetLength, _streetLabelScale2);
//    AddLabelVerticesSingle(vecLabelVertices, streetSegments, _forwardFacingIndicesL3, streetLength, _streetLabelScale3);
//    AddLabelVerticesSingle(vecLabelVertices, streetSegments, _forwardFacingIndicesL4, streetLength, _streetLabelScale4);
//    AddLabelVerticesSingle(vecLabelVertices, streetSegments, _forwardFacingIndicesL5, streetLength, _streetLabelScale5);

//    ReversePointList(ls);
//    std::vector<boost::shared_ptr<StreetSegment> > streetSegmentsReverse;
//    streetLength = CreateStreetSegmentVector(ls, streetSegmentsReverse);

//    AddLabelVerticesSingle(vecLabelVertices, streetSegmentsReverse, _reverseFacingIndicesL1, streetLength, _streetLabelScale1);
//    AddLabelVerticesSingle(vecLabelVertices, streetSegmentsReverse, _reverseFacingIndicesL2, streetLength, _streetLabelScale2);
//    AddLabelVerticesSingle(vecLabelVertices, streetSegmentsReverse, _reverseFacingIndicesL3, streetLength, _streetLabelScale3);
//    AddLabelVerticesSingle(vecLabelVertices, streetSegmentsReverse, _reverseFacingIndicesL4, streetLength, _streetLabelScale4);
//    AddLabelVerticesSingle(vecLabelVertices, streetSegmentsReverse, _reverseFacingIndicesL5, streetLength, _streetLabelScale5);

//    _loadingComplete = true;
//}

//void openglalignedtext::RenderChar(double xpos, double ypos, double zpos, std::string character, double scale,
//                                    double rotation, bool tiltUp, double shiftX, double shiftY, double shiftZ, bool centerX,
//                                    double xrot, double yrot, double zrot, boost::shared_ptr<SViewData> viewData, double camx,
//                                    double camy, double camz, QOpenGLShaderProgram* program, QMatrix4x4 projectionMatrix,
//                                    int index, double xcharOffset)
//{

//    _program = program;
//    _arrayBuf.bind();//use model defined in initsymbol geometry

//    int offset = 0;
//    int vertexLocation = _program->attributeLocation("a_position");
//    _program->enableAttributeArray(vertexLocation);
//    _program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexDataLW));

//    // Offset for texture coordinate
//    offset += sizeof(QVector3D);

//    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
//    int texcoordLocation = _program->attributeLocation("a_texcoord");
//    _program->enableAttributeArray(texcoordLocation);
//    _program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexDataLW));


//            QMatrix4x4 modelViewMatrix;
//            modelViewMatrix.setToIdentity();

//            //camera rotations for icon
//            modelViewMatrix.rotate(viewData->dCameraRoll, 0.0f, 0.0f, 1.0f);

//            modelViewMatrix.rotate(-viewData->dCameraPitch, 1.0f, 0.0f, 0.0f);
//            modelViewMatrix.rotate(viewData->dCameraHeading, 0.0f, 1.0f, 0.0f);

//            //rotate so that z is down
//            modelViewMatrix.rotate( -90, 1.0f, 0.0f, 0.0f);
//            modelViewMatrix.translate(-camx, -camy, -camz);

//            modelViewMatrix.translate( xpos, ypos, zpos );
//            modelViewMatrix.rotate(180.0, 1.0, 0.0,0.0);
//            modelViewMatrix.rotate( rotation  , 0.0, 0.0, 1.0);
//            modelViewMatrix.rotate(zrot  , 0.0f, 0.0f, 1.0f);
//            modelViewMatrix.translate(xcharOffset, 0.00, 0.00);

//            //make icon/text flip back to aim up/down towards camera if in the air
//            //////////
//            static double testScale = 100.00;
//            if(testScale > 1.0)
//            {
//                testScale -= 0.1;
//            }
//            modelViewMatrix.scale(scale, scale, scale * 1.5);
//            modelViewMatrix.rotate(90.0, 1.0f, 0.0f, 0.0f);
//            modelViewMatrix.translate( shiftX, 0.0, 0.0 );
//            _program->setUniformValue("mvp_matrix", projectionMatrix * modelViewMatrix);
//            _program->setUniformValue("texture", 0);

//            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//            glEnable(GL_BLEND);

//            glDrawArrays(GL_TRIANGLES, index  * 6, 6 );
//}

//#include <algorithm>
//void openglalignedtext::ReversePointList(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices)
//{
//    std::reverse(lineVertices.begin(),lineVertices.end());
//}
//void openglalignedtext::AlignStreetVectorOrientation(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices)
//{
//    if(lineVertices.size() > 1)
//    {
//        boost::shared_ptr<Kml::Point> ptFirst = lineVertices[0];
//        double ptFirstLat = ptFirst->GetLat();
//        double ptFirstLon = ptFirst->GetLon();

//        boost::shared_ptr<Kml::Point> ptLast = lineVertices[lineVertices.size() - 1];
//        double ptLastLat = ptLast->GetLat();
//        double ptLastLon = ptLast->GetLon();

//        double vehLat = _viewData->dVehicleLat;
//        double vehLon = _viewData->dVehicleLon;
//        double deltaLatFirstPt = ptFirstLat - vehLat;
//        double deltaLonFirstPt = ptFirstLon - vehLon;

//        double deltaLatLastPt = ptLastLat - vehLat;
//        double deltaLonLastPt = ptLastLon - vehLon;


//        double thetaFirstPt = atan(deltaLatFirstPt/deltaLonFirstPt);
//        double headingFirstPt = 0.00;
//        if(ptFirstLon > vehLon)
//        {
//            headingFirstPt = 90.0 - thetaFirstPt;
//        }
//        else
//        {
//            headingFirstPt = 180 + 90.0 - thetaFirstPt;
//        }

//        double thetaLastPt = atan(deltaLatLastPt / deltaLonLastPt);
//        double headingLastPt = 0.00;
//        if(ptLastLon > vehLon)
//        {
//            headingLastPt = 90.0 - thetaLastPt;
//        }
//        else
//        {
//            headingLastPt = 180 + 90.0 - thetaLastPt;
//        }
//        double deltaPtHeadings = headingLastPt - headingFirstPt;
//        if(deltaPtHeadings < 0.00)
//        {
//            deltaPtHeadings *= -1.0;//make heading diff positive
//        }

//        if(headingLastPt > headingFirstPt && deltaPtHeadings < 180.0)
//        {
//            //no need to rearrange point list, points will be calculated first to last left to right
//        }
//        else if(headingLastPt < headingFirstPt && deltaPtHeadings < 180.0)
//        {
//            //reverse
//            ReversePointList(lineVertices);
//        }
//        else if(headingLastPt > headingFirstPt && deltaPtHeadings > 180.0)
//        {
//            //reverse the point list order. points are currently going last to first when going left to right
//            ReversePointList(lineVertices);
//        }
//        else if(headingLastPt < headingFirstPt && deltaPtHeadings > 180.0)
//        {
//            //do nothing, list already in the right order left->right = first to last
//        }
//    }
//}



//double openglalignedtext::CalculateSymbolScale3DCamx(double camx, double camy, double camz, double symbolx,
//                                  double symboly, double symbolz, double fov, double pixelHeight)
//{
//    double vx = camx - symbolx;
//    double vy = camy - symboly;
//    double vz = camz - symbolz;

//    double d = sqrt(pow(vx,2)+pow(vy,2)+pow(vz,2));
//    double fovRadians = fov * (3.14 / 180.0);//convert to radians for tan function
//    double fovRadiansHalf = fovRadians /2;
//    double dgdscale = d * tan(fovRadiansHalf);
//    return dgdscale ;//STREETLABELSCALE;
//}
//double openglalignedtext::CalculateStringLength(std::string text)
//{

//    //_openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
//    //totalAdvance += advance;


//    //const char* c = text.c_str();
//    int size = text.size();
//    double stringLength = 0.00;
//    for(int i = 0 ; i < size ; i++)
//    {
//       // bool space = false;

//        double lowerNormalizedx = 0.0;
//        double lowerNormalizedy = 0.0;
//        double upperNormalizedx = 0.0;
//        double upperNormalizedy = 0.0;
//        int width = 0;
//        int height = 0;
//        int advance = 0;
//        int originX = 0;
//        int originY = 0;
//        int charx = 0;
//        int chary = 0;
//        char letter = text[i];
//        _openglFont->getLetterTextureDimensions(letter, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
//        stringLength += advance;
//    }
//    return stringLength;
//}

////OBSOLETE!!
//void openglalignedtext::render(QOpenGLShaderProgram* program, boost::shared_ptr<SViewData> viewData, QMatrix4x4 projectionMatrix)
//{
//    //todo: send in a ptr to a vector, and viewdata etc.  figure out which version of text we want to render
//    // then add the indices to the ptrvector so that vectortile can just call gldrawelements once which
//    // will take care of rendering all the street text for streets in said vector tile.
//    double camx = 0.00;
//    double camy = 0.00;
//    double camz = 0.00;

//    _coordConverter->GeodeticToCartesian(viewData->dVehicleLat, viewData->dVehicleLon, viewData->dVehicleAltitude, camx, camy, camz);
//    _viewData = viewData;
//    //todo: we will now have to setup a mvp for each letter, then drawarray each letter, because as we zoom in out or move around the scene
//    // and try align text to the pathway, text size needs to be consistent.  ie if we zoom in at a street curve with a label, the line/curve
//    // will get bigger, but text size needs to stay consistent
//    //  we need to keep a consistent text size/scale as we move around the scene/zoom in/out
//    //  ie, if we are drawing text around a curve and zoom in, the street line will get larger
//    //cout<<"rendering aligned text"<<endl;
//    //set arraybuff current
//    //call openglfont with bindfonttexture
//    _openglFont->bindFont();//ie bind font.png texture
//    _arrayBuf.bind();

//    int offset = 0;
//    int vertexLocation = program->attributeLocation("a_position");
//    program->enableAttributeArray(vertexLocation);
//    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexDataLW));

//    // Offset for texture coordinate
//    offset += sizeof(QVector3D);

//    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
//    int texcoordLocation = program->attributeLocation("a_texcoord");
//    program->enableAttributeArray(texcoordLocation);
//    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexDataLW));

//    //////////////////////////new render code letter by letter align to pline
//   // SHIELDTYPES shieldType = NONE;
//    //for(int i = 0 ; i < _text.size() ; i++)
//    //{
//        //shieldType = NONE;
//        boost::shared_ptr<Kml::Vectors> pline = _alignmentVector;// objectAtIndex:i];
//        std::vector<boost::shared_ptr<Kml::Point> > ls = pline->GetStreetVectors();//lineString;

//        //take 1st point and last point and compare.  if 1st point has less of heading in relation
//        //to vehicle than last point, then we don't need to reverse order. else we do need to reverse
//        //the order of points
//        AlignStreetVectorOrientation(ls);

//        //double endPoint = 0.0;
//        std::vector<boost::shared_ptr<StreetSegment> > streetSegments;
//        double streetLength = CreateStreetSegmentVector(ls, streetSegments);

//        //now we have to figure out the size of each letter of text, and text letter count
//        //find full length of text, length of street vector(whole segment) .
//        //take vector length - text length divide by 2.  this gives us the start location
//        //from the beginning of street to place first letter.  Create a new function
//        //findrotation(streetsegments, textloc, charwidth) which will return the rotation needed
//        //for that particular letter, then incrent textloc, and call again.  repeat until all letters
//        //complete.  The whole while printing out the char at given rotation.
//        //shieldType = GetShieldType(pline);
//        //std::string name = pline->GetLabel();
//        boost::shared_ptr<StreetSegment> streetSeg = streetSegments[0];
//        double x1 = streetSeg->GetX1();//x1;
//        double y1 = streetSeg->GetY1();//y1;
//        double z1 = streetSeg->GetElevation();//elevation;

//        double generalScale = 1.0;//todo: fix CalculateSymbolScale3DCamx(camx, camy, camz,
//                                                        // x1, y1, z1, viewData->dfov, 1.0);
//        double stringLength = CalculateStringLength(_text); //TODO: CALL NEW CLASS
//        double textSize = generalScale * stringLength;
//        double currentLocation = (streetLength - textSize) / 2;
//        int strLength = _text.size();// length];
//        //if(shieldType == NONE)
//        //{
//           // no shield just the label
//        for(int j = 0 ; j < strLength ; j++)
//        {
//            char c[2];
//            c[0] = _text[j];
//            c[1] = '\0';
//            std::string strChar(c);
//            double charX = 0.00;
//            double charY = 0.00;
//            double charAlt = 0.00;

//            double labelx = 0.00;
//            double labely = 0.00;
//            double labelz = 0.00;

//            double rotation = FindRotationSegments(streetSegments, currentLocation, 0.00, charX, charY, charAlt);
//            double letterScale = 1.0;//todo: fix CalculateSymbolScale3DCamx(camx, camy, camz,
//                                                 //           charX, charY, charAlt, viewData->dfov, 1.0);



////            void openglalignedtext::RenderChar(double xpos, double ypos, double zpos, std::string character, double scale,
////                                double rotation, bool tiltUp, double shiftX, double shiftY, double shiftZ, bool centerX, double xrot, double yrot,
////                                double zrot,
////                                boost::shared_ptr<SViewData> viewData, double camx, double camy, double camz,
////                                               QOpenGLShaderProgram* program, QMatrix4x4 projectionMatrix, int index, double xcharOffset)

//            //TODO: ADD OUR NEW RENDER ROUTINE,
//            RenderChar(charX, charY, charAlt, strChar, letterScale ,
//                                        rotation-90.0, false, 0.00, 0.00, 0.00, false, 0.00, 0.00,
//                                        0.00, viewData, camx, camy, camz,
//                                        program, projectionMatrix, j, 0.0/*currentLocation*/);


//            int charVal = _text[j];

//            double lowerNormalizedx = 0.0;
//            double lowerNormalizedy = 0.0;
//            double upperNormalizedx = 0.0;
//            double upperNormalizedy = 0.0;
//            int width = 0;
//            int height = 0;
//            int advance = 0;
//            int originX = 0;
//            int originY = 0;
//            int charx = 0;
//            int chary = 0;
//            _openglFont->getLetterTextureDimensions(_text[j], lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
//            float charWidth = width;

//            currentLocation += (letterScale * charWidth);
//        }


//    //////////////////////////end new render code letter by letter align to pline
//    //todo: now we need to create an mvp and set that uniform in the shader for each letter so we can scale dynamically
//    //the text
//   // glScalef(10.0, 10.0, 10.0);
//    //glDrawArrays(GL_TRIANGLES, 0, 6 * _numOfTextChars);

//    //int offset = 0;
//}

//void openglalignedtext::setJustification(int justification)
//{

//}

//void openglalignedtext::setBasePoint(int x, int y)
//{

//}
