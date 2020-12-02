//
//  openglautoalignedtext.cpp
//  SmartCam
//
//  Created by David Dvergsten on 4/23/20.
//  Copyright © 2020 Rapid Imaging Technologies. All rights reserved.
//

#include "openglautoalignedtext.hpp"
#include "utilities.h"

//#include "openglautoalignedtext.h"
#include "streetlabelline.hpp"
#define LETTERYOFFSET 10.0f

//void openglautoalignedtext::SplitStreetVector(boost::shared_ptr<Kml::Vectors> original, boost::shared_ptr<Kml::Vectors>& firstHalf, boost::shared_ptr<Kml::Vectors>& secondHalf)
//{
//    std::vector<boost::shared_ptr<Kml::Point> > pointsOrig = original->GetStreetVectors();
//    double totalLength = CalculateStreetVectorLength(original);
//    const double halfWayLength = totalLength / 2.0;
//
//    //create new streetlinevector for first half of split
//    firstHalf.reset(new Kml::Vectors);
//    std::vector<boost::shared_ptr<Kml::Point> > firstHalfPoints;
//
//    //find the midpoint of the full streetvector list, we'll use as the last point of the first half, and first point of 2nd half
//    double midptx = 0.0;
//    double midpty = 0.0;
//    double midptz = 0.0;
//    double midLat = 0.0;
//    double midLon = 0.0;
//    double midalt = 0.0;
//    original->FindMidPoint(midptx, midpty, midptz);
//    boost::shared_ptr<Kml::Point> midPointOrig(new Kml::Point);
//    midPointOrig->SetX(midptx);
//    midPointOrig->SetY(midpty);
//    midPointOrig->SetZ(midptz);
//
//    _coordConverter->CartesianToGeodetic(midptx, midpty, midptz, midLat, midLon, midalt);
//    midPointOrig->SetLat(midLat);
//    midPointOrig->SetLon(midLon);
//    midPointOrig->SetAlt(midalt);
//
//    //do a for loop here adding pts to firsthalf until we get to midptx/y
//    double currentDistance = 0.0;
//
//    //the first point of the first half of the original street vec list is just the first point of the orig list
//    //keep adding existing points from orignal street vec list until we go beyond the midpoint, then just add the midpoint of the orig list
//    //as the last point of the first half streetvec list
//    int index = 0;
//    while(halfWayLength > currentDistance){
//        firstHalfPoints.push_back(pointsOrig[index]);
//        currentDistance += original->DistanceBetweenPoints(pointsOrig[index], pointsOrig[index + 1]);
//        index++;
//    }
//    firstHalfPoints.push_back(midPointOrig);//midpoint of the original line is the end of the 1st half line
//
//    //set the first half streetvectors for calling method to use
//    firstHalf->SetStreetVectors(firstHalfPoints);
//
//    /////////////now do the 2nd half/////////////////
//    secondHalf.reset(new Kml::Vectors);
//    std::vector<boost::shared_ptr<Kml::Point> > secondHalfPoints;
//
//    secondHalfPoints.push_back(midPointOrig);//midpoint of the original line is the start of the second line
//    //just add the rest of the street vector points all the way to the end of the original streetline vec since this is the second half
//    //index should point at the point that is just beyond the midpoint of the original line
//    while(index < pointsOrig.size()){
//        secondHalfPoints.push_back(pointsOrig[index]);
//        index++;
//    }
//    //set the second half streetvectors for calling method to use
//    secondHalf->SetStreetVectors(secondHalfPoints);
//}

openglautoalignedtext::openglautoalignedtext(boost::shared_ptr<openglfont> font, double textScale, boost::shared_ptr<UserSettingsData> usettingsData,
                      boost::shared_ptr<StatusLogger> logger, boost::shared_ptr<CoordinateConverter> coordConverter,
                      boost::shared_ptr<ElevationManagerLocal> EML, std::string text, boost::shared_ptr<Kml::Vectors> spVector, int indexIntoUB):
    _vectorArrayOffset(0),
    _xpadding(2.0),
    _streetLengthL1x(0.00),
    _streetLengthL2x(0.00),//length of both segments of street
    _textWidth(0.00),
    _textHeight(0.00)
    //_collision(false)
{
    _currentSixDof.reset(new SViewData);
    _coordConverter = coordConverter;
    _streetLengthL1x = CalculateStreetVectorLength(spVector, _coordConverter);
    _streetLengthL2x = _streetLengthL1x / 2.0;//2x density labels will show 2x the labels, but street length is cut in half
    _streetLengthL4x = _streetLengthL2x / 2.0;//2x density labels will show 2x the labels, but street length is cut in half
    _streetLengthL8x = _streetLengthL4x / 2.0;//2x density labels will show 2x the labels, but street length is cut in half
    _streetLengthL16x = _streetLengthL8x / 2.0;//2x density labels will show 2x the labels, but street length is cut in half
    
    _indexIntoUniformBuffer = static_cast<float>(indexIntoUB);
    _alignmentVectorL1 = spVector;//streetlabelline will use this to calc collisions between other oglautoalignedtexts
    _text = text;
    _EML = EML;//elevation manager
    LoggableConfigurableObject::Init(usettingsData, logger, "openglalignedtext");
    _openglFont = font;//has all setting for text width etc, texture coordinates
    _textWidth = CalculateStringLength(_text);

    _maxScaleBeforeCollisionL1 = _streetLengthL1x;//?? doesn't make sense
    
    //split in 2, first line segment maxscale before collision
    _maxScaleBeforeCollisionL2 = 0;
    //split in 2, second line segment maxscale before collision
    _maxScaleBeforeCollisionL3 = 0;
    //cout<<"created alignedtext"<<endl;
    
    //x4 scales
    _maxScaleBeforeCollisionL4 = 0;
    _maxScaleBeforeCollisionL5 = 0;
    _maxScaleBeforeCollisionL6 = 0;
    _maxScaleBeforeCollisionL7 = 0;
    
    //x8 scales
    _maxScaleBeforeCollisionL8 = 0;
    _maxScaleBeforeCollisionL9 = 0;
    _maxScaleBeforeCollisionL10 = 0;
    _maxScaleBeforeCollisionL11 = 0;
    _maxScaleBeforeCollisionL12 = 0;
    _maxScaleBeforeCollisionL13 = 0;
    _maxScaleBeforeCollisionL14 = 0;
    _maxScaleBeforeCollisionL15 = 0;
    
    //x8 scales
    _maxScaleBeforeCollisionL16 = 0;
    _maxScaleBeforeCollisionL17 = 0;
    _maxScaleBeforeCollisionL18 = 0;
    _maxScaleBeforeCollisionL19 = 0;
    _maxScaleBeforeCollisionL20 = 0;
    _maxScaleBeforeCollisionL21 = 0;
    _maxScaleBeforeCollisionL22 = 0;
    _maxScaleBeforeCollisionL23 = 0;
    _maxScaleBeforeCollisionL24 = 0;
    _maxScaleBeforeCollisionL25 = 0;
    _maxScaleBeforeCollisionL26 = 0;
    _maxScaleBeforeCollisionL27 = 0;
    _maxScaleBeforeCollisionL28 = 0;
    _maxScaleBeforeCollisionL29 = 0;
    _maxScaleBeforeCollisionL30 = 0;
    _maxScaleBeforeCollisionL31 = 0;
    
}

