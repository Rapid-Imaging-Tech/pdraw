#pragma once
#include <GL/gl.h>
#include <vector>
#include "LoggableObject.h"

class DynamicDisplayListVector: public LoggableConfigurableObject
{
	public:
        int Init(boost::shared_ptr<UserSettingsData> data, boost::shared_ptr<StatusLogger> logger, std::string classID);//, boost::shared_ptr<ROIMan> roiman)
		void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
		DynamicDisplayListVector();
		//GLuint GetListAtIndex(int index);
		GLuint GetNextList();
		void reset();
		~DynamicDisplayListVector();
		int GetSize();
		//void DeleteLast();
		//void DeleteAtIndex(int index);
	private:
		boost::shared_ptr<UserSettingsData> _usettingsdata;
		//int endPtr;
		std::vector<GLuint> _list;
};
