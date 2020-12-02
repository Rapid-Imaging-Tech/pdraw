#ifndef openglalignedtext_H
#define openglalignedtext_H
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

using namespace boost;
class openglalignedtext : public LoggableConfigurableObject
{
private:

    double _streetLabelScaleNoShow;

    double _streetLabelScale1;
    double _streetLabelScale2;
    double _streetLabelScale3;
    double _streetLabelScale4;
    double _streetLabelScale5;

    double _streetLabelCutoff1;
    double _streetLabelCutoff2;
    double _streetLabelCutoff3;
    double _streetLabelCutoff4;

    int CalculateTotalAdvance();
    void AddLabelVerticesSingle(boost::shared_ptr<std::vector<float> > vecLabelVertices, std::vector<boost::shared_ptr<StreetSegment> > segments,
                                std::vector<unsigned int>& indices, double streetLength, float scale );
    int _vectorArrayOffset;
    std::vector<unsigned int> _forwardFacingIndicesL1;
    std::vector<unsigned int> _forwardFacingIndicesL2;
    std::vector<unsigned int> _forwardFacingIndicesL3;
    std::vector<unsigned int> _forwardFacingIndicesL4;
    std::vector<unsigned int> _forwardFacingIndicesL5;

    std::vector<unsigned int> _reverseFacingIndicesL1;
    std::vector<unsigned int> _reverseFacingIndicesL2;
    std::vector<unsigned int> _reverseFacingIndicesL3;
    std::vector<unsigned int> _reverseFacingIndicesL4;
    std::vector<unsigned int> _reverseFacingIndicesL5;

    double CalculateStringLength(std::string text);
    double CalculateSymbolScale3DCamx(double camx, double camy, double camz, double symbolx,
                                      double symboly, double symbolz, double fov, double pixelHeight);
    double calculateSymbolScaleCamx(double camx, double camy, double camz, double symx, double symy,
                                    double symz, double fov, double pixHeight);
    void ReversePointList(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices);
    boost::shared_ptr<SViewData> _viewData;
    void AlignStreetVectorOrientation(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices);
    //QOpenGLShaderProgram* _program;
//        void RenderChar(double xpos, double ypos, double zpos, std::string character, double scale,
//                            double rotation, bool tiltUp, double shiftX, double shiftY, double shiftZ, bool centerX, double xrot, double yrot,
//                            double zrot,
//                            boost::shared_ptr<SViewData> viewData, double camx, double camy, double camz,
//                                           QOpenGLShaderProgram* program, QMatrix4x4 projectionMatrix, int index, double xcharOffset);

//    void RenderChar(double xpos, double ypos, double zpos, std::string character, double scale,
//                    double rotation, bool tiltUp, double shiftX, double shiftY, double shiftZ, bool centerX, double xrot, double yrot,
//                    double zrot,
//                    boost::shared_ptr<SViewData> viewData, double camx, double camy, double camz,
//                    GLuint program, vmath::mat4 projectionMatrix, int index, double xcharOffset);
    boost::shared_ptr<ElevationManagerLocal> _EML;
    double FindRotationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double charLocation, double charsize,
                                double& charx, double& chary, double& charalt);
    void findCenterLocationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double& x, double& y, double& z);
    double CreateStreetSegmentVector(std::vector<boost::shared_ptr<Kml::Point> > points, std::vector<boost::shared_ptr<StreetSegment> >& streetSegments);
    void CalculatePlineCenterCoord(boost::shared_ptr<Kml::Vectors> pline, double& centerx, double& centery, double& centerz);
    double GetDist2Points(boost::shared_ptr<Kml::Point> coord, boost::shared_ptr<Kml::Point> coord2);
    double CalculatePlineLength(boost::shared_ptr<Kml::Vectors> pline);
    double CalculateStreetVectorLength(boost::shared_ptr<Kml::Vectors> lineString);
    boost::shared_ptr<Kml::Vectors> _alignmentVector;
    void SetUserSettings();
    //int _numOfTextChars;
    std::string _text;
    double _textScale;
    //QOpenGLBuffer _arrayBuf;
    boost::shared_array<VertexDataLW> _lineVertices;
    boost::shared_ptr<openglfont> _openglFont;
    boost::shared_ptr<CoordinateConverter> _coordConverter;
public:
    bool _loadingComplete;
    void GetLabelIndices(boost::shared_ptr<std::vector<unsigned int> > indices, boost::shared_ptr<SViewData> vehicleSixDof,
                         double pixelSize);
    void AddLabelVertices(boost::shared_ptr<std::vector<float> > vecLabelVertices);

   // void render(QOpenGLShaderProgram* program, boost::shared_ptr<SViewData> viewData, QMatrix4x4 //projectionMatrix);
    //void setTextOpenglEntities(std::string text,  boost::shared_ptr<Kml::Vectors> alignmentVector, boost::shared_ptr<SViewData> viewData);//setup a buffer/indexes array for the shader, so we can just call draw once
    void setJustification(int justification);//todo: use an enum center left, right
    void setBasePoint(int x, int y);//if left justified, base point is first char, if center justif, base point is center char etc.
    void loadfont();//load the font atlas
    openglalignedtext(boost::shared_ptr<openglfont> font, double textScale, boost::shared_ptr<UserSettingsData> usettingsData,
                      boost::shared_ptr<StatusLogger> logger, boost::shared_ptr<CoordinateConverter> coordConverter,
                      boost::shared_ptr<ElevationManagerLocal> EML, std::string text, boost::shared_ptr<Kml::Vectors> spVector);
};
#endif // openglalignedtext_H