//take total text string and calculate the width in pixels in the text atlas we use
//this is needed so we can tell when not to render this text, ie if we zoom out so much that the text
//is too big too fit on the street line.  Also used to setup our attribute array.
double openglautoalignedtext::CalculateStringLength(std::string text)
{
    size_t size = text.size();
    double accumStringLength = 0.00;

    for(int i = 0 ; i < size ; i++)
    {
        double lowerNormalizedx = 0.0;//these are not used
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

        //todo: take letter origin,
        accumStringLength += advance + _xpadding;
    }
    return accumStringLength;
}

//calculate the whole length of the street vector that we will place text on.  in Meters or world coordinates
//double openglautoalignedtext::CalculateStreetVectorLength(boost::shared_ptr<Kml::Vectors> lineString){
//    //using namespace Kml;
//    int length = 0;
//    //NSMutableArray* points = lineString.coordinateArray;
//    std::vector<boost::shared_ptr<Kml::Point> > points = lineString->GetStreetVectors();// coordinateArray;
//    size_t size = points.size();// count];
//
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
//
//            double deltaX = abs(x1 - x2);
//            double deltaY = abs(y1 - y2);
//            double segLength = sqrt((deltaX * deltaX) + (deltaY * deltaY));
//            length += segLength;
//        }
//    }
//    return length;
//}

