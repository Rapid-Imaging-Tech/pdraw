#pragma once
#ifndef __GL_COMPONENT
#define __GL_COMPONENT

#define PI           3.14159
#define TWO_PI       PI*2.0
#define HALF_PI      PI/2.0
#include "ROIMan.h"
#include "boost/shared_ptr.hpp"
//#include "SymbolMgr.h"
#include "SViewData.h"
#include "KMLParser.h"
#include "StatusLogger.h"
#include "VideoPlayer.h"
#include "LFClient.h"
#include "WebMapServiceMgr.h"
#include "KMLEntities.h"
#include "StreetEntities.h"
#include "UserSettings.h"

class CTargaImage;

class LFClientEngine: public LFClient, LoggableObject
{
private:
	boost::condition_variable _kmlRequestCondition;
	bool _kmlRequestReady;
	int _levelOfDetail;
	//bool GetKmlThreadEntered();
	//void SetKmlThreadEntered(bool val);
	//bool _kmlThreadEntered;
	bool _markersOn;
	boost::mutex _kmlRequestMutex;
	void KMLRequestThread();
	boost::mutex _viewDataMutex;
	boost::shared_ptr<SViewData> GetViewData();
	void SetViewData(boost::shared_ptr<SViewData> val);
	void StartupKmlRequestThread();
	boost::shared_ptr<boost::thread> _kmlRequestThread;
	//void RefreshKMLfromWMS();
	//void SetKmlThreadEntered(boost::shared_ptr<SViewData> val);
	//std::vector<boost::shared_ptr<boost::thread> > _kmlThreadQueue;
	//std::vector<boost::shared_ptr<boost::thread> > GetThreadQueue;//lock around this
protected:
	virtual void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData){};
public:

	virtual int GetBestLODforROISize();
	virtual void ToggleMarkers();
	virtual void UpdateLocationText(boost::shared_ptr<SViewData> viewPointData, int XPixel, int YPixel);
	std::string GetLocationText();
	void StreetLayerToggle();
	virtual void SetLOD(int lod);	
	virtual int GetLOD();
	virtual int SetUserSettings(boost::shared_ptr<UserSettingsData> data);
	virtual SERVERSTATE KMLServiceState() ;
	virtual SERVERSTATE ElevationServiceState() ;
	virtual void SetAlpha(double val);
	void Resize(int width, int height);
	bool initializationComplete();//done
	virtual bool ShutDown();//done
	bool Init(boost::shared_ptr<SViewData> viewData);//, HDC hdc);//done
	virtual boost::shared_array<unsigned int> GetFramePtr(int* width, int* height);//done
	void SetVP(boost::shared_ptr<SViewData> viewPointData);//done

	LFClientEngine();
	virtual ~LFClientEngine();
	void SetVideoFrame(boost::shared_array<unsigned char> saVideo, const int& width, const int& height);
	//void SetupProjection(double fov, int width, int height);
	//void Render();
	/////////////////////////////////////////////////////////////
	//new functions for interface
	//void SetLatestKLV(const SViewData& viewData);//this is now setvp
	//bool InitializationComplete();
	//void Shutdown();

	/////////////////////////////////////////////////////////////////
	virtual int Size(unsigned int type, int nx, int ny){return 0;};
	virtual int RealizeView(boost::shared_ptr<SViewData> pSViewData){return 0;};
	virtual int OnMouseMove(int nx, int ny);
	virtual int OnButtonDn(int nx, int ny, int code){return 0;};
	virtual int OnButtonUp(int nx, int ny, int code){return 0;};
	virtual int InitVideoSize(int nx, int ny, int pack){return 0;};
	// clear all entities
	virtual int ClearEntities(){return 0;};
	// set the map service URL
	virtual int SetMapServiceURL(std::string url);
	// set the director where placemark icons and shields reside
	virtual int SetIconDirectory(std::string dir){return 0;};
	// set map service layers
	// set map service layers
	virtual int SetServiceLayers(std::vector <LAYERENUM>);
	// Add KML file
	virtual int AddKMLFile(std::string kmlfile);
	// Add KML content
	virtual int AddKMLContent(std::string content){return 0;};
	// Refresh the KML from the server
	virtual int RefreshKMLfromWMS();
	// Send a text script command to be executed by the engine
	virtual int DoCommand(std::string command) {return 0;};
	virtual void SetElevManagerURL(std::string url);
	virtual int GetROILimits(double *limits);
	virtual void *GetFootPrints();
	
