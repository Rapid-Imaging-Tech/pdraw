//
//  openglautoalignedtext.hpp
//  SmartCam
//
//  Created by David Dvergsten on 4/23/20.
//  Copyright © 2020 Rapid Imaging Technologies. All rights reserved.
//

//#ifndef openglautoalignedtext_hpp
//#define openglautoalignedtext_hpp
//
//#include <stdio.h>
//
//#endif /* openglautoalignedtext_hpp */


#ifndef OPENGLAUTOALIGNEDTEXT_H
#define OPENGLAUTOALIGNEDTEXT_H

#include <string>
#include "boost/shared_ptr.hpp"
#include "openglfont.h"
#include "LFDefinitionsRA.h"
#include "LoggableObject.h"
#include "coordinateconverter.h"
//#include "LFDefinitionsRA.h"
#include "KMLEntities.h"
#include "StreetSegment.h"
#include "elevationmanagerlocal.h"
//#include "vmath.h"
#include "streetlabelline.hpp"
using namespace boost;
struct LetterPointAutoAlign{
    float _x;
    float _y;
    LetterPointAutoAlign(float x, float y):
    _x(x),
    _y(y)
    {};
};

class LetterBoxAutoAlign{
public:
    LetterBoxAutoAlign(LetterPointAutoAlign lp0, LetterPointAutoAlign lp1, LetterPointAutoAlign lp2, LetterPointAutoAlign lp3):
    _letterPoints{lp0, lp1, lp2, lp3}
    {

    };
    LetterPointAutoAlign _letterPoints[4];
    bool IsCollision(boost::shared_ptr<LetterBoxAutoAlign> otherLetterBox);
    bool PointIsInside(float testx, float testy);
};

class openglautoalignedtext : public LoggableConfigurableObject
{
public:
    //DENSITY: NEED FOR LEVEL2 AND 3 HERE TOO
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL1;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL2;//1/4 position;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL3;//3/4 position
    
    //x4 label density, divy up into 4 segments so we can show more street labels when flying low
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL4;//1st 1/4
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL5;//2nd 1/4
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL6;//3rd 1/4
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL7;//4th 1/4
    
    //x8 label density, divy up into 4 segments so we can show more street labels when flying low
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL8;//1st 1/4
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL9;//2nd 1/4
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL10;//3rd 1/4
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL11;//4th 1/4
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL12;//1st 1/4
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL13;//2nd 1/4
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL14;//3rd 1/4
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL15;//4th 1/4
    
    //x8 label density, divy up into 4 segments so we can show more street labels when flying low
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL16;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL17;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL18;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL19;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL20;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL21;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL22;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL23;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL24;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL25;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL26;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL27;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL28;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL29;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL30;
    boost::shared_ptr<StreetLabelLine> _streetLabelLineL31;
    
    int _vectorArrayOffset;
    std::vector<unsigned int> _vertexIndices;
//    bool DetectCollisionByLevel(std::shared_ptr<std::vector<std::shared_ptr<LetterBox>>> thisLetterBoxes,
//                                std::shared_ptr<std::vector<std::shared_ptr<LetterBox>>> theirLetterBoxes);
     double DetectLabelCollisions(boost::shared_ptr<openglautoalignedtext> otherAlignedText);
    void DetectLabelCollisionsx16(boost::shared_ptr<openglautoalignedtext> otherAlignedText);
     //incorporate a way to detect at what scale a collision occurs
     std::vector<boost::shared_ptr<openglautoalignedtext>> _collisions;//we now use 1 level since we autoscale in the shader
private:
    boost::shared_ptr<SViewData> _currentSixDof;
    double CalculatePitch(boost::shared_ptr<Kml::Vectors> streetVector, double vehx, double vehy, double vehz);
    void LabelScaleTest(int currentLevel, int levelToTest, double scaledTextWidth, double streetLength, double maxScale, double textScale, boost::shared_ptr<std::vector<unsigned int> > indices,
                        boost::shared_ptr<Kml::Vectors> streetVector, double vehx, double vehy, double vehz);
    void SetupCollisionScales(boost::shared_ptr<StreetLabelLine> line1, boost::shared_ptr<StreetLabelLine> line2, double& line1MaxScaleBeforeCollision, double& line2MaxScaleBeforeCollision);
    void AddStreetLabelLine(boost::shared_ptr<Kml::Vectors> alignmentVector, boost::shared_ptr<StreetLabelLine> streetLabelLine, double& maxScaleBeforeCollision);
    //void SplitStreetVector(boost::shared_ptr<Kml::Vectors> original, boost::shared_ptr<Kml::Vectors>& firstHalf, boost::shared_ptr<Kml::Vectors>& secondHalf);
    //level 1 label ie 1 label per streetline
     double _maxScaleBeforeCollisionL1;
    
    //level 2 label ie 2 labels for the street line ie we split it in half and create 2 autoscale labels
    double _maxScaleBeforeCollisionL2;
    double _maxScaleBeforeCollisionL3;
    
    //level 4x label ie 4 labels for the street line ie we split it in half and create 2 autoscale labels then split again
    double _maxScaleBeforeCollisionL4;
    double _maxScaleBeforeCollisionL5;
    double _maxScaleBeforeCollisionL6;
    double _maxScaleBeforeCollisionL7;
    