//vectortile will send us a ref to the vector<float> we will push onto it our text vertices and texture coords.
//also center x/y and index into uniform buffer which we will use the z  component of center coord.
//also keep track of the indices into this vector.  Remember it is a shared vector<float> with all other
//street labels inside vectortile.  use size of vector to get indices for this street label.  Only need to keep track of one
//set of indices this time and just either return them all or none.
void openglautoalignedtext::AddLabelVertices(boost::shared_ptr<std::vector<float> > vecLabelVertices)
{
    float firstindex = _indexIntoUniformBuffer;

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
    float currentLeftLetterSide = -1*(_textWidth / 2.0);
    int fontWidth = _openglFont->GetFontWidth();
    int fontHeight = _openglFont->GetFontHeight();
    //todo: take total text length and divide by 2.  take width of first char and divide by 2
    for(int i = 0 ; i < _text.size() ; i ++)
    {
         _vectorArrayOffset = vecLabelVertices->size() / 8;//8 floats for each vec attribute xyz, tex xy, center xyz(index)

        int index = i * 4;
        unsigned char cc = _text[i];
        _openglFont->getLetterTextureDimensions(cc, lowerNormalizedx, lowerNormalizedy, upperNormalizedx, upperNormalizedy, width, height, advance, originX, originY, charx, chary);
        float leftX = currentLeftLetterSide;//left side location of letter box
        float rightX = currentLeftLetterSide + width;//right side location of letter box
        float topY = originY - LETTERYOFFSET;
        float bottomY = originY - height - LETTERYOFFSET;

        //this is the center point of the character that this vertices belongs too
        float centerX = leftX + (width / 2.0);
        float centerY = 0.00f;

        float leftTexture = static_cast<float>(charx) / static_cast<float>(fontWidth);
        float rightTexture = static_cast<float>(charx + width) / static_cast<float>(fontWidth);
        float bottomTexture = 1.0f - (static_cast<float>(fontHeight - chary - height) / static_cast<float>(fontHeight));
        float topTexture = 1.0f - (static_cast<float>(fontHeight - chary) / static_cast<float>(fontHeight));

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

        //p0
        vecLabelVertices->push_back(leftX);
        vecLabelVertices->push_back(topY);
        vecLabelVertices->push_back(0.00f);//I think the shader calcs this with its mix function
        //p0 texture coords
        vecLabelVertices->push_back(leftTexture);
        vecLabelVertices->push_back(topTexture);
        //p0 center point
        vecLabelVertices->push_back(centerX);
        vecLabelVertices->push_back(centerY);
        vecLabelVertices->push_back(_indexIntoUniformBuffer);

        //p1
        vecLabelVertices->push_back(rightX);
        vecLabelVertices->push_back(topY);
        vecLabelVertices->push_back(0.00f);//I think the shader calcs this with its mix function
        //p1 texture coords
        vecLabelVertices->push_back(rightTexture);
        vecLabelVertices->push_back(topTexture);
        //p1 center point
        vecLabelVertices->push_back(centerX);
        vecLabelVertices->push_back(centerY);
        vecLabelVertices->push_back(_indexIntoUniformBuffer);

        //p2
        vecLabelVertices->push_back(leftX);
        vecLabelVertices->push_back(bottomY);
        vecLabelVertices->push_back(0.00f);//I think the shader calcs this with its mix function
        //p2 texture coords
        vecLabelVertices->push_back(leftTexture);
        vecLabelVertices->push_back(bottomTexture);
        //p2 center point
        vecLabelVertices->push_back(centerX);
        vecLabelVertices->push_back(centerY);
        vecLabelVertices->push_back(_indexIntoUniformBuffer);

        //p3
        vecLabelVertices->push_back(rightX);
        vecLabelVertices->push_back(bottomY);
        vecLabelVertices->push_back(0.00f);//I think the shader calcs this with its mix function
        //p3 texture coords
        vecLabelVertices->push_back(rightTexture);
        vecLabelVertices->push_back(bottomTexture);
        //p3 center point
        vecLabelVertices->push_back(centerX);
        vecLabelVertices->push_back(centerY);
        vecLabelVertices->push_back(_indexIntoUniformBuffer);

        _vertexIndices.push_back(_vectorArrayOffset + 0);
        _vertexIndices.push_back(_vectorArrayOffset + 2);
        _vertexIndices.push_back(_vectorArrayOffset + 3);
        _vertexIndices.push_back(_vectorArrayOffset + 1);
        _vertexIndices.push_back(_vectorArrayOffset + 0);
        _vertexIndices.push_back(_vectorArrayOffset + 3);

        currentLeftLetterSide += advance + _xpadding;
        if(_textHeight < topY - bottomY){
            _textHeight = topY - bottomY;
        }
    }

    AddStreetLabelLine(_alignmentVectorL1, _streetLabelLineL1, _maxScaleBeforeCollisionL1);
    
    //find first half and 2nd half of entire street line, so we can double our label density
    SplitStreetVector(_alignmentVectorL1, _alignmentVectorL2, _alignmentVectorL3, _coordConverter);//alignment2 = 1st half of street alignment3 = 2nd half
    
    //we set L2 and L3 because we want more label density when flying lower ie we see more street labels since we're so close to the ground and our footprint is small
    //we will use streetlabelline2/3 for collision detection with other labels in 2x density, and set maxscalebefore collision again later, but for now
    //just use the max scale before we fall off the end of the line with our label
    AddStreetLabelLine(_alignmentVectorL2, _streetLabelLineL2, _maxScaleBeforeCollisionL2);
    AddStreetLabelLine(_alignmentVectorL3, _streetLabelLineL3, _maxScaleBeforeCollisionL3);
    
    //divide the street vector into 1/4s to get 4x label density
    SplitStreetVector(_alignmentVectorL2, _alignmentVectorL4, _alignmentVectorL5, _coordConverter);//alignment4 = 1st half of street alignment2 and align5 = 2nd half
    SplitStreetVector(_alignmentVectorL3, _alignmentVectorL6, _alignmentVectorL7, _coordConverter);//alignment6 = 1st half of street alignment3 and align7 = 2nd half
    
    AddStreetLabelLine(_alignmentVectorL4, _streetLabelLineL4, _maxScaleBeforeCollisionL4);
    AddStreetLabelLine(_alignmentVectorL5, _streetLabelLineL5, _maxScaleBeforeCollisionL5);
    AddStreetLabelLine(_alignmentVectorL6, _streetLabelLineL6, _maxScaleBeforeCollisionL6);
    AddStreetLabelLine(_alignmentVectorL7, _streetLabelLineL7, _maxScaleBeforeCollisionL7);
    
    //create x8 labellines
    SplitStreetVector(_alignmentVectorL4, _alignmentVectorL8, _alignmentVectorL9, _coordConverter);//alignment4 = 1st half of street alignment2 and align5 = 2nd half
    SplitStreetVector(_alignmentVectorL5, _alignmentVectorL10, _alignmentVectorL11, _coordConverter);//alignment6 = 1st half of street alignment3 and align7 = 2nd half
    SplitStreetVector(_alignmentVectorL6, _alignmentVectorL12, _alignmentVectorL13, _coordConverter);//alignment4 = 1st half of street alignment2 and align5 = 2nd half
    SplitStreetVector(_alignmentVectorL7, _alignmentVectorL14, _alignmentVectorL15, _coordConverter);//alignment6 = 1st half of street alignment3 and align7 = 2nd half
    
    AddStreetLabelLine(_alignmentVectorL8, _streetLabelLineL8, _maxScaleBeforeCollisionL8);
    AddStreetLabelLine(_alignmentVectorL9, _streetLabelLineL9, _maxScaleBeforeCollisionL9);
    AddStreetLabelLine(_alignmentVectorL10, _streetLabelLineL10, _maxScaleBeforeCollisionL10);
    AddStreetLabelLine(_alignmentVectorL11, _streetLabelLineL11, _maxScaleBeforeCollisionL11);
    AddStreetLabelLine(_alignmentVectorL12, _streetLabelLineL12, _maxScaleBeforeCollisionL12);
    AddStreetLabelLine(_alignmentVectorL13, _streetLabelLineL13, _maxScaleBeforeCollisionL13);
    AddStreetLabelLine(_alignmentVectorL14, _streetLabelLineL14, _maxScaleBeforeCollisionL14);
    AddStreetLabelLine(_alignmentVectorL15, _streetLabelLineL15, _maxScaleBeforeCollisionL15);
    
    //create x16 labellines
    SplitStreetVector(_alignmentVectorL8, _alignmentVectorL16, _alignmentVectorL17, _coordConverter);//alignment4 = 1st half of street alignment2 and align5 = 2nd half
    SplitStreetVector(_alignmentVectorL9, _alignmentVectorL18, _alignmentVectorL19, _coordConverter);//alignment6 = 1st half of street alignment3 and align7 = 2nd half
    SplitStreetVector(_alignmentVectorL10, _alignmentVectorL20, _alignmentVectorL21, _coordConverter);//alignment4 = 1st half of street alignment2 and align5 = 2nd half
    SplitStreetVector(_alignmentVectorL11, _alignmentVectorL22, _alignmentVectorL23, _coordConverter);//alignment6 = 1st half of street alignment3 and align7 = 2nd half
    SplitStreetVector(_alignmentVectorL12, _alignmentVectorL24, _alignmentVectorL25, _coordConverter);//alignment6 = 1st half of street alignment3 and align7 = 2nd half
    SplitStreetVector(_alignmentVectorL13, _alignmentVectorL26, _alignmentVectorL27, _coordConverter);//alignment6 = 1st half of street alignment3 and align7 = 2nd half
    SplitStreetVector(_alignmentVectorL14, _alignmentVectorL28, _alignmentVectorL29, _coordConverter);//alignment6 = 1st half of street alignment3 and align7 = 2nd half
    SplitStreetVector(_alignmentVectorL15, _alignmentVectorL30, _alignmentVectorL31, _coordConverter);//alignment6 = 1st half of street alignment3 and align7 = 2nd half
    
    AddStreetLabelLine(_alignmentVectorL16, _streetLabelLineL16, _maxScaleBeforeCollisionL16);
    AddStreetLabelLine(_alignmentVectorL17, _streetLabelLineL17, _maxScaleBeforeCollisionL17);
    AddStreetLabelLine(_alignmentVectorL18, _streetLabelLineL18, _maxScaleBeforeCollisionL18);
    AddStreetLabelLine(_alignmentVectorL19, _streetLabelLineL19, _maxScaleBeforeCollisionL19);
    AddStreetLabelLine(_alignmentVectorL20, _streetLabelLineL20, _maxScaleBeforeCollisionL20);
    AddStreetLabelLine(_alignmentVectorL21, _streetLabelLineL21, _maxScaleBeforeCollisionL21);
    AddStreetLabelLine(_alignmentVectorL22, _streetLabelLineL22, _maxScaleBeforeCollisionL22);
    AddStreetLabelLine(_alignmentVectorL23, _streetLabelLineL23, _maxScaleBeforeCollisionL23);
    AddStreetLabelLine(_alignmentVectorL24, _streetLabelLineL24, _maxScaleBeforeCollisionL24);
    AddStreetLabelLine(_alignmentVectorL25, _streetLabelLineL25, _maxScaleBeforeCollisionL25);
    AddStreetLabelLine(_alignmentVectorL26, _streetLabelLineL26, _maxScaleBeforeCollisionL26);
    AddStreetLabelLine(_alignmentVectorL27, _streetLabelLineL27, _maxScaleBeforeCollisionL27);
    AddStreetLabelLine(_alignmentVectorL28, _streetLabelLineL28, _maxScaleBeforeCollisionL28);
    AddStreetLabelLine(_alignmentVectorL29, _streetLabelLineL29, _maxScaleBeforeCollisionL29);
    AddStreetLabelLine(_alignmentVectorL30, _streetLabelLineL30, _maxScaleBeforeCollisionL30);
    AddStreetLabelLine(_alignmentVectorL31, _streetLabelLineL31, _maxScaleBeforeCollisionL31);
    
}

