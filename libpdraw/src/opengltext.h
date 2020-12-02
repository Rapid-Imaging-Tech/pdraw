#ifndef OPENGLTEXT_H
#define OPENGLTEXT_H
#include <string>
#include "boost/shared_ptr.hpp"
#include "openglfont.h"
#include "LFDefinitionsRA.h"
#include "LoggableObject.h"
class opengltext : public LoggableConfigurableObject
{
private:
    int _totalAdvance ;
    void SetUserSettings();
    int _numOfTextChars;
    std::string _text;
    double _textScale;
#if defined(__APPLE__)
	std::vector<float> _lineVertices;
	GLuint _instanceVBO;
#else
	QOpenGLBuffer _arrayBuf;
	boost::shared_array<VertexDataLW> _lineVertices;
#endif
    
    //QOpenGLBuffer _indexBuf;
    
    boost::shared_ptr<openglfont> _openglFont;
    //boost::shared_array<VertexDataLW> _lineVertices;
public:
    int GetNumChars(){return _numOfTextChars;}
    int GetTotalAdvance(){return _totalAdvance;}
#if defined(__APPLE__)
	void render(GLuint program);
#else
    	void render(QOpenGLShaderProgram* program);
#endif
    void setText(std::string text);//setup a buffer/indexes array for the shader, so we can just call draw once
    void setJustification(int justification);//todo: use an enum center left, right
    void setBasePoint(int x, int y);//if left justified, base point is first char, if center justif, base point is center char etc.
    void loadfont();//load the font atlas
    opengltext(boost::shared_ptr<openglfont> font, double textScale, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger);
};
#endif // OPENGLTEXT_H
