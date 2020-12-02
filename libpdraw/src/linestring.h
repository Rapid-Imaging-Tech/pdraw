#ifndef LINESTRING_H
#define LINESTRING_H

#include "LoggableObject.h"
#include "LFDefinitionsRA.h"
#include "boost/shared_array.hpp"
#include "qopenglbuffer.h"
#include "qopengltexture.h"
#include "openglfont.h"
#include <QOpenGLShaderProgram>
#include "opengltext.h"

class linestring: public LoggableConfigurableObject
{
private:
    boost::shared_ptr<QOpenGLTexture> _textureWhite;
    std::string _labelText;
    boost::shared_ptr<openglfont> _openglFont;
    boost::shared_ptr<opengltext> _openglText;

        double _textScale;
        //bool _entitiesInitialized;
        double _distToVehicle;
    //    _rotationTK(usettingsData, logger),
    //    _kmlPathDetails(new KmlPathDetails),
        bool _visible;
    QOpenGLBuffer _arrayBuf;
    boost::shared_array<VertexDataLW> _lineVertices;
    virtual void SetUserSettings() ;
public:
    void render(boost::shared_ptr<SViewData> viewData, double windowWidth, double windowHeight,
                QOpenGLShaderProgram* program,  double vehx, double vehy, double vehz, QMatrix4x4 projection);
    ~linestring();
    void UpdateDistToVehicle(double vehLat, double vehLon);
    bool _entitiesInitialized;
    linestring(std::string text, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger,
                   boost::shared_ptr<QOpenGLTexture> whiteTexture, boost::shared_ptr<openglfont> font);
    void InitOpenGLEntities();
};

#endif // LINESTRING_H
