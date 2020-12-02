#pragma once
#if defined(__APPLE__)
//#include "vmath.h"
#else
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#endif

#include <KMLEntities.h>


#include <LFDefinitionsRA.h>

class SymbolData {
public:
    SymbolData(){
        _kmlPathDetails.reset(new KmlPathDetails);
        _hide = false;
    }

    boost::shared_ptr<KmlPathDetails> _kmlPathDetails;
    double _x;
    double _y;
    double _z;
    bool _mapSymbol;
    double _scale;
    double _latitude;
    double _longitude;
    double _altitude;
    GLuint _textureNumber;
    std::string _label;
    std::string _href;
#if defined(__APPLE__)
    //vmath::mat4 _modelViewMatrix;
    //vmath::mat4 _modelViewProjectionMatrix;
#else
    QMatrix4x4 _modelViewMatrix;
    QMatrix4x4 _modelViewProjectionMatrix;
#endif
    bool _streetLabel;
    boost::shared_ptr<Kml::Vectors> _pline;
    bool _hide;
};


class RISStreetColoredPline{
public:
    //boost::shared_ptr<KmlPathDetails> _kmlPathDetails;
    RISStreetColoredPline():_arraySize(0),
        _totalLineStringSize(0)
        

    {
        _lineVertices = 0x00;
#if defined(__APPLE__)
#else
        _arrayBuf.create();
#endif

    };

    ~RISStreetColoredPline(){
      if(_lineVertices != 0x00){
          delete[] _lineVertices;
      }
#if !defined(__APPLE__)
      _arrayBuf.destroy();
#endif
    };

    std::vector<boost::shared_ptr<Kml::Vectors> > _polyinesByColor;
    VertexDataLW* _lineVertices;
    int _totalLineStringSize;
    std::vector<int> _lineStringIndices;
#if defined(__APPLE__)
#else
    QOpenGLBuffer _arrayBuf;
#endif
    int _arraySize;
    //bool _roiRefreshable;
};

class RISStreetColoredPlineCollection{
public:
    RISStreetColoredPlineCollection():
        _roiRefreshable(true),
        _uid(-1),
        _hide(false)
    {

    };
    void AddColoredPline(boost::shared_ptr<RISStreetColoredPline> newPline){
        _coloredPlines.push_back(newPline);
    }

    std::vector<boost::shared_ptr<RISStreetColoredPline> > _coloredPlines;
    bool _roiRefreshable;
    int _uid;
    bool _hide;

};


class RISColor{
public:
    double _red;
    double _green;
    double _blue;
    double _alpha;
};

class RISPlacemarkCollection{
public:
    RISPlacemarkCollection():
        _uid(-1),
        _hide(false)
    {

    }

    //this function is used in the line chopping method of dynamic street label generation
    void addPolylineWithLineString(std::vector<boost::shared_ptr<Kml::Point> > lineString, kmlbase::Color32 color, double width, std::string name){
        //lineString->
        boost::shared_ptr<Kml::Vectors> vecs(new Kml::Vectors);
        vecs->SetLabel(name);
        for(int i = 0 ; i < lineString.size() ; i++){
            vecs->SetStreetVectors(lineString);
        }
        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > sppl(new std::vector<boost::shared_ptr<Kml::Vectors> > );
        sppl->push_back(vecs);
        _plines.push_back(sppl);
    }

    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _plines;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > > _symbols;
    void SetRoiRefreshable(bool val){_roiRefreshable = val;}
    bool GetRoiRefreshable(){return _roiRefreshable ;}
    int _uid;
    bool _hide;
private:
    bool _roiRefreshable;
};
