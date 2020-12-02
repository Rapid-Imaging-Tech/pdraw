#pragma once
#include "SIL.h"
#include "boost\shared_array.hpp"
#include "boost\shared_ptr.hpp"
#include "SViewData.h"
#include "DJIMetadata.h"
#include "4609SIL.h"

class DJISIL : public SIL
{
public:
	DJISIL();
	~DJISIL();
	virtual void Init(double& frameRate) ;
	virtual boost::shared_array<GLubyte> GetLatestVideo(int &width, int &height) ;
	virtual boost::shared_ptr<SViewData> GetLatestViewData() ;

	virtual int SetPlayTime(double t);
	virtual double GetElapsedTime();
	virtual double GetDuration();

	int SetFileNames(std::string vfn, std::string mfn);
	int SetAltitudeAtLaunchPointandMagCorrect(double aalp,double mc,double toff);
	double AltitudeAtLaunchPoint;
	double magCorrection;
	double timeOffset;
	int AltIsSet;
private:
	double start, etime;
	DJIMetadata _djiMetadata;
	std::string videoFileName, metadataFileName;
	Stanag4609SIL *vsil; 

};