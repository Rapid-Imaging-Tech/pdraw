//
//  testPng.h
//  SmartCam
//
//  Created by David Dvergsten on 12/7/18.
//  Copyright © 2018 Rapid Imaging Technologies. All rights reserved.
//

#ifndef testPng_h
#define testPng_h
#include <iostream>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#include <map>
#include "boost/shared_ptr.hpp"
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
#ifdef lfobjc
@interface LFObjCFunctions : NSObject
-(GLubyte*)loadPng:(std::string)fname Width:(int&)width Height:(int&)height;
-(GLubyte*)loadUserPng:(std::string)fname Width:(int&)width Height:(int&)height;
-(void)createPngFile:(std::string)fname PtrToData:(unsigned char*)ptrData Size:(size_t)dataSize;
-(std::map<unsigned char, boost::shared_ptr<CharDimensions> >)loadCharDimensions;
-(std::string)GetBundleFolderPath;
-(std::string)getPbfFullPath:(std::string)pbfFilename;
-(GLubyte*)loadPngWithoutPath:(std::string)fname Width:(int&)width Height:(int&)height;
-(std::string)GetSettingsFilePath;
-(std::string)getTimestamp ;
@property (atomic, readonly) NSMutableData *pngData;
@property (atomic, readonly) bool fontItalic ;
@property (atomic, readonly)  int fontHeight ;
@property (atomic, readonly)  int fontWidth ;
@property (atomic, readonly)  int fontSize;
@property (atomic, readonly)  std::string fontName;
@property (atomic, readonly)  bool fontBold ;

@end
#endif
std::string GetDataFolderPath();
std::string GetBundleFolderPath();
std::string GetSixdofTimeStamp();
class LFObjCFunctionsCPPWrapper{
public:
    std::string GetSettingsFilePath();
    GLubyte* loadPngWithoutPath(std::string fnameNoPath, int& width, int& height);
    std::string GetPbfPath(std::string pbfFilename);//grab filename + path to directory in ios
    LFObjCFunctionsCPPWrapper(){};
    GLubyte* loadpng(std::string fname, int& width, int& height);
    GLubyte* loaduserpng(std::string fname, int& width, int& height);
    void createPngFile(std::string fname, unsigned char* data, size_t sz);
    std::map<unsigned char, boost::shared_ptr<CharDimensions> > loadCharDimensions();
    bool GetFontItalic() {return _fontItalic;};
    int GetFontHeight() {return _fontHeight;};
    int GetFontWidth() {return _fontWidth;};
    int GetFontSize() {return _fontSize;};
    std::string GetFontName(){return _fontName;};
    bool GetFontBold (){return _fontBold;};
    //should eventually be private and create some accessors
    bool _fontItalic ;
    int _fontHeight ;
    int _fontWidth ;
    int _fontSize;
    std::string _fontName;
    bool _fontBold ;
private:
    
};
#endif /* testPng_h */