// new capabilities added by MFA on August 5, 2016
	virtual void GetGeo4Pixel(boost::shared_ptr<SViewData> viewPointData, int xpxl, int ypxl, double * latitude, double * longitude, double * altitude);
	// compute the x and y pixel location for a given latitutde and longitude
	virtual void GetPixel4Geo(boost::shared_ptr<SViewData> viewPointData, double lat, double lon, int * xpxl, int * ypxl);
	// place mark at geographic location
	virtual void InsertMark(boost::shared_ptr<SViewData> viewPointData, int XPixel, int YPixel, std::string Name, std::string MarkerURL);
	// get location description text
	virtual std::string GetLocationDescriptionText(double latitude, double longitude);

private:
	bool _wcdone ;
	/////////////LFClient Objects
	WebMapServiceMgr _webMapSrvMgr;//responsible for drawing objects ie street lines, labels, symbols
	//KMLParser _kmlParser;
	boost::shared_ptr<ROIMan> _pRoiMan;
	boost::shared_ptr<StatusLogger> _statusLogger;//object for writing to a status log
	boost::shared_ptr<UserSettingsData> _usettingsdata;//the settings read from a settings file
	boost::shared_ptr<UserSettings> _userSettings;//the object used to get settings from a settings file.

	bool _clientUserSettings;
	bool _newKmlFile;
	bool _refreshingKML;
	SERVERSTATE _kmlState;
	SERVERSTATE _elevMgrState;
	bool IsLoadingNow();
	void GetVectorWMS_Update();
	double _fov;
	int _frameWidth;
	int _frameHeight;
	double _desiredAlpha;
	bool _initComplete;
	boost::mutex _initCompleteMutex;
	boost::mutex _framePtrMutex;
	boost::mutex _alphaMutex;
	int _vidWidth;
	int _vidHeight;
	boost::shared_array<GLuint> _simFramePtr;
	void SetFramePtr();
	//HGLRC _hRCRender;
	void SetShutdown(bool val);
	bool GetShutdown();
	boost::mutex _mutexShutdownSignal;
	bool _exitLFClientEngine;
	boost::thread* _mainRenderThread;
	
	GLuint _fboId;
	GLuint _rboId;
	GLuint _depthrboId;
	void CreateRenderBuffer();
	void Render();
	//void StartMainRenderLoop(HDC hdc);
	bool _kmlLoaded;
	void waitForElevationMgr();
	void GetKMLFromFile();
	void SetStreetLabelElevations(std::vector<boost::shared_ptr<Kml::StreetLabel> > labels);
	std::vector<boost::shared_ptr<Kml::Vectors> > _streetVectors;
	void SetStreetElevations(std::vector<boost::shared_ptr<Kml::Vectors> > vecs);
	
	boost::shared_array<unsigned char> _saVideo;
	int _videoWidth;
	int _videoHeight;
	VideoPlayer _videoPlayer;
	//void WriteToStatusLog(std::string message);
	
	GLfloat _alpha;
	GLuint _dlBox;
	double _camx ;
	double _camy ;
	double _camz ;
	GLuint _dlVPXLate;
	GLuint _dlVPRotate;
	GLuint _dlWireBox;
	void ViewPointUpdate(boost::shared_ptr<SViewData> viewData);
	
	int m_windowWidth;
	int m_windowHeight;
	boost::shared_ptr<SViewData> _viewData;
	bool _elevationDataReady;
	bool _viewDataReady;
	
	bool Shutdown();
	//void Prepare(float dt);
	void DrawPlane();
	std::string _kmlFileName;
	std::string _kmlLayerList;
};

#endif
