//#define FLIRVERSION

#if defined FLIRVERSION
// version 1.0.10 header file
// EDITTED to FLIR software environment
#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <../include/ClientFactory.h>
#include <../include/LFClient.h>
#include <../include/SViewData.h>
#include <boost/shared_ptr.hpp>
#include <../include/geometryengine.h>
#include <QOpenGLTexture>
#include <boost/thread.hpp>

// may need to look at QQuickFramebufferObject?
class smartcamrenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    smartcamrenderer(bool simulation) : m_t(1.0), m_program(0), _lfInited(false), geometries(0), _viewDataSimulate(simulation),
        _lfServiceLayersSet(false), _shadersInited(false),
        _time(0.00),
        _lat(0.00),
        _lon(0.00),
        _alt(0.00),
        _losPitch(0.00),
        _losHeading(0.00),
        _losRoll(0.00),
         _ifov(0.00)
    {
        _viewData.reset(new SViewData);
        _lfClient.reset();
    }
    ~smartcamrenderer();

    //Keep this
    void setT(qreal t) { m_t = t; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }

    int SetView(uint64_t Time,  // unix time in micro seconds
            double lat, double lon, double alt, // camera postion in degrees WGS84, altitude in meters GEOID96
            double LOS_pitch, double LOS_heading, double LOS_roll, // camera attitude in degrees angle relative horizon, relative true north, relative local level for center pixel
            double iFOV);
    int enableStreetVectors(bool enable);
    int enableStreetLabels(bool enable);
    int enablePlacemarks(bool enable);

public slots:
    void paint();

public:
    double _time;
    double _lat;
    double _lon;
    double _alt;
    double _losPitch;
    double _losHeading;
    double _losRoll;
    double _ifov;
private:
    boost::shared_ptr<SViewData> copyViewData(boost::shared_ptr<SViewData> from );
    bool _shadersInited;
    bool _lfServiceLayersSet;
    bool _viewDataSimulate;
    QOpenGLTexture *texture;
    void initTextures();
    void simulateMetadata();
    GeometryEngine *geometries;
    QMatrix4x4 _projection;
    boost::shared_ptr<LFClient> _lfClient;
    ClientFactory _clientFactory;
    boost::shared_ptr<SViewData> _viewData;
    bool _lfInited;
    void init();
    void initShaders();//SMARTCAM
    QSize m_viewportSize;
    qreal m_t;
    QOpenGLShaderProgram m_program;
    QQuickWindow *m_window;
    boost::mutex _dataMutex;
};