    //x8 scales
    double _maxScaleBeforeCollisionL8 ;
    double _maxScaleBeforeCollisionL9 ;
    double _maxScaleBeforeCollisionL10 ;
    double _maxScaleBeforeCollisionL11 ;
    double _maxScaleBeforeCollisionL12 ;
    double _maxScaleBeforeCollisionL13 ;
    double _maxScaleBeforeCollisionL14 ;
    double _maxScaleBeforeCollisionL15 ;
    
    //x16 scales
    double _maxScaleBeforeCollisionL16 ;
    double _maxScaleBeforeCollisionL17 ;
    double _maxScaleBeforeCollisionL18 ;
    double _maxScaleBeforeCollisionL19 ;
    double _maxScaleBeforeCollisionL20 ;
    double _maxScaleBeforeCollisionL21 ;
    double _maxScaleBeforeCollisionL22 ;
    double _maxScaleBeforeCollisionL23 ;
    double _maxScaleBeforeCollisionL24 ;
    double _maxScaleBeforeCollisionL25 ;
    double _maxScaleBeforeCollisionL26 ;
    double _maxScaleBeforeCollisionL27 ;
    double _maxScaleBeforeCollisionL28 ;
    double _maxScaleBeforeCollisionL29 ;
    double _maxScaleBeforeCollisionL30 ;
    double _maxScaleBeforeCollisionL31 ;
    
    //bool _collision;
     double _textHeight; //take max top/bottom when we calc shader array for this
     double _textWidth;
    
    //street lengths for us to pair up a label with.  If text is too long at given scale, then don't show it.  This is our comparison value
     double _streetLengthL1x;
    double _streetLengthL2x;
    double _streetLengthL4x;
    double _streetLengthL8x;
    double _streetLengthL16x;
    
     //double CalculateStreetVectorLength(boost::shared_ptr<Kml::Vectors> lineString);
     float _xpadding ;
     double CalculateStringLength(std::string text);
    float _indexIntoUniformBuffer;
    double _streetLabelScaleNoShow;

    int CalculateTotalAdvance();
   // void AddLabelVerticesSingle(std::shared_ptr<std::vector<float> > vecLabelVertices, std::vector<std::shared_ptr<StreetSegment> > segments,
     //                           std::vector<unsigned int>& indices, double streetLength, float scale,
       //                         std::shared_ptr<std::vector<std::shared_ptr<LetterBoxAutoAlign> > > letterBoxVector );
    double CalculateSymbolScale3DCamx(double camx, double camy, double camz, double symbolx,
                                      double symboly, double symbolz, double fov, double pixelHeight);
    double calculateSymbolScaleCamx(double camx, double camy, double camz, double symx, double symy,
                                    double symz, double fov, double pixHeight);
    void ReversePointList(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices);
    boost::shared_ptr<SViewData> _viewData;
    void AlignStreetVectorOrientation(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices);

    boost::shared_ptr<ElevationManagerLocal> _EML;
    double FindRotationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double charLocation, double charsize,
                                double& charx, double& chary, double& charalt);
    void CalculatePlineCenterCoord(boost::shared_ptr<Kml::Vectors> pline, double& centerx, double& centery, double& centerz);
    double GetDist2Points(boost::shared_ptr<Kml::Point> coord, boost::shared_ptr<Kml::Point> coord2);
    double CalculatePlineLength(boost::shared_ptr<Kml::Vectors> pline);
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL1;
    
    //x2 labeldensity collision detection
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL2;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL3;
    
    //x4 labeldensity collision detection
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL4;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL5;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL6;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL7;
    
    //x8 labeldensity collision detection
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL8;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL9;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL10;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL11;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL12;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL13;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL14;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL15;
    
    //x8 labeldensity collision detection
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL16;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL17;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL18;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL19;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL20;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL21;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL22;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL23;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL24;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL25;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL26;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL27;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL28;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL29;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL30;
    boost::shared_ptr<Kml::Vectors> _alignmentVectorL31;
    
    void SetUserSettings();
    std::string _text;
    double _textScale;
    boost::shared_array<VertexDataLW> _lineVertices;
    boost::shared_ptr<openglfont> _openglFont;
    boost::shared_ptr<CoordinateConverter> _coordConverter;
public:
    bool _loadingComplete;
    void GetLabelIndices(boost::shared_ptr<std::vector<unsigned int> > indices, boost::shared_ptr<SViewData> vehicleSixDof,
                         double textScale, int currentLevel, double vehx, double vehy, double vehz);
    void AddLabelVertices(boost::shared_ptr<std::vector<float> > vecLabelVertices);

    void setJustification(int justification);//todo: use an enum center left, right
    void setBasePoint(int x, int y);//if left justified, base point is first char, if center justif, base point is center char etc.
    void loadfont();//load the font atlas
    openglautoalignedtext(boost::shared_ptr<openglfont> font, double textScale, boost::shared_ptr<UserSettingsData> usettingsData,
    boost::shared_ptr<StatusLogger> logger, boost::shared_ptr<CoordinateConverter> coordConverter,
                                                 boost::shared_ptr<ElevationManagerLocal> EML, std::string text, boost::shared_ptr<Kml::Vectors> spVector, int indexIntoUB);
};

#endif // OPENGLAUTOALIGNEDTEXT_H
