#pragma once
#include <iostream>
#include "boost/shared_ptr.hpp"
#include "SViewData.h"
#include "ROIMan.h"
#include <vector>
#include "placemarkdatatypes.h"
#include "roilistener.h"
#include "LoggableObject.h"

class Linework: public LoggableConfigurableObject, public RoiListener {

public:
    int enableStreetVectors(bool enable);
    //int enableStreetLabels(bool enable);
    //int enablePlacemarks(bool enable);

    virtual void OnRoiChange(double lowLat, double lowLon, double upperLat, double upperLon) ;
    void DeleteKMLLines(boost::shared_ptr<KmlPathDetails> path);
    void HideKMLLines(boost::shared_ptr<KmlPathDetails> path);
    void ShowKMLLines(boost::shared_ptr<KmlPathDetails> path);
    void SetStreetLineColor(double red, double green, double blue)
    {
        _streetColorRed = red;
        _streetColorGreen = green;
        _streetColorBlue = blue;
    }
    void SetStreetLineWidth(double lineWidth)
    {
        _lineWidth = lineWidth;
    }

    bool PendingSwapFromListener();
    bool _signalSwap;
    virtual void OnPendingRoiChange();
    virtual void OnRoiChangeSwap();
    virtual void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
    ~Linework();
    void Init(boost::shared_ptr<ROIMan> pRoiMan, QOpenGLShaderProgram* programRGBLinework);
    void AddPlacemarks(boost::shared_ptr<RISPlacemarkCollection> placemarkCollection, bool roiRefreshable);
    QOpenGLShaderProgram *_programRGBLinework;
    Linework();
    void AddUserPolylines(boost::shared_ptr<RISPlacemarkCollection> userPolylineCollection);
    void triggerNewROI();

    void render(double camX, double camY, double camZ, boost::shared_ptr<SViewData> viewData, double frameWidth,
                                    double frameHeight, double displayWidth, double displayHeight,
                QOpenGLShaderProgram* program, double alpha);

  private:
    bool _allocateBuffs;
    bool _allocateUserBuffs;
    void resetVerticesBuffers();
    void resetSingleVerticesBuffer(boost::shared_ptr<RISPlacemarkCollection> copyFrom);//, boost::shared_ptr<std::vector<boost::shared_ptr<RISStreetColorPlineCollection> > >& copyTo);
    double _lineWidth;
    double _streetColorRed;
    double _streetColorGreen;
    double _streetColorBlue;
    double _streetColorAlpha;

    bool _pendingSwap;
    std::string _iconFolder;
    boost::shared_ptr<UserSettingsData> _usettingsdata;

    boost::mutex _mutexBackBuffColorSorted;
    boost::mutex _mutexUnsortedPolylines;//streetVectorMutex;

    QMatrix4x4 _modelViewMatrixLW;
    QMatrix4x4 _normalMatrixLW;
    QMatrix4x4 _modelViewProjectionMatrixLW;
    //QMatrix4x4 _projectionMatrix;

    boost::shared_ptr<ROIMan> _pRoiMan;
    bool _generateUserPolylineLists;

    std::vector<boost::shared_ptr<RISPlacemarkCollection> > _unsortedPolylines;
    std::vector<boost::shared_ptr<RISStreetColoredPlineCollection > > _backBuffColorSortedPolylines;
    std::vector<boost::shared_ptr<RISStreetColoredPlineCollection > > _colorSortedPolylines;

    boost::shared_ptr<RISStreetColoredPlineCollection> _userPolylinesSortedByColor;
    boost::shared_ptr<RISStreetColoredPlineCollection> _backBuffuserPolylinesSortedByColor;
    //RISStreetColoredPlineCollection
    boost::shared_ptr<RISStreetColoredPlineCollection > _polylinesSortedByColor;
    boost::shared_ptr<RISStreetColoredPlineCollection> _backBuffPolylinesSortedByColor;
    bool _generateLineworkLists;

    boost::shared_ptr<RISPlacemarkCollection>  _streetPlacemarkCollection;
    boost::shared_ptr<RISPlacemarkCollection>  _userPlinePlacemarkCollection;
    double _camX;
    double _camY;
    double _camZ;
    boost::shared_ptr<SViewData> _viewData;
    void SortToNewVectorsByColor(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > pmarks,
                                           boost::shared_ptr<RISStreetColoredPlineCollection >& sortTo,
                                 bool roiRefreshable, int uid, bool hide);
    bool shouldPrint(boost::shared_ptr<Kml::Vectors> pline);
    void calculateMatrices();
};