//setup streetlabellines that will be used for collision detection with other labels
void openglautoalignedtext::AddStreetLabelLine(boost::shared_ptr<Kml::Vectors> alignmentVector, boost::shared_ptr<StreetLabelLine> streetLabelLine, double& maxScaleBeforeCollision)
{
    std::vector<boost::shared_ptr<Kml::Point> > pointsL1 = alignmentVector->GetStreetVectors();

    //DENSITY: HAVE TO CREATE 2 MORE FOR EACH HALF WHEN WE DO DOUBLE DENSITY
    std::vector<SLPoint> centerLineLabelPointsL1;
    for(int i = 0 ; i <  pointsL1.size(); i++)
    {
        boost::shared_ptr<Kml::Point> point = pointsL1[i];
        double x = point->GetX();
        double y = point->GetY();
        //double z = point->GetZ();//don't need z for collision detection
        SLPoint slpoint(x, y);
        centerLineLabelPointsL1.push_back(slpoint);
    }
    streetLabelLine.reset(new StreetLabelLine(centerLineLabelPointsL1, _textWidth, _textHeight));
    streetLabelLine->Init();
    maxScaleBeforeCollision = streetLabelLine->_maxScale;
}

void openglautoalignedtext::SetupCollisionScales(boost::shared_ptr<StreetLabelLine> line1, boost::shared_ptr<StreetLabelLine> line2, double& line1MaxScaleBeforeCollision, double& line2MaxScaleBeforeCollision)
{
    bool collision = false;
    double collisionScale = 1.0;
    bool alwaysCollide = false;
    bool neverCollide = false;
    collisionScale = line1->CalculateCollisionScale(line2, alwaysCollide, neverCollide);
    if(neverCollide == false ){
        if(line1MaxScaleBeforeCollision > collisionScale)
            line1MaxScaleBeforeCollision = collisionScale;
        if(line2MaxScaleBeforeCollision > collisionScale)
            line2MaxScaleBeforeCollision = collisionScale;
    }
}