class smartcamlayer : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(qreal sim READ sim WRITE setSim NOTIFY simChanged)

    Q_PROPERTY(qreal time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(qreal lat READ lat WRITE setLat NOTIFY latChanged)
    Q_PROPERTY(qreal lon READ lon WRITE setLon NOTIFY lonChanged)
    Q_PROPERTY(qreal alt READ alt WRITE setAlt NOTIFY altChanged)
    Q_PROPERTY(qreal losPitch READ losPitch WRITE setLosPitch NOTIFY losPitchChanged)
    Q_PROPERTY(qreal losHeading READ losHeading WRITE setLosHeading NOTIFY losHeadingChanged)
    Q_PROPERTY(qreal losRoll READ losRoll WRITE setLosRoll NOTIFY losRollChanged)
    Q_PROPERTY(qreal ifov READ ifov WRITE setIfov NOTIFY ifovChanged)

public:
    smartcamlayer();

    Q_INVOKABLE int setView(double Time,  // unix time in micro seconds
            double lat, double lon, double alt, // camera postion in degrees WGS84, altitude in meters GEOID96
            double LOS_pitch, double LOS_heading, double LOS_roll, // camera attitude in degrees angle relative horizon, relative true north, relative local level for center pixel
            double iFOV)
    {
        if (m_renderer)
        {
//            qDebug() << "time=" << Time << " lat" << lat << " lon=" << lon << "heading=" << LOS_heading;
            return m_renderer->SetView((uint64_t)Time,lat,lon,alt,LOS_pitch,LOS_heading,LOS_roll,iFOV);
        }
        qDebug() << "no m_renderer";
        return 0;
    }

    Q_INVOKABLE int enableStreetVectors(bool enable){
        if (m_renderer)
        {
//            qDebug() << "time=" << Time << " lat" << lat << " lon=" << lon << "heading=" << LOS_heading;
            return m_renderer->enableStreetVectors(enable);
        }
    }
    Q_INVOKABLE int enableStreetLabels(bool enable){
        if (m_renderer)
        {
//            qDebug() << "time=" << Time << " lat" << lat << " lon=" << lon << "heading=" << LOS_heading;
            return m_renderer->enableStreetLabels(enable);
        }
    }
    Q_INVOKABLE int enablePlacemarks(bool enable){
        if (m_renderer)
        {
//            qDebug() << "time=" << Time << " lat" << lat << " lon=" << lon << "heading=" << LOS_heading;
            return m_renderer->enablePlacemarks(enable);
        }
    }

    qreal t() const { return m_t; }
    void setT(qreal t);

    qreal sim() const{ return _viewDataSimulate;}
    void setSim(qreal sim);

    qreal time() const{ return _time;}
    void setTime(qreal time);
    qreal lat() const{ return _lat;}
    void setLat(qreal lat);
    qreal lon() const{ return _lon;}
    void setLon(qreal lon);
    qreal alt() const{ return _alt;}
    void setAlt(qreal alt);
    qreal losPitch() const{ return _losPitch;}
    void setLosPitch(qreal losPitch);
    qreal losHeading() const{ return _losHeading;}
    void setLosHeading(qreal losHeading);
    qreal losRoll() const{ return _losRoll;}
    void setLosRoll(qreal losRoll);
    qreal ifov() const{ return _ifov;}
    void setIfov(qreal ifov);


signals:
    void tChanged();
    void simChanged();

    void timeChanged();
    void latChanged();
    void lonChanged();
    void altChanged();
    void losPitchChanged();
    void losHeadingChanged();
    void losRollChanged();
    void ifovChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    qreal m_t;
    smartcamrenderer *m_renderer;
    qreal _viewDataSimulate;

    qreal _time;
    qreal _lat;
    qreal _lon;
    qreal _alt;
    qreal _losPitch;
    qreal _losHeading;
    qreal _losRoll;
    qreal _ifov;
};

//#endif // AR_LAYER_H



#else










#ifndef SMARTCAM_LAYER_H
#define SMARTCAM_LAYER_H
// version 1.0.4 header file
// EDITTED to FLIR software environment 
#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include "ClientFactory.h"
#include "LFClient.h"
#include "SViewData.h"
#include <boost/shared_ptr.hpp>
#include <../LFClientEngineLibrary/include-public/geometryengine.h>
#include <QOpenGLTexture>

// may need to look at QQuickFramebufferObject?
class smartcamrenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    smartcamrenderer(bool simulation) : m_t(1.0), m_program(0), _lfInited(false), geometries(0),
        _viewDataSimulate(simulation),
        _lfServiceLayersSet(false), _shadersInited(false),
        _time(0.00),
        _lat(0.00),
        _lon(0.00),
        _alt(0.00),
        _losPitch(0.00),
        _losHeading(0.00),
        _losRoll(0.00),
         _ifov(0.00)
    {
        _viewData.reset(new SViewData);
        _lfClient.reset();
    }
    ~smartcamrenderer();

    //Keep this
    void setT(qreal t) { m_t = t; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }

    int SetView(u_int64_t Time,  // unix time in micro seconds
            double lat, double lon, double alt, // camera postion in degrees WGS84, altitude in meters GEOID96
            double LOS_pitch, double LOS_heading, double LOS_roll, // camera attitude in degrees angle relative horizon, relative true north, relative local level for center pixel
            double iFOV);
    int enableStreetVectors(bool enable);
    int enableStreetLabels(bool enable);
    int enablePlacemarks(bool enable);

public slots:
    void paint();

public:
    double _time;
    double _lat;
    double _lon;
    double _alt;
    double _losPitch;
    double _losHeading;
    double _losRoll;
    double _ifov;
private:
    std::vector<PlacemarkInfo> _kmlPathDetails;
    bool _shadersInited;
    bool _lfServiceLayersSet;
    bool _viewDataSimulate;
    QOpenGLTexture *texture;
    void initTextures();
    void simulateMetadata();
    GeometryEngine *geometries;
    QMatrix4x4 _projection;
    boost::shared_ptr<LFClient> _lfClient;
    ClientFactory _clientFactory;
    boost::shared_ptr<SViewData> _viewData;
    bool _lfInited;
    void init();
    void initShaders();//SMARTCAM
    QSize m_viewportSize;
    qreal m_t;
    QOpenGLShaderProgram m_program;
    QQuickWindow *m_window;
};

