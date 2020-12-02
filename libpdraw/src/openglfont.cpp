#include "openglfont.h"
#include <iostream>
#if defined(__APPLE__)
#include "objcUtilityWrapper.h"
#else
#include <qfile.h>
#include <qjsondocument.h>

#include <qjsonarray.h>
#include <qstring.h>
#endif
#include "utilities.h"
using namespace std;
#if defined(__APPLE__)
extern GLuint gbl_u_SamplerLocation ;
extern GLuint gbl_u_SamplerLocationAutoScale ;
extern GLuint gbl_u_mvpLocation ;
extern GLuint gbl_u_colorLocation ;
#endif
openglfont::openglfont( boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger)
    :
    _textScale(1.0),
    _fontName("arial"),
    _fontSize(0),
    _italic(false),
    _atlasWidth(512),
    _atlasHeight(512),
    _bold(false),
#if !defined(__APPLE__)
    _arrayBuf(QOpenGLBuffer::Type::VertexBuffer),
  _indexBuf(QOpenGLBuffer::Type::IndexBuffer),
#endif
  _loadComplete(false)
{
    LoggableConfigurableObject::Init(usettingsData, logger, "OpenglFont");
    //cout<<"create font"<<endl;
}
void openglfont::SetUserSettings(){

    if(!(_userSettingsData->_intSettings.find("TEXTSCALE") == _userSettingsData->_intSettings.end()))
    {
        int l = _userSettingsData->_intSettings["TEXTSCALE"];
        double dl = static_cast<double>(l);
        this->_textScale = dl / 1000.0;
    }
    else{
        this->_textScale = 1.00;
    }
}

openglfont::~openglfont()
{
    cout<<"hit destructor"<<endl;
}
//void openglfont::SetUserSettings(boost::shared_ptr<UserSettingsData> usettings){

////    //this->_usettingsdata = usettings;

////    if(!(_usettingsdata->_intSettings.find("TEXTSCALE") == _usettingsdata->_intSettings.end()))
////    {
////        int l = _usettingsdata->_intSettings["TEXTSCALE"];
////        double dl = static_cast<double>(l);
////        this->_textScale = dl / 1000.0;
////    }
////    else{
////        this->_textScale = 1.00;
////    }

//}
#if !defined(__APPLE__)
int openglfont::getCharAttribute(std::string character, std::string attribute)
{
    //return 5;//todo: remove
     QJsonValue v = _characters[character.c_str()];
     QJsonObject xObj = v.toObject();
     QJsonValue xVal = xObj[attribute.c_str()];
     int retValue = xVal.toInt();
     return retValue;
}

int openglfont::getBufferIndex(QString character)
{
    std::vector<QString>::iterator it = std::find(_vecChars.begin(), _vecChars.end(), character);
    int index = it - _vecChars.begin();
     return index;
}

void openglfont::readJson(std::string jsonFilename)
{

    WriteToStatusLog("reading json file", false, false);
    QString val;
    QFile file;
    std::string errorMsg;
    if(!FileExists(jsonFilename, errorMsg))
    {
        WriteToStatusLog(errorMsg, true, true);
        return ;
    }
    file.setFileName(jsonFilename.c_str());
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject sett2 = d.object();

    QJsonValue value = sett2.value(QString("characters"));
    _characters = value.toObject();
    int xx = getCharAttribute("a", "originX");
    QJsonValue vname = sett2.value(QString("name"));
    _fontName = vname.toString() ;
    QJsonValue vsize = sett2.value(QString("size"));
    _fontSize = vsize.toInt();
    QJsonValue vbold = sett2.value(QString("bold"));
    _bold = vbold.toBool();
    QJsonValue vitalic = sett2.value(QString("italic"));
    _italic = vitalic.toBool();
    QJsonValue vwidth = sett2.value(QString("width"));
    _atlasWidth = vwidth.toInt();
    QJsonValue vheight = sett2.value(QString("height"));
    _atlasHeight = vheight.toInt();
}
#endif
////extern bool FileExists (const std::string& name) ;
//void openglfont::drawText(QOpenGLShaderProgram* program)
//{

