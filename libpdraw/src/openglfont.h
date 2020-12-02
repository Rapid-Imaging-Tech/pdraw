#ifndef OPENGLFONT_H
#define OPENGLFONT_H
#if defined(__APPLE__)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#include <vector>
#include <string>
#include "objcUtilityWrapper.h"
#else
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <qjsonobject.h>
#include <qopengltexture.h>
#include <qopenglshaderprogram.h>
#endif

#include <string>
#include "boost/shared_array.hpp"
#include "LFDefinitionsRA.h"

#include "UserSettingsData.h"

#include "LoggableObject.h"
#if !defined(__APPLE__)
class CharDimensions
{
public:
    CharDimensions(int width, int height, int advance, int origx, int origy, int charx, int chary)
    {
      _width = width;
      _height = height;
      _advance = advance;
      _originX = origx;
      _originY = origy;
      _charx = charx;
      _chary = chary;
    };

    //int x ;//= getCharAttribute(cstr, "x");
    //int y ;//= getCharAttribute(cstr, "y");
    int _width;// = getCharAttribute(cstr, "width");
    int _height;// = getCharAttribute(cstr, "height");

    int _advance;// = getCharAttribute(cstr, "advance");
    int _originX;// = getCharAttribute(cstr, "originX");
    int _originY;// = getCharAttribute(cstr, "originY");

    int _charx;// = getCharAttribute(cstr, "x");
    int _chary;// = getCharAttribute(cstr, "y");


};
#endif
class openglfont: public LoggableConfigurableObject
{
public:
    bool _loadComplete;
    std::map<unsigned char, boost::shared_ptr<CharDimensions> > _characterDimensions;
    int GetFontWidth();
    int GetFontHeight();
    int GetFontSize();
    bool getLetterTextureDimensions(unsigned char cc, double& lowerNormalizedx, double& lowerNormalizedy, double& upperNormalizedx,
                                    double& upperNormalizedy, int& width, int& height, int& advance, int& originX, int& originY,
                                    int& charx, int& chary);
    void bindFont();
#if defined(__APPLE__)
	void drawText(GLuint program);
#else
    void drawText(QOpenGLShaderProgram* program);
#endif
    //void drawText();
    //void SetUserSettings(boost::shared_ptr<UserSettingsData> usettings);
    openglfont( boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger);
    //setup opengl vbo, by reading png and json describing where letters located,
    void loadFont(std::string fontPngPath, std::string jsonPath);
    ~openglfont();
    void generateTextBuffer(std::string text);
private:
    boost::shared_ptr<CharDimensions> CreateCharDimension(std::string character);
    void LoadCharacterDimensions();
    virtual void SetUserSettings();
#if defined(__APPLE__)
    std::string _fontName;
    GLuint loadTextureWithFilename(std::string filename);
#else
    std::vector<QString> _vecChars;
    int getBufferIndex(QString character);
    //QOpenGLTexture* _fontTexture ;
    QOpenGLTexture* _textureToBind;
    QOpenGLTexture* loadTextureWithFilename(std::string filename);
    QJsonObject _characters;
    QString _fontName;
 //boost::shared_ptr<UserSettingsData> _usettingsdata;
    boost::shared_ptr<QOpenGLTexture> _symbolTexture;
    QOpenGLBuffer _arrayBuf;
    QOpenGLBuffer _indexBuf;
#endif
    
    int getCharAttribute(std::string character, std::string attribute);
    //TODO:
    //QJsonObject _characters;

    int _fontSize;
    bool _bold;
    bool _italic;
    int _atlasWidth;
    int _atlasHeight;
    
    void readJson(std::string jsonFilename);
    double _textScale;
#if defined(__APPLE__)
    bool _fontItalic ;
    bool _fontBold ;
    //boost::shared_ptr<UserSettingsData> _usettingsdata;
    GLuint _textureToBind;
    //QOpenGLBuffer _arrayBuf;
    //QOpenGLBuffer _indexBuf;
#endif
    boost::shared_array<VertexDataLW> _lineVertices;
};

#endif // OPENGLFONT_H
