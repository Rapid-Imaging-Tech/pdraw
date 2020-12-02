#pragma once
#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "boost/shared_array.hpp"
#include "SViewData.h"
class SIL
{

public:
	virtual void Init(double& frameRate) = 0;
	virtual boost::shared_array<GLubyte> GetLatestVideo(int &width, int &height) = 0;
	virtual boost::shared_ptr<SViewData> GetLatestViewData()  = 0;
	virtual int SetPlayTime(double t)  = 0;
	virtual double GetElapsedTime()  = 0;
	virtual double GetDuration()   = 0;
};