void openglautoalignedtext::DetectLabelCollisionsx16(boost::shared_ptr<openglautoalignedtext> otherAlignedText){
    if(_streetLabelLineL16){//1/4 position
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL16, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL16, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL17){//1/4 position
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL17, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL17, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL18){//1/4 position
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL18, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL18, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL19){//1/4 position
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL19, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL19, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL20){//1/4 position
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL20, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL20, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL21){//1/4 position
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL21, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL21, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL22){//1/4 position
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL22, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL22, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL23){//1/4 position
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL23, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL23, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL24){//1/4 position
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL24, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL24, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL25){//1/4 position
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL25, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL25, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL26){//1/4 position
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL26, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL26, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL27){//1/4 position
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL27, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL27, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL28){//1/4 position
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL28, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL28, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL29){//1/4 position
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL29, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL29, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL30){//1/4 position
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL30, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL30, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
    if(_streetLabelLineL31){//1/4 position
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL16, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL16);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL17, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL17);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL18, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL18);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL19, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL19);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL20, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL20);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL21, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL21);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL22, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL22);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL23, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL23);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL24, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL24);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL25, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL25);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL26, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL26);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL27, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL27);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL28, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL28);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL29, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL29);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL30, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL30);
        SetupCollisionScales(_streetLabelLineL31, otherAlignedText->_streetLabelLineL31, this->_maxScaleBeforeCollisionL31, otherAlignedText->_maxScaleBeforeCollisionL31);
    }
}
double openglautoalignedtext::DetectLabelCollisions(boost::shared_ptr<openglautoalignedtext> otherAlignedText)
{
    bool collision = false;
    double collisionScale = 1.0;
    bool alwaysCollide = false;
    bool neverCollide = false;
    //DENSITY: DETECT COLLISIONS BETWEEN MODE2/MODE3 LABELS, IE IF WE DOUBLE THE LABEL DENSITY THESE NEED TO BE COLLISION DETECTED.
    DetectLabelCollisionsx16(otherAlignedText);
    //level 8-15 streetlines are used for when we are in 2x label density mode
    if(_streetLabelLineL8){//1/4 position
        SetupCollisionScales(_streetLabelLineL8, otherAlignedText->_streetLabelLineL8, this->_maxScaleBeforeCollisionL8, otherAlignedText->_maxScaleBeforeCollisionL8);
        SetupCollisionScales(_streetLabelLineL8, otherAlignedText->_streetLabelLineL9, this->_maxScaleBeforeCollisionL8, otherAlignedText->_maxScaleBeforeCollisionL9);
        SetupCollisionScales(_streetLabelLineL8, otherAlignedText->_streetLabelLineL10, this->_maxScaleBeforeCollisionL8, otherAlignedText->_maxScaleBeforeCollisionL10);
        SetupCollisionScales(_streetLabelLineL8, otherAlignedText->_streetLabelLineL11, this->_maxScaleBeforeCollisionL8, otherAlignedText->_maxScaleBeforeCollisionL11);
        SetupCollisionScales(_streetLabelLineL8, otherAlignedText->_streetLabelLineL12, this->_maxScaleBeforeCollisionL8, otherAlignedText->_maxScaleBeforeCollisionL12);
        SetupCollisionScales(_streetLabelLineL8, otherAlignedText->_streetLabelLineL13, this->_maxScaleBeforeCollisionL8, otherAlignedText->_maxScaleBeforeCollisionL13);
        SetupCollisionScales(_streetLabelLineL8, otherAlignedText->_streetLabelLineL14, this->_maxScaleBeforeCollisionL8, otherAlignedText->_maxScaleBeforeCollisionL14);
        SetupCollisionScales(_streetLabelLineL8, otherAlignedText->_streetLabelLineL15, this->_maxScaleBeforeCollisionL8, otherAlignedText->_maxScaleBeforeCollisionL15);
    }
    if(_streetLabelLineL9){//1/4 position
        SetupCollisionScales(_streetLabelLineL9, otherAlignedText->_streetLabelLineL8, this->_maxScaleBeforeCollisionL9, otherAlignedText->_maxScaleBeforeCollisionL8);
        SetupCollisionScales(_streetLabelLineL9, otherAlignedText->_streetLabelLineL9, this->_maxScaleBeforeCollisionL9, otherAlignedText->_maxScaleBeforeCollisionL9);
        SetupCollisionScales(_streetLabelLineL9, otherAlignedText->_streetLabelLineL10, this->_maxScaleBeforeCollisionL9, otherAlignedText->_maxScaleBeforeCollisionL10);
        SetupCollisionScales(_streetLabelLineL9, otherAlignedText->_streetLabelLineL11, this->_maxScaleBeforeCollisionL9, otherAlignedText->_maxScaleBeforeCollisionL11);
        SetupCollisionScales(_streetLabelLineL9, otherAlignedText->_streetLabelLineL12, this->_maxScaleBeforeCollisionL9, otherAlignedText->_maxScaleBeforeCollisionL12);
        SetupCollisionScales(_streetLabelLineL9, otherAlignedText->_streetLabelLineL13, this->_maxScaleBeforeCollisionL9, otherAlignedText->_maxScaleBeforeCollisionL13);
        SetupCollisionScales(_streetLabelLineL9, otherAlignedText->_streetLabelLineL14, this->_maxScaleBeforeCollisionL9, otherAlignedText->_maxScaleBeforeCollisionL14);
        SetupCollisionScales(_streetLabelLineL9, otherAlignedText->_streetLabelLineL15, this->_maxScaleBeforeCollisionL9, otherAlignedText->_maxScaleBeforeCollisionL15);
    }
    if(_streetLabelLineL10){//1/4 position
        SetupCollisionScales(_streetLabelLineL10, otherAlignedText->_streetLabelLineL8, this->_maxScaleBeforeCollisionL10, otherAlignedText->_maxScaleBeforeCollisionL8);
        SetupCollisionScales(_streetLabelLineL10, otherAlignedText->_streetLabelLineL9, this->_maxScaleBeforeCollisionL10, otherAlignedText->_maxScaleBeforeCollisionL9);
        SetupCollisionScales(_streetLabelLineL10, otherAlignedText->_streetLabelLineL10, this->_maxScaleBeforeCollisionL10, otherAlignedText->_maxScaleBeforeCollisionL10);
        SetupCollisionScales(_streetLabelLineL10, otherAlignedText->_streetLabelLineL11, this->_maxScaleBeforeCollisionL10, otherAlignedText->_maxScaleBeforeCollisionL11);
        SetupCollisionScales(_streetLabelLineL10, otherAlignedText->_streetLabelLineL12, this->_maxScaleBeforeCollisionL10, otherAlignedText->_maxScaleBeforeCollisionL12);
        SetupCollisionScales(_streetLabelLineL10, otherAlignedText->_streetLabelLineL13, this->_maxScaleBeforeCollisionL10, otherAlignedText->_maxScaleBeforeCollisionL13);
        SetupCollisionScales(_streetLabelLineL10, otherAlignedText->_streetLabelLineL14, this->_maxScaleBeforeCollisionL10, otherAlignedText->_maxScaleBeforeCollisionL14);
        SetupCollisionScales(_streetLabelLineL10, otherAlignedText->_streetLabelLineL15, this->_maxScaleBeforeCollisionL10, otherAlignedText->_maxScaleBeforeCollisionL15);
    }
    if(_streetLabelLineL11){//1/4 position
        SetupCollisionScales(_streetLabelLineL11, otherAlignedText->_streetLabelLineL8, this->_maxScaleBeforeCollisionL11, otherAlignedText->_maxScaleBeforeCollisionL8);
        SetupCollisionScales(_streetLabelLineL11, otherAlignedText->_streetLabelLineL9, this->_maxScaleBeforeCollisionL11, otherAlignedText->_maxScaleBeforeCollisionL9);
        SetupCollisionScales(_streetLabelLineL11, otherAlignedText->_streetLabelLineL10, this->_maxScaleBeforeCollisionL11, otherAlignedText->_maxScaleBeforeCollisionL10);
        SetupCollisionScales(_streetLabelLineL11, otherAlignedText->_streetLabelLineL11, this->_maxScaleBeforeCollisionL11, otherAlignedText->_maxScaleBeforeCollisionL11);
        SetupCollisionScales(_streetLabelLineL11, otherAlignedText->_streetLabelLineL12, this->_maxScaleBeforeCollisionL11, otherAlignedText->_maxScaleBeforeCollisionL12);
        SetupCollisionScales(_streetLabelLineL11, otherAlignedText->_streetLabelLineL13, this->_maxScaleBeforeCollisionL11, otherAlignedText->_maxScaleBeforeCollisionL13);
        SetupCollisionScales(_streetLabelLineL11, otherAlignedText->_streetLabelLineL14, this->_maxScaleBeforeCollisionL11, otherAlignedText->_maxScaleBeforeCollisionL14);
        SetupCollisionScales(_streetLabelLineL11, otherAlignedText->_streetLabelLineL15, this->_maxScaleBeforeCollisionL11, otherAlignedText->_maxScaleBeforeCollisionL15);
    }
    if(_streetLabelLineL12){//1/4 position
        SetupCollisionScales(_streetLabelLineL12, otherAlignedText->_streetLabelLineL8, this->_maxScaleBeforeCollisionL12, otherAlignedText->_maxScaleBeforeCollisionL8);
        SetupCollisionScales(_streetLabelLineL12, otherAlignedText->_streetLabelLineL9, this->_maxScaleBeforeCollisionL12, otherAlignedText->_maxScaleBeforeCollisionL9);
        SetupCollisionScales(_streetLabelLineL12, otherAlignedText->_streetLabelLineL10, this->_maxScaleBeforeCollisionL12, otherAlignedText->_maxScaleBeforeCollisionL10);
        SetupCollisionScales(_streetLabelLineL12, otherAlignedText->_streetLabelLineL11, this->_maxScaleBeforeCollisionL12, otherAlignedText->_maxScaleBeforeCollisionL11);
        SetupCollisionScales(_streetLabelLineL12, otherAlignedText->_streetLabelLineL12, this->_maxScaleBeforeCollisionL12, otherAlignedText->_maxScaleBeforeCollisionL12);
        SetupCollisionScales(_streetLabelLineL12, otherAlignedText->_streetLabelLineL13, this->_maxScaleBeforeCollisionL12, otherAlignedText->_maxScaleBeforeCollisionL13);
        SetupCollisionScales(_streetLabelLineL12, otherAlignedText->_streetLabelLineL14, this->_maxScaleBeforeCollisionL12, otherAlignedText->_maxScaleBeforeCollisionL14);
        SetupCollisionScales(_streetLabelLineL12, otherAlignedText->_streetLabelLineL15, this->_maxScaleBeforeCollisionL12, otherAlignedText->_maxScaleBeforeCollisionL15);
    }
    if(_streetLabelLineL13){//1/4 position
        SetupCollisionScales(_streetLabelLineL13, otherAlignedText->_streetLabelLineL8, this->_maxScaleBeforeCollisionL13, otherAlignedText->_maxScaleBeforeCollisionL8);
        SetupCollisionScales(_streetLabelLineL13, otherAlignedText->_streetLabelLineL9, this->_maxScaleBeforeCollisionL13, otherAlignedText->_maxScaleBeforeCollisionL9);
        SetupCollisionScales(_streetLabelLineL13, otherAlignedText->_streetLabelLineL10, this->_maxScaleBeforeCollisionL13, otherAlignedText->_maxScaleBeforeCollisionL10);
        SetupCollisionScales(_streetLabelLineL13, otherAlignedText->_streetLabelLineL11, this->_maxScaleBeforeCollisionL13, otherAlignedText->_maxScaleBeforeCollisionL11);
        SetupCollisionScales(_streetLabelLineL13, otherAlignedText->_streetLabelLineL12, this->_maxScaleBeforeCollisionL13, otherAlignedText->_maxScaleBeforeCollisionL12);
        SetupCollisionScales(_streetLabelLineL13, otherAlignedText->_streetLabelLineL13, this->_maxScaleBeforeCollisionL13, otherAlignedText->_maxScaleBeforeCollisionL13);
        SetupCollisionScales(_streetLabelLineL13, otherAlignedText->_streetLabelLineL14, this->_maxScaleBeforeCollisionL13, otherAlignedText->_maxScaleBeforeCollisionL14);
        SetupCollisionScales(_streetLabelLineL13, otherAlignedText->_streetLabelLineL15, this->_maxScaleBeforeCollisionL13, otherAlignedText->_maxScaleBeforeCollisionL15);
    }
    
    if(_streetLabelLineL14){//1/4 position
        SetupCollisionScales(_streetLabelLineL14, otherAlignedText->_streetLabelLineL8, this->_maxScaleBeforeCollisionL14, otherAlignedText->_maxScaleBeforeCollisionL8);
        SetupCollisionScales(_streetLabelLineL14, otherAlignedText->_streetLabelLineL9, this->_maxScaleBeforeCollisionL14, otherAlignedText->_maxScaleBeforeCollisionL9);
        SetupCollisionScales(_streetLabelLineL14, otherAlignedText->_streetLabelLineL10, this->_maxScaleBeforeCollisionL14, otherAlignedText->_maxScaleBeforeCollisionL10);
        SetupCollisionScales(_streetLabelLineL14, otherAlignedText->_streetLabelLineL11, this->_maxScaleBeforeCollisionL14, otherAlignedText->_maxScaleBeforeCollisionL11);
        SetupCollisionScales(_streetLabelLineL14, otherAlignedText->_streetLabelLineL12, this->_maxScaleBeforeCollisionL14, otherAlignedText->_maxScaleBeforeCollisionL12);
        SetupCollisionScales(_streetLabelLineL14, otherAlignedText->_streetLabelLineL13, this->_maxScaleBeforeCollisionL14, otherAlignedText->_maxScaleBeforeCollisionL13);
        SetupCollisionScales(_streetLabelLineL14, otherAlignedText->_streetLabelLineL14, this->_maxScaleBeforeCollisionL14, otherAlignedText->_maxScaleBeforeCollisionL14);
        SetupCollisionScales(_streetLabelLineL14, otherAlignedText->_streetLabelLineL15, this->_maxScaleBeforeCollisionL14, otherAlignedText->_maxScaleBeforeCollisionL15);
    }
    if(_streetLabelLineL15){//1/4 position
        SetupCollisionScales(_streetLabelLineL15, otherAlignedText->_streetLabelLineL8, this->_maxScaleBeforeCollisionL15, otherAlignedText->_maxScaleBeforeCollisionL8);
        SetupCollisionScales(_streetLabelLineL15, otherAlignedText->_streetLabelLineL9, this->_maxScaleBeforeCollisionL15, otherAlignedText->_maxScaleBeforeCollisionL9);
        SetupCollisionScales(_streetLabelLineL15, otherAlignedText->_streetLabelLineL10, this->_maxScaleBeforeCollisionL15, otherAlignedText->_maxScaleBeforeCollisionL10);
        SetupCollisionScales(_streetLabelLineL15, otherAlignedText->_streetLabelLineL11, this->_maxScaleBeforeCollisionL15, otherAlignedText->_maxScaleBeforeCollisionL11);
        SetupCollisionScales(_streetLabelLineL15, otherAlignedText->_streetLabelLineL12, this->_maxScaleBeforeCollisionL15, otherAlignedText->_maxScaleBeforeCollisionL12);
        SetupCollisionScales(_streetLabelLineL15, otherAlignedText->_streetLabelLineL13, this->_maxScaleBeforeCollisionL15, otherAlignedText->_maxScaleBeforeCollisionL13);
        SetupCollisionScales(_streetLabelLineL15, otherAlignedText->_streetLabelLineL14, this->_maxScaleBeforeCollisionL15, otherAlignedText->_maxScaleBeforeCollisionL14);
        SetupCollisionScales(_streetLabelLineL15, otherAlignedText->_streetLabelLineL15, this->_maxScaleBeforeCollisionL15, otherAlignedText->_maxScaleBeforeCollisionL15);
    }
    //end of x8 labeldensity setup
    
    //level 4 5 6 7 streetlines are used for when we are in 2x label density mode
    if(_streetLabelLineL4){//1/4 position
        SetupCollisionScales(_streetLabelLineL4, otherAlignedText->_streetLabelLineL4, this->_maxScaleBeforeCollisionL4, otherAlignedText->_maxScaleBeforeCollisionL4);
        SetupCollisionScales(_streetLabelLineL4, otherAlignedText->_streetLabelLineL5, this->_maxScaleBeforeCollisionL4, otherAlignedText->_maxScaleBeforeCollisionL5);
        SetupCollisionScales(_streetLabelLineL4, otherAlignedText->_streetLabelLineL6, this->_maxScaleBeforeCollisionL4, otherAlignedText->_maxScaleBeforeCollisionL6);
        SetupCollisionScales(_streetLabelLineL4, otherAlignedText->_streetLabelLineL7, this->_maxScaleBeforeCollisionL4, otherAlignedText->_maxScaleBeforeCollisionL7);
    }
    if(_streetLabelLineL5){//1/4 position
        SetupCollisionScales(_streetLabelLineL5, otherAlignedText->_streetLabelLineL4, this->_maxScaleBeforeCollisionL5, otherAlignedText->_maxScaleBeforeCollisionL4);
        SetupCollisionScales(_streetLabelLineL5, otherAlignedText->_streetLabelLineL5, this->_maxScaleBeforeCollisionL5, otherAlignedText->_maxScaleBeforeCollisionL5);
        SetupCollisionScales(_streetLabelLineL5, otherAlignedText->_streetLabelLineL6, this->_maxScaleBeforeCollisionL5, otherAlignedText->_maxScaleBeforeCollisionL6);
        SetupCollisionScales(_streetLabelLineL5, otherAlignedText->_streetLabelLineL7, this->_maxScaleBeforeCollisionL5, otherAlignedText->_maxScaleBeforeCollisionL7);
    }
    if(_streetLabelLineL6){//1/4 position
        SetupCollisionScales(_streetLabelLineL6, otherAlignedText->_streetLabelLineL4, this->_maxScaleBeforeCollisionL6, otherAlignedText->_maxScaleBeforeCollisionL4);
        SetupCollisionScales(_streetLabelLineL6, otherAlignedText->_streetLabelLineL5, this->_maxScaleBeforeCollisionL6, otherAlignedText->_maxScaleBeforeCollisionL5);
        SetupCollisionScales(_streetLabelLineL6, otherAlignedText->_streetLabelLineL6, this->_maxScaleBeforeCollisionL6, otherAlignedText->_maxScaleBeforeCollisionL6);
        SetupCollisionScales(_streetLabelLineL6, otherAlignedText->_streetLabelLineL7, this->_maxScaleBeforeCollisionL6, otherAlignedText->_maxScaleBeforeCollisionL7);
    }
    if(_streetLabelLineL7){//1/4 position
        SetupCollisionScales(_streetLabelLineL7, otherAlignedText->_streetLabelLineL4, this->_maxScaleBeforeCollisionL7, otherAlignedText->_maxScaleBeforeCollisionL4);
        SetupCollisionScales(_streetLabelLineL7, otherAlignedText->_streetLabelLineL5, this->_maxScaleBeforeCollisionL7, otherAlignedText->_maxScaleBeforeCollisionL5);
        SetupCollisionScales(_streetLabelLineL7, otherAlignedText->_streetLabelLineL6, this->_maxScaleBeforeCollisionL7, otherAlignedText->_maxScaleBeforeCollisionL6);
        SetupCollisionScales(_streetLabelLineL7, otherAlignedText->_streetLabelLineL7, this->_maxScaleBeforeCollisionL7, otherAlignedText->_maxScaleBeforeCollisionL7);
    }
    //end of x4 labeldensity setup
    
    //level 2 3 streetlines are used for when we are in 2x label density mode
    if(_streetLabelLineL2){//1/4 position
        SetupCollisionScales(_streetLabelLineL2, otherAlignedText->_streetLabelLineL2, this->_maxScaleBeforeCollisionL2, otherAlignedText->_maxScaleBeforeCollisionL2);
        SetupCollisionScales(_streetLabelLineL2, otherAlignedText->_streetLabelLineL3, this->_maxScaleBeforeCollisionL2, otherAlignedText->_maxScaleBeforeCollisionL3);
    }
    if(_streetLabelLineL3){//3/4 position
        SetupCollisionScales(_streetLabelLineL3, otherAlignedText->_streetLabelLineL2, this->_maxScaleBeforeCollisionL3, otherAlignedText->_maxScaleBeforeCollisionL2);
        SetupCollisionScales(_streetLabelLineL3, otherAlignedText->_streetLabelLineL3, this->_maxScaleBeforeCollisionL3, otherAlignedText->_maxScaleBeforeCollisionL3);
    }
    //these collisions will be used when in 1x mode
    if(_streetLabelLineL1){//1/2 position ie centered on entire street line
        SetupCollisionScales(_streetLabelLineL1, otherAlignedText->_streetLabelLineL1, this->_maxScaleBeforeCollisionL1, otherAlignedText->_maxScaleBeforeCollisionL1);
    }
    //the return value isn't used, we keep track of collision max scale locally in this class
    return 0.0;
}
double openglautoalignedtext::CalculatePitch(boost::shared_ptr<Kml::Vectors> streetVector, double vehx, double vehy, double vehz)
{
    double deltax = streetVector->_midPointx - vehx;
    double deltay = streetVector->_midPointy - vehy;
    double deltaz = vehz - streetVector->_midPointz ;
    double groundDistance = sqrt(deltax*deltax + deltay*deltay);
    double radPitch = atan(deltaz / groundDistance);
    double degPitch = 57.2958 * radPitch;
    degPitch = -1.0 * degPitch;
    return degPitch;
}
void openglautoalignedtext::LabelScaleTest(int currentLevel, int levelToTest, double scaledTextWidth, double streetLength, double maxScale, double textScale, boost::shared_ptr<std::vector<unsigned int> > indices,    boost::shared_ptr<Kml::Vectors> streetVector, double vehx, double vehy, double vehz)
{
    //DENSITY2: CHECK USING VEHICLESIXDOF ALONG WITH LABEL POINT TO GET THE PITCH THE LABEL IS FROM THE VEHICLE.  IF IT'S < -70 THEN RETURN ALL THE INDICES
    if(currentLevel == levelToTest && scaledTextWidth < streetLength && maxScale > textScale)
    {
        //double pitch = CalculatePitch(streetVector,  vehx,  vehy,  vehz);
        //if(pitch < -20)
        {
            //todo: get the scale we are trying to print the label at,
            for(int i =0 ; i < _vertexIndices.size() ;i++)
            {
              indices->push_back(_vertexIndices[i]);
            }
        }
    }
}
//return the label indices used by the DrawElements in vectortile code.  Just return the whole _vertexIndices or nothing at all
void openglautoalignedtext::GetLabelIndices(boost::shared_ptr<std::vector<unsigned int> > indices, boost::shared_ptr<SViewData> vehicleSixDof,
                     double textScale, int currentLevel, double vehx, double vehy, double vehz)
{
    //now just return one set of label indices since we're auto scaling.  But return nothing if a collision detected or line not big enough for text.
    //use
    //double textWidth = _;//todo: this should come from a function

    _currentSixDof = vehicleSixDof;
    //todo: here we will be able to just check scale and eventually look through a list of other openglautoalignedtext sharedptrs
    //  and see if there is a collision at the given scale, and decide if we should display this label
    double scaledTextWidth = _textWidth * textScale;
    //x16 density test
    LabelScaleTest(currentLevel, 16, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL16, textScale, indices, _alignmentVectorL16,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 17, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL17, textScale, indices, _alignmentVectorL17,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 18, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL18, textScale, indices, _alignmentVectorL18,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 19, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL19, textScale, indices, _alignmentVectorL19,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 20, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL20, textScale, indices, _alignmentVectorL20,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 21, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL21, textScale, indices, _alignmentVectorL21,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 22, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL22, textScale, indices, _alignmentVectorL22,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 23, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL23, textScale, indices, _alignmentVectorL23,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 24, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL24, textScale, indices, _alignmentVectorL24,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 25, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL25, textScale, indices, _alignmentVectorL25,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 26, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL26, textScale, indices, _alignmentVectorL26,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 27, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL27, textScale, indices, _alignmentVectorL27,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 28, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL28, textScale, indices, _alignmentVectorL28,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 29, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL29, textScale, indices, _alignmentVectorL29,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 30, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL30, textScale, indices, _alignmentVectorL30,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 31, scaledTextWidth, _streetLengthL16x, _maxScaleBeforeCollisionL31, textScale, indices, _alignmentVectorL31,  vehx,  vehy,  vehz);

    //x8 density test
    LabelScaleTest(currentLevel, 8, scaledTextWidth, _streetLengthL8x, _maxScaleBeforeCollisionL8, textScale, indices, _alignmentVectorL8,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 9, scaledTextWidth, _streetLengthL8x, _maxScaleBeforeCollisionL9, textScale, indices, _alignmentVectorL9,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 10, scaledTextWidth, _streetLengthL8x, _maxScaleBeforeCollisionL10, textScale, indices, _alignmentVectorL10,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 11, scaledTextWidth, _streetLengthL8x, _maxScaleBeforeCollisionL11, textScale, indices, _alignmentVectorL11,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 12, scaledTextWidth, _streetLengthL8x, _maxScaleBeforeCollisionL12, textScale, indices, _alignmentVectorL12,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 13, scaledTextWidth, _streetLengthL8x, _maxScaleBeforeCollisionL13, textScale, indices, _alignmentVectorL13,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 14, scaledTextWidth, _streetLengthL8x, _maxScaleBeforeCollisionL14, textScale, indices, _alignmentVectorL14,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 15, scaledTextWidth, _streetLengthL8x, _maxScaleBeforeCollisionL15, textScale, indices, _alignmentVectorL15,  vehx,  vehy,  vehz);

    //x4 density test
    LabelScaleTest(currentLevel, 4, scaledTextWidth, _streetLengthL4x, _maxScaleBeforeCollisionL4, textScale, indices, _alignmentVectorL4,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 5, scaledTextWidth, _streetLengthL4x, _maxScaleBeforeCollisionL5, textScale, indices, _alignmentVectorL5,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 6, scaledTextWidth, _streetLengthL4x, _maxScaleBeforeCollisionL6, textScale, indices, _alignmentVectorL6,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 7, scaledTextWidth, _streetLengthL4x, _maxScaleBeforeCollisionL7, textScale, indices, _alignmentVectorL7,  vehx,  vehy,  vehz);
    /////////end density 4

    LabelScaleTest(currentLevel, 2, scaledTextWidth, _streetLengthL2x, _maxScaleBeforeCollisionL2, textScale, indices, _alignmentVectorL2,  vehx,  vehy,  vehz);
    LabelScaleTest(currentLevel, 3, scaledTextWidth, _streetLengthL2x, _maxScaleBeforeCollisionL3, textScale, indices, _alignmentVectorL3,  vehx,  vehy,  vehz);

    LabelScaleTest(currentLevel, 1, scaledTextWidth, _streetLengthL1x, _maxScaleBeforeCollisionL1, textScale, indices, _alignmentVectorL1,  vehx,  vehy,  vehz);
}

//gets called on startup, grab any info from the settings.ini file
void openglautoalignedtext::SetUserSettings()
{
    //cout<<"set user settings"<<endl;
}