//}
#if defined(__APPLE__)
GLuint openglfont::loadTextureWithFilename(std::string filename)
#else
QOpenGLTexture* openglfont::loadTextureWithFilename(std::string filename)
#endif
{
    std::string errorMsg;
    if(!FileExists(filename, errorMsg))
    {
        WriteToStatusLog(errorMsg, true, true);
        std::string rismsg("can't find file ");
        rismsg.append(filename);
        //this->WriteToStatusLog(rismsg, true, true);
    }
#if defined(__APPLE__)
	//QOpenGLTexture* textureRef = new QOpenGLTexture(QImage(filename.c_str()).mirrored());
    // Set bilinear filtering mode for texture minification
    //textureRef->setMinificationFilter(QOpenGLTexture::Linear);
    // Set bilinear filtering mode for texture magnification
    //textureRef->setMagnificationFilter(QOpenGLTexture::Linear);
    LFObjCFunctionsCPPWrapper ocWrapper;
    int width = 0;
    int height = 0;
    
    GLubyte* imageData = ocWrapper.loaduserpng(filename, width, height);
    
    glGenTextures(1, &_textureToBind);
    glBindTexture(GL_TEXTURE_2D, _textureToBind);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    
    // Wrap texture coordinates by repeating
    //textureRef->setWrapMode(QOpenGLTexture::Repeat);
    //textureRef->bind();
    return _textureToBind;
#else
    QOpenGLTexture* textureRef = new QOpenGLTexture(QImage(filename.c_str()).mirrored());
    // Set bilinear filtering mode for texture minification
    textureRef->setMinificationFilter(QOpenGLTexture::Linear);
    // Set bilinear filtering mode for texture magnification
    textureRef->setMagnificationFilter(QOpenGLTexture::Linear);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    // Wrap texture coordinates by repeating
    textureRef->setWrapMode(QOpenGLTexture::Repeat);
    textureRef->bind();
    return textureRef;
#endif
}
int openglfont::GetFontWidth()
{
    return _atlasWidth;
}
int openglfont::GetFontHeight()
{
    return _atlasHeight;
}

int openglfont::GetFontSize()
{
    return _fontSize;
}
#if !defined (__APPLE__)
boost::shared_ptr<CharDimensions> openglfont::CreateCharDimension(std::string character)
{
    int width = getCharAttribute(character, "width");
    int height = getCharAttribute(character, "height");

    int advance = getCharAttribute(character, "advance");
    int originX = getCharAttribute(character, "originX");
    int originY = getCharAttribute(character, "originY");

    int charx = getCharAttribute(character, "x");
    int chary = getCharAttribute(character, "y");
    boost::shared_ptr<CharDimensions> newCharDim(new CharDimensions(width, height, advance, originX, originY, charx, chary));
    return newCharDim;
}
#endif