class smartcamlayer : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(qreal sim READ sim WRITE setSim NOTIFY simChanged)

    Q_PROPERTY(qreal time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(qreal lat READ lat WRITE setLat NOTIFY latChanged)
    Q_PROPERTY(qreal lon READ lon WRITE setLon NOTIFY lonChanged)
    Q_PROPERTY(qreal alt READ alt WRITE setAlt NOTIFY altChanged)
    Q_PROPERTY(qreal losPitch READ losPitch WRITE setLosPitch NOTIFY losPitchChanged)
    Q_PROPERTY(qreal losHeading READ losHeading WRITE setLosHeading NOTIFY losHeadingChanged)
    Q_PROPERTY(qreal losRoll READ losRoll WRITE setLosRoll NOTIFY losRollChanged)
    Q_PROPERTY(qreal ifov READ ifov WRITE setIfov NOTIFY ifovChanged)

public:
    smartcamlayer();
virtual void keyPressEvent(QKeyEvent *event);
    Q_INVOKABLE int setView(double Time,  // unix time in micro seconds
            double lat, double lon, double alt, // camera postion in degrees WGS84, altitude in meters GEOID96
            double LOS_pitch, double LOS_heading, double LOS_roll, // camera attitude in degrees angle relative horizon, relative true north, relative local level for center pixel
            double iFOV)
    {
        if (m_renderer)
        {
//            qDebug() << "time=" << Time << " lat" << lat << " lon=" << lon << "heading=" << LOS_heading;
            return m_renderer->SetView((uint64_t)Time,lat,lon,alt,LOS_pitch,LOS_heading,LOS_roll,iFOV);
        }
        qDebug() << "no m_renderer";
        return 0;
    }

    Q_INVOKABLE int enableStreetVectors(bool enable){
        if (m_renderer)
        {
//            qDebug() << "time=" << Time << " lat" << lat << " lon=" << lon << "heading=" << LOS_heading;
            return m_renderer->enableStreetVectors(enable);
        }
    }
    Q_INVOKABLE int enableStreetLabels(bool enable){
        if (m_renderer)
        {
//            qDebug() << "time=" << Time << " lat" << lat << " lon=" << lon << "heading=" << LOS_heading;
            return m_renderer->enableStreetLabels(enable);
        }
    }
    Q_INVOKABLE int enablePlacemarks(bool enable){
        if (m_renderer)
        {
//            qDebug() << "time=" << Time << " lat" << lat << " lon=" << lon << "heading=" << LOS_heading;
            return m_renderer->enablePlacemarks(enable);
        }
    }

    qreal t() const { return m_t; }
    void setT(qreal t);

    qreal sim() const{ return _viewDataSimulate;}
    void setSim(qreal sim);

    qreal time() const{ return _time;}
    void setTime(qreal time);
    qreal lat() const{ return _lat;}
    void setLat(qreal lat);
    qreal lon() const{ return _lon;}
    void setLon(qreal lon);
    qreal alt() const{ return _alt;}
    void setAlt(qreal alt);
    qreal losPitch() const{ return _losPitch;}
    void setLosPitch(qreal losPitch);
    qreal losHeading() const{ return _losHeading;}
    void setLosHeading(qreal losHeading);
    qreal losRoll() const{ return _losRoll;}
    void setLosRoll(qreal losRoll);
    qreal ifov() const{ return _ifov;}
    void setIfov(qreal ifov);


signals:
    void tChanged();
    void simChanged();

    void timeChanged();
    void latChanged();
    void lonChanged();
    void altChanged();
    void losPitchChanged();
    void losHeadingChanged();
    void losRollChanged();
    void ifovChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    qreal m_t;
    smartcamrenderer *m_renderer;
    qreal _viewDataSimulate;

    qreal _time;
    qreal _lat;
    qreal _lon;
    qreal _alt;
    qreal _losPitch;
    qreal _losHeading;
    qreal _losRoll;
    qreal _ifov;
};

#endif // AR_LAYER_H

#endif
