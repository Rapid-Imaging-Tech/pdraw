#include <string>
#include "KMLEngine.h"
#include "ROIMan.h"
#include "boost/shared_ptr.hpp"
#include "KMLEntities.h"
//#include "SymbolMgr.h"
#include "StreetEntities.h"
#include "LoggableObject.h"
#include "WMSQuery.h"
#include <QtGui/QOpenGLShaderProgram>
#include <linework.h>
#include <symbolmgr.h>
#include <streetlabelsview.h>
//enum VECTORTYPE{STREETVEC, POLYGONVEC};

class WebMapServiceMgr: public LoggableConfigurableObject
{
private:
    boost::shared_ptr<KMLEngine> _kmlEngine;
    boost::shared_ptr<StreetLabelsView> _streetLabelsView;
    boost::shared_ptr<Linework> _linework;
    QOpenGLShaderProgram *_programRGBLinework;
    //QOpenGLShaderProgram *_programRGBSymbol;
	double _frameWidth;
	double _frameHeight;
    double _displayWidth;
    double _displayHeight;

	bool GetTriggerRefreshSymbols();
	bool _triggerRefreshSymbols;
	void TriggerRefreshSymbols();
    void SetStreetVectors(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > vecs);
    void SetUserPlineVectors(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > vecs);
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > GetStreetVectors();

    void SetPolygonVectors(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > vecs);
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > GetPolygonVectors();

	boost::mutex _streetVectorMutex;
	boost::mutex _polygonVectorMutex;
	boost::mutex _triggerRefreshSymbolsMutex;

	std::string _locationText;
	bool _roadLabelsOn;
	bool _roadLinesOn;
	int _openglVersionMaj;
    bool _streetVectorsEnabled;
    bool _streetLabelsEnabled;
    bool _placemarksEnabled;
public:
    void SetStreetLabelScale(double scale){_streetLabelsView->SetLabelScale(scale);}
    void SetStreetLineWidth(double lineWidth){_linework->SetStreetLineWidth(lineWidth);}
    void SetStreetLineColor(double red, double green, double blue){_linework->SetStreetLineColor(red, green, blue);}
    boost::shared_ptr<KMLEngine> GetKmlEngineRef(){return _kmlEngine;};
    int enableStreetVectors(bool enable);//{_streetVectorsEnabled = enable;};
    int enableStreetLabels(bool enable);//{_streetLabelsEnabled = enable;};
    int enablePlacemarks(bool enable);//{_placemarksEnabled = enable;};
	void Shutdown();
	void SetRoadLinesOnOff(bool val);
	void SetRoadLabelsOnOff(bool val);

	std::string GetLocationTextForDisplay(){return _locationText;};
	void InsertMark(boost::shared_ptr<SViewData> viewPointData, double lat, double lon, double alt, std::string Name, std::string MarkerURL);
	void Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID, boost::shared_ptr<ROIMan> roimgr,
        int openglVersionMaj, QOpenGLShaderProgram* lineworkShader);
	void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
	bool LoadKMLForArea(int lod);
	void RefreshSymbols(bool renderThread);
	void RefreshStreets();
	void SetURL(std::string url);
	void SetAlpha(double val);
    void Render(boost::shared_ptr<SViewData> viewData, double pixelHeight,
                double frameWidth, double frameHeight, QOpenGLShaderProgram *program,  QMatrix4x4 projection, double alpha);
    std::vector<boost::shared_ptr<KmlPathDetails> > AddKMLEntitiesFromFile(std::string kmlFileName, bool listOnly);
    void HideKMLEntities(boost::shared_ptr<KmlPathDetails> path);
    void ShowKMLEntities(boost::shared_ptr<KmlPathDetails> path);
    void DeleteKMLEntities(boost::shared_ptr<KmlPathDetails> path);
    //void Init();
	void SetROIMgrReference(boost::shared_ptr<ROIMan> val){this->_pROIMan = val;};
	void SetViewData(boost::shared_ptr<SViewData> viewData){this->_viewData = viewData;};
	WebMapServiceMgr(void);
	~WebMapServiceMgr(void);
	std::string GetKML(std::string host, std::string service, double n, double e, double s, double w, int nx, int ny, int lod, bool locationText);
    //std::string GetKMLSCServer(double roiMaxLat, double roiMaxLon, double roiMinLat, double roiMinLon, int lod, DATATYPE dataType);
	std::string _xml, _host, _mapfilename,_KMLLayerList,_MapLayerList;
	int setKMLLayers(std::string list);
	int setMapLayers(std::string list);
	std::string GetLocationText(double lat, double lon);
private:

	//////////LFClient Objects
    void SetSymbolElevations(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > > points);
    std::string _kmlPort;//todo: remove
	bool _useSC3DKmlServer;
    boost::shared_ptr<SymbolMgr> _symbMgrShader;
	boost::shared_ptr<UserSettingsData> _usettingsdata;
    //StreetEntities _streetEntities;
    //KMLEngine _kmlEngine;
	boost::shared_ptr<ROIMan> _pROIMan;

	std::string _queryPrefix;
	double _alpha;
	bool _streetRefresh;
    std::string _kmlURL;//todo: remove
//    void RenderVectors(double camx, double camy, double camz,
//                       std::vector<boost::shared_ptr<Kml::Vectors> > vectorsToRender,
//                       bool polylines, QOpenGLShaderProgram *program);
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _streetVectors;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _userPlineVectors;
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > _polygonVectors;

    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > > _placemarks;
    void SetStreetLabelElevations(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > > labels);
    void SetStreetElevations(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > > vecs);
	void SetStreetsXY(std::vector<boost::shared_ptr<Kml::Vectors> > vecs);
	//void GetKMLFromFile();
    std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > > GetSymbolPlacemarks();
    void SetSymbolPlacemarks(std::vector<boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > > placemarks);
    boost::shared_ptr<SViewData> _viewData;
};