//#ifndef openglalignedtext_H
//#define openglalignedtext_H
//#include <string>
//#include "boost/shared_ptr.hpp"
//#include "openglfont.h"
//#include "LFDefinitionsRA.h"
//#include "LoggableObject.h"
//#include "coordinateconverter.h"
////#include "LFDefinitionsRA.h"
//#include "KMLEntities.h"
//#include "StreetSegment.h"
//#include "elevationmanagerlocal.h"

//using namespace boost;
//class openglalignedtext : public LoggableConfigurableObject
//{
//private:

//    double _streetLabelScaleNoShow;

//    double _streetLabelScale1;
//    double _streetLabelScale2;
//    double _streetLabelScale3;
//    double _streetLabelScale4;
//    double _streetLabelScale5;

//    double _streetLabelCutoff1;
//    double _streetLabelCutoff2;
//    double _streetLabelCutoff3;
//    double _streetLabelCutoff4;

//    int CalculateTotalAdvance();
//    void AddLabelVerticesSingle(boost::shared_ptr<std::vector<float> > vecLabelVertices, std::vector<boost::shared_ptr<StreetSegment> > segments,
//                                std::vector<unsigned int>& indices, double streetLength, float scale );
//    int _vectorArrayOffset;
//    std::vector<unsigned int> _forwardFacingIndicesL1;
//    std::vector<unsigned int> _forwardFacingIndicesL2;
//    std::vector<unsigned int> _forwardFacingIndicesL3;
//     std::vector<unsigned int> _forwardFacingIndicesL4;
//      std::vector<unsigned int> _forwardFacingIndicesL5;

//    std::vector<unsigned int> _reverseFacingIndicesL1;
//    std::vector<unsigned int> _reverseFacingIndicesL2;
//    std::vector<unsigned int> _reverseFacingIndicesL3;
//    std::vector<unsigned int> _reverseFacingIndicesL4;
//    std::vector<unsigned int> _reverseFacingIndicesL5;

//    double CalculateStringLength(std::string text);
//    double CalculateSymbolScale3DCamx(double camx, double camy, double camz, double symbolx,
//                                      double symboly, double symbolz, double fov, double pixelHeight);
//    double calculateSymbolScaleCamx(double camx, double camy, double camz, double symx, double symy,
//                                    double symz, double fov, double pixHeight);
//    void ReversePointList(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices);
//    boost::shared_ptr<SViewData> _viewData;
//    void AlignStreetVectorOrientation(std::vector<boost::shared_ptr<Kml::Point> >& lineVertices);
//    QOpenGLShaderProgram* _program;
//    void RenderChar(double xpos, double ypos, double zpos, std::string character, double scale,
//                        double rotation, bool tiltUp, double shiftX, double shiftY, double shiftZ, bool centerX, double xrot, double yrot,
//                        double zrot,
//                        boost::shared_ptr<SViewData> viewData, double camx, double camy, double camz,
//                                       QOpenGLShaderProgram* program, QMatrix4x4 projectionMatrix, int index, double xcharOffset);
//    boost::shared_ptr<ElevationManagerLocal> _EML;
//    double FindRotationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double charLocation, double charsize,
//                                double& charx, double& chary, double& charalt);
//    void findCenterLocationSegments(std::vector<boost::shared_ptr<StreetSegment> > segments, double& x, double& y, double& z);
//    double CreateStreetSegmentVector(std::vector<boost::shared_ptr<Kml::Point> > points, std::vector<boost::shared_ptr<StreetSegment> >& streetSegments);
//    void CalculatePlineCenterCoord(boost::shared_ptr<Kml::Vectors> pline, double& centerx, double& centery, double& centerz);
//    double GetDist2Points(boost::shared_ptr<Kml::Point> coord, boost::shared_ptr<Kml::Point> coord2);
//    double CalculatePlineLength(boost::shared_ptr<Kml::Vectors> pline);
//    double CalculateStreetVectorLength(boost::shared_ptr<Kml::Vectors> lineString);
//    boost::shared_ptr<Kml::Vectors> _alignmentVector;
//    void SetUserSettings();
//    //int _numOfTextChars;
//    std::string _text;
//    double _textScale;
//    QOpenGLBuffer _arrayBuf;
//    boost::shared_array<VertexDataLW> _lineVertices;
//    boost::shared_ptr<openglfont> _openglFont;
//    boost::shared_ptr<CoordinateConverter> _coordConverter;
//public:
//    bool _loadingComplete;
//    void GetLabelIndices(boost::shared_ptr<std::vector<unsigned int> > indices, boost::shared_ptr<SViewData> vehicleSixDof,
//                         double pixelSize);
//    void AddLabelVertices(boost::shared_ptr<std::vector<float> > vecLabelVertices);

//    void render(QOpenGLShaderProgram* program, boost::shared_ptr<SViewData> viewData, QMatrix4x4 projectionMatrix);
//    //void setTextOpenglEntities(std::string text,  boost::shared_ptr<Kml::Vectors> alignmentVector, boost::shared_ptr<SViewData> viewData);//setup a buffer/indexes array for the shader, so we can just call draw once
//    void setJustification(int justification);//todo: use an enum center left, right
//    void setBasePoint(int x, int y);//if left justified, base point is first char, if center justif, base point is center char etc.
//    void loadfont();//load the font atlas
//    openglalignedtext(boost::shared_ptr<openglfont> font, double textScale, boost::shared_ptr<UserSettingsData> usettingsData,
//                      boost::shared_ptr<StatusLogger> logger, boost::shared_ptr<CoordinateConverter> coordConverter,
//                      boost::shared_ptr<ElevationManagerLocal> EML, std::string text, boost::shared_ptr<Kml::Vectors> spVector);
//};
//#endif // openglalignedtext_H