#if defined(__APPLE__)
void openglfont::LoadCharacterDimensions()
{
    LFObjCFunctionsCPPWrapper objcUtils;
    _characterDimensions = objcUtils.loadCharDimensions();
    _fontItalic = objcUtils._fontItalic;
    _atlasHeight = objcUtils._fontHeight;
    _atlasWidth = objcUtils._fontWidth;
    _fontName = objcUtils._fontName;
    _fontBold = objcUtils._fontBold;
    _fontSize = objcUtils._fontSize;
}
#else
void openglfont::LoadCharacterDimensions()
{

    _characterDimensions[' '] = CreateCharDimension(" ");

    _characterDimensions['@'] = CreateCharDimension("@");
    _characterDimensions['{'] = CreateCharDimension("{");
    _characterDimensions['}'] = CreateCharDimension("}");
    _characterDimensions['('] = CreateCharDimension("(");
    _characterDimensions['['] = CreateCharDimension("[");
    _characterDimensions[']'] = CreateCharDimension("]");
    _characterDimensions['j'] = CreateCharDimension("j");
    _characterDimensions['|'] = CreateCharDimension("|");
    _characterDimensions['$'] = CreateCharDimension("$");
    _characterDimensions['Q'] = CreateCharDimension("Q");
    _characterDimensions['&'] = CreateCharDimension("&");
    _characterDimensions['W'] = CreateCharDimension("W");
    _characterDimensions['%'] = CreateCharDimension("%");
    _characterDimensions['A'] = CreateCharDimension("A");
    _characterDimensions['M'] = CreateCharDimension("M");
    _characterDimensions['O'] = CreateCharDimension("O");
    _characterDimensions['V'] = CreateCharDimension("V");

    _characterDimensions['Y'] = CreateCharDimension("Y");
    _characterDimensions['G'] = CreateCharDimension("G");
    _characterDimensions['K'] = CreateCharDimension("K");
    _characterDimensions['R'] = CreateCharDimension("R");
    _characterDimensions['X'] = CreateCharDimension("X");
    _characterDimensions['C'] = CreateCharDimension("C");
    _characterDimensions['B'] = CreateCharDimension("B");
    _characterDimensions['D'] = CreateCharDimension("D");
    _characterDimensions['H'] = CreateCharDimension("H");
    _characterDimensions['N'] = CreateCharDimension("N");
    _characterDimensions['S'] = CreateCharDimension("S");
    _characterDimensions['T'] = CreateCharDimension("T");
    _characterDimensions['U'] = CreateCharDimension("U");
    _characterDimensions['Z'] = CreateCharDimension("Z");
    _characterDimensions['#'] = CreateCharDimension("#");
    _characterDimensions['4'] = CreateCharDimension("4");
    _characterDimensions['?'] = CreateCharDimension("?");

    _characterDimensions['E'] = CreateCharDimension("E");
    _characterDimensions['P'] = CreateCharDimension("P");
    _characterDimensions['y'] = CreateCharDimension("y");
    _characterDimensions['2'] = CreateCharDimension("2");
    _characterDimensions['F'] = CreateCharDimension("F");
    _characterDimensions['L'] = CreateCharDimension("L");
    _characterDimensions['b'] = CreateCharDimension("b");
    _characterDimensions['d'] = CreateCharDimension("d");
    _characterDimensions['g'] = CreateCharDimension("g");
    _characterDimensions['p'] = CreateCharDimension("p");
    _characterDimensions['q'] = CreateCharDimension("q");
    _characterDimensions['0'] = CreateCharDimension("0");
    _characterDimensions['3'] = CreateCharDimension("3");
    _characterDimensions['5'] = CreateCharDimension("5");
    _characterDimensions['6'] = CreateCharDimension("6");
    _characterDimensions['7'] = CreateCharDimension("7");
    _characterDimensions['8'] = CreateCharDimension("8");
    _characterDimensions['9'] = CreateCharDimension("9");
    _characterDimensions['J'] = CreateCharDimension("J");

    _characterDimensions['h'] = CreateCharDimension("h");
    _characterDimensions['k'] = CreateCharDimension("k");
    _characterDimensions['f'] = CreateCharDimension("f");
    _characterDimensions['1'] = CreateCharDimension("1");
    _characterDimensions['l'] = CreateCharDimension("l");
    _characterDimensions['t'] = CreateCharDimension("t");
    _characterDimensions['/'] = CreateCharDimension("/");
    _characterDimensions['\\'] = CreateCharDimension("\\");
    _characterDimensions['!'] = CreateCharDimension("!");
    _characterDimensions[';'] = CreateCharDimension(";");
    _characterDimensions['i'] = CreateCharDimension("i");
    _characterDimensions['<'] = CreateCharDimension("<");
    _characterDimensions['>'] = CreateCharDimension(">");
    _characterDimensions['m'] = CreateCharDimension("m");
    _characterDimensions['w'] = CreateCharDimension("w");
    _characterDimensions['o'] = CreateCharDimension("o");
    _characterDimensions['v'] = CreateCharDimension("v");
    _characterDimensions['x'] = CreateCharDimension("x");
    _characterDimensions['s'] = CreateCharDimension("s");
    _characterDimensions['a'] = CreateCharDimension("a");

    _characterDimensions['c'] = CreateCharDimension("c");
    _characterDimensions['e'] = CreateCharDimension("e");
    _characterDimensions['n'] = CreateCharDimension("n");
    _characterDimensions['u'] = CreateCharDimension("u");
    _characterDimensions['z'] = CreateCharDimension("z");
    _characterDimensions['r'] = CreateCharDimension("r");
    _characterDimensions[':'] = CreateCharDimension(":");
    _characterDimensions['+'] = CreateCharDimension("+");
    _characterDimensions['^'] = CreateCharDimension("^");
    _characterDimensions['*'] = CreateCharDimension("*");
    _characterDimensions['='] = CreateCharDimension("=");
    _characterDimensions['\''] = CreateCharDimension("'");
    _characterDimensions['"'] = CreateCharDimension("\"");
    _characterDimensions['~'] = CreateCharDimension("~");
    _characterDimensions['`'] = CreateCharDimension("`");
    _characterDimensions['-'] = CreateCharDimension("-");

}
#endif
bool openglfont::getLetterTextureDimensions(unsigned char cc, double& lowerNormalizedx, double& lowerNormalizedy, double& upperNormalizedx,
                                            double& upperNormalizedy, int& width, int& height, int& advance, int& originX, int& originY,
                                            int& charx, int& chary)
{

    bool found = false;

    std::string cstr(1, cc);

    //TODO: GRAB THIS FROM A MAP OR ARRAY THAT'S ALL LOADED FROM STARTUP, DONT USE GETCHARATTRIBUTE,
    // OR AT LEAST REWRITE THAT FUNCTIONI IT IS SLOW!!
    //int x = getCharAttribute(cstr, "x");
    //int y = getCharAttribute(cstr, "y");
    if ( _characterDimensions.find(cc) == _characterDimensions.end() )
    {
      // not found
        width = 0;
        width = 0;
        height = 0;

        advance = 0;
        originX = 0;
        originY = 0;

        charx = 0;
        chary = 0;
        found = false;
    } else {
      // found
        boost::shared_ptr<CharDimensions> charDims = _characterDimensions[cc];
        width = charDims->_width;
        //width = getCharAttribute(cstr, "width");
        //height = getCharAttribute(cstr, "height");
        height = charDims->_height;

        //advance = getCharAttribute(cstr, "advance");
        advance = charDims->_advance;
        //originX = getCharAttribute(cstr, "originX");
        originX = charDims->_originX;
        //originY = getCharAttribute(cstr, "originY");
        originY = charDims->_originY;

        //charx = getCharAttribute(cstr, "x");
        charx = charDims->_charx;
        //chary = getCharAttribute(cstr, "y");
        chary = charDims->_chary;
        found = true;
    }
    return found;
}

void openglfont::generateTextBuffer(std::string text)
{
}
void openglfont::bindFont()
{
#if defined(__APPLE__)
 	glActiveTexture ( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, _textureToBind);
    
    // Set the base map sampler to texture unit to 0
    glUniform1i ( gbl_u_SamplerLocationAutoScale, 0 );
#else
    _textureToBind->bind();
#endif
}

void openglfont::loadFont(std::string fontPngPath, std::string jsonPath)
{
    //TODO: ON STARTUP, LOAD A MAP OR ARRAY WITH ALL THE CHARACTER DIMENSIONS
    // SO WE CAN DO A QUICKER LOOKUP AT RUNTIME.

    _textScale = 50.0;
#if !defined(__APPLE__)
    readJson(jsonPath);
#endif
    //
    _textureToBind = loadTextureWithFilename(fontPngPath );
     LoadCharacterDimensions();
     _loadComplete = true;
}
