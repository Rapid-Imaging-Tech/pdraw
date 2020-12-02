//
//  testpng.m
//  SmartCam
//
//  Created by David Dvergsten on 12/7/18.
//  Copyright © 2018 Rapid Imaging Technologies. All rights reserved.
//
#define lfobjc
#import <Foundation/Foundation.h>
#import "objcUtilityWrapper.h"
#import <iostream>
#import <UIKit/UIKit.h>
@implementation LFObjCFunctions
std::map<unsigned char, boost::shared_ptr<CharDimensions> > characterDimensions;
//bool gblFontItalic = false;
//int gblFontHeight = 0;
//int gblFontWidth = 0;
//int gblFontSize = 0;
//std::string gblFontName;
//bool gblFontBold = false;

//load png image given just the filename, this function will find the bundle path and add it
-(GLubyte*)loadPngWithoutPath:(std::string)fname Width:(int&)width Height:(int&)height
{
    NSString *nsfname = [NSString stringWithCString:fname.c_str() encoding:[NSString defaultCStringEncoding]];
    NSString *nsfullpath = [[NSBundle mainBundle] pathForResource:nsfname ofType:@"png"];
    
    UIImage* image = [UIImage imageNamed:nsfullpath];
    width = image.size.width;
    height = image.size.height;
    GLubyte* imageData = (GLubyte*)malloc(image.size.width * image.size.height * 4);
    CGContextRef imageContext = CGBitmapContextCreate(imageData, image.size.width, image.size.height, 8, image.size.width * 4, CGColorSpaceCreateDeviceRGB(), kCGImageAlphaPremultipliedLast);
    CGContextDrawImage(imageContext, CGRectMake(0.0, 0.0, image.size.width, image.size.height), image.CGImage);
    CGContextRelease(imageContext);
    
    return imageData;
    
}
-(GLubyte*)loadPng:(std::string)fname Width:(int&)width Height:(int&)height
{
    NSString *nsFname = [NSString stringWithCString:fname.c_str() encoding:[NSString defaultCStringEncoding]];
    NSString* documentsDirectory2 = [self getDocumentFolderNS];
    NSString *filePath2 = [documentsDirectory2 stringByAppendingPathComponent:nsFname];
    NSLog(@"reading data from %@", filePath2);
    UIImage* image = [UIImage imageNamed:filePath2];
    width = image.size.width;
    height = image.size.height;
    GLubyte* imageData = (GLubyte*)malloc(image.size.width * image.size.height * 4);
    
    CGContextRef imageContext = CGBitmapContextCreate(imageData, image.size.width, image.size.height, 8, image.size.width * 4, CGColorSpaceCreateDeviceRGB(), kCGImageAlphaPremultipliedLast);
    CGContextDrawImage(imageContext, CGRectMake(0.0, 0.0, image.size.width, image.size.height), image.CGImage);
    CGContextRelease(imageContext);
    
    
    return imageData;
}

-(NSString*)getDocumentFolderNS
{
    NSArray *paths2 = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory2 = [paths2 objectAtIndex:0];
    return documentsDirectory2;
}
-(std::string)getDocumentFolder
{
    NSString* nsDocFolder = [self getDocumentFolderNS];
    std::string strFolderPath = std::string([nsDocFolder UTF8String]);
    return strFolderPath;
}
-(GLubyte*)loadUserPng:(std::string)fname Width:(int&)width Height:(int&)height
{
    NSString *nsFname = [NSString stringWithCString:fname.c_str() encoding:[NSString defaultCStringEncoding]];
    NSLog(@"reading data from %@", nsFname);
    UIImage* image = [UIImage imageNamed:nsFname];//TODO: THIS LINE IS A HUGE SLOW DOWN
    width = image.size.width;
    height = image.size.height;
    GLubyte* imageData = (GLubyte*)malloc(image.size.width * image.size.height * 4);
    for(int i = 0 ; i < image.size.width * image.size.height * 4 ; i++)
    {
        imageData[i] = 0x00;
    }
    CGContextRef imageContext = CGBitmapContextCreate(imageData, image.size.width, image.size.height, 8, image.size.width * 4, CGColorSpaceCreateDeviceRGB(),
                                                      kCGImageAlphaPremultipliedLast);
    CGContextDrawImage(imageContext, CGRectMake(0.0, 0.0, image.size.width, image.size.height), image.CGImage);
    CGContextRelease(imageContext);
    
    
    return imageData;
}

-(void)createPngFile:(std::string)fname PtrToData:(unsigned char*)ptrData Size:(size_t)dataSize
{
    NSString *nsFname = [NSString stringWithCString:fname.c_str() encoding:[NSString defaultCStringEncoding]];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        NSString *filePath = [documentsDirectory stringByAppendingPathComponent:nsFname];
    NSLog(@"writing data to %@", filePath);
    NSData* pngData = [NSData dataWithBytes:(const void *)ptrData length:sizeof(unsigned char)*dataSize];
    BOOL result = [pngData writeToFile:filePath atomically:YES];
    NSLog(@"sdf");
}
-(boost::shared_ptr<CharDimensions>) CreateCharDimension:(NSArray*) charAttributes
{
    //int height  = [[charAttributes valueForKey:@"height"] intValue];
    
    int width = [[charAttributes valueForKey:@"width"] intValue];
    int height = [[charAttributes valueForKey:@"height"] intValue];
    
    int advance = [[charAttributes valueForKey:@"advance"] intValue];
    int originX = [[charAttributes valueForKey:@"originX"] intValue];
    int originY = [[charAttributes valueForKey:@"originY"] intValue];
    
    int charx = [[charAttributes valueForKey:@"x"] intValue];
    int chary = [[charAttributes valueForKey:@"y"] intValue];
    boost::shared_ptr<CharDimensions> newCharDim(new CharDimensions(width, height, advance, originX, originY, charx, chary));
    return newCharDim;
}

- (std::map<unsigned char, boost::shared_ptr<CharDimensions> >)loadCharDimensions {
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"arial" ofType:@"json"];//TODO: FIX THIS IN CASE WE USE A DIFFERENT FONT
    NSData *data = [NSData dataWithContentsOfFile:filePath];
    NSArray *json = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:nil];
    _fontItalic  = [[json valueForKey:@"italic"] boolValue];
    _fontHeight  = [[json valueForKey:@"height"] intValue];
    _fontWidth  = [[json valueForKey:@"width"] intValue];
    _fontSize  = [[json valueForKey:@"size"] intValue];
    _fontName = std::string([[json valueForKey:@"name"] UTF8String]);
    _fontBold = [[json valueForKey:@"bold"] boolValue];
    //NSDictionary* dict = [[json valueForKey:@"characters"]dictionaryForKey:@"characters"];
    //NSArray* entries = [json valueForKeyPath:@"characters.}"];
    characterDimensions[' '] = [self CreateCharDimension:[json valueForKeyPath:@"characters. "]];
    
    characterDimensions['}'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.}"]];
    
    //characterDimensions['}'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.}"]];
    characterDimensions['{'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.{"]];
    characterDimensions['('] = [self CreateCharDimension:[json valueForKeyPath:@"characters.("]];
    characterDimensions['['] = [self CreateCharDimension:[json valueForKeyPath:@"characters.["]];
    characterDimensions[']'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.]"]];
    characterDimensions['j'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.j"]];
    characterDimensions['|'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.|"]];
    characterDimensions['$'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.$"]];
    characterDimensions['Q'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.Q"]];
    characterDimensions['&'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.&"]];
    characterDimensions['W'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.W"]];
    characterDimensions['%'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.%"]];
    characterDimensions['A'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.A"]];
    characterDimensions['M'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.M"]];
    characterDimensions['O'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.O"]];
    characterDimensions['V'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.V"]];
    
    characterDimensions['Y'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.Y"]];
    characterDimensions['G'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.G"]];
    characterDimensions['K'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.K"]];
    characterDimensions['R'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.R"]];
    characterDimensions['X'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.X"]];
    characterDimensions['C'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.C"]];
    characterDimensions['B'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.B"]];
    characterDimensions['D'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.D"]];
    characterDimensions['H'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.H"]];
    characterDimensions['N'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.N"]];
    characterDimensions['S'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.S"]];
    characterDimensions['T'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.T"]];
    characterDimensions['U'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.U"]];
    characterDimensions['Z'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.Z"]];
    characterDimensions['#'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.#"]];
    characterDimensions['4'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.4"]];
    characterDimensions['?'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.?"]];
    
    characterDimensions['E'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.E"]];
    characterDimensions['P'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.P"]];
    characterDimensions['y'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.y"]];
    characterDimensions['2'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.2"]];
    characterDimensions['F'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.F"]];
    characterDimensions['L'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.L"]];
    characterDimensions['l'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.l"]];
    characterDimensions['b'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.b"]];
    characterDimensions['d'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.d"]];
    characterDimensions['g'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.g"]];
    characterDimensions['p'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.p"]];
    characterDimensions['q'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.q"]];
    characterDimensions['0'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.0"]];
    characterDimensions['3'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.3"]];
    characterDimensions['5'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.5"]];
    characterDimensions['6'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.6"]];
    characterDimensions['7'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.7"]];
    characterDimensions['8'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.8"]];
    characterDimensions['9'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.9"]];
    characterDimensions['J'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.J"]];
    
    characterDimensions['h'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.h"]];
    characterDimensions['k'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.k"]];
    characterDimensions['f'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.f"]];
    characterDimensions['1'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.1"]];
    characterDimensions['t'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.t"]];
    characterDimensions['/'] = [self CreateCharDimension:[json valueForKeyPath:@"characters./"]];
    characterDimensions['\\'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.\\"]];
    characterDimensions['!'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.!"]];
    characterDimensions[';'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.;"]];
    characterDimensions['i'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.i"]];
    characterDimensions['<'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.<"]];
    characterDimensions['>'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.>"]];
    characterDimensions['m'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.m"]];
    characterDimensions['w'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.w"]];
    characterDimensions['o'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.o"]];
    characterDimensions['v'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.v"]];
    characterDimensions['x'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.x"]];
    characterDimensions['s'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.s"]];
    characterDimensions['a'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.a"]];
    
    characterDimensions['c'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.c"]];
    characterDimensions['e'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.e"]];
    characterDimensions['n'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.n"]];
    characterDimensions['u'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.u"]];
    characterDimensions['z'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.z"]];
    characterDimensions['r'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.r"]];
    characterDimensions[':'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.:"]];
    characterDimensions['+'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.+"]];
    characterDimensions['^'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.^"]];
    characterDimensions['*'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.*"]];
    characterDimensions['='] = [self CreateCharDimension:[json valueForKeyPath:@"characters.="]];
    characterDimensions['\''] = [self CreateCharDimension:[json valueForKeyPath:@"characters.'"]];
    characterDimensions['"'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.\""]];
    characterDimensions['~'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.~"]];
    characterDimensions['`'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.`"]];
    characterDimensions['-'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.-"]];
    //NSLog(@"this is @");
    //NSString* atstr = @"characters.@";
    //NSArray* aptr = [json valueForKey:@"characters"];
    //NSArray* aptr2 = [aptr valueForKey:@"@"];
    //characterDimensions['@'] = [self CreateCharDimension:[json valueForKeyPath:@"characters.-"]];
    //NSObject* obj = [json obj:@"characters.@"] ;
    return characterDimensions;
    
    //CharDimensions* cdp = [self CreateCharDimension:[json valueForKeyPath:@"characters.}"]];
    
    
}
-(std::string)GetBundleFolderPath
{
    NSString *folderPath = [[NSBundle mainBundle]bundlePath];
    
    std::string strFolderPath = std::string([folderPath UTF8String]);
    return strFolderPath;
}

-(std::string)GetDataFolderPath
{
    NSString *folderPath = [[NSBundle mainBundle]bundlePath];
    
    std::string strFolderPath = std::string([folderPath UTF8String]);
    return strFolderPath;
}
//TODOSETTINGS: ADD A GET SETTINGSFILEFULLPATH HERE
-(std::string)getPbfFullPath:(std::string)pbfFilename
{
    std::string DocFolder = [self getDocumentFolder];
    DocFolder.append("/");
    DocFolder.append(pbfFilename);
    //NSString *nsFname = [NSString stringWithCString:pbfFilename.c_str() encoding:[NSString defaultCStringEncoding]];
    //NSLog(@"reading data from %@", nsFname);
    return DocFolder;
}
-(std::string)GetSettingsFilePath
{
    std::string DocFolder = [self getDocumentFolder];
    DocFolder.append("/settings.ini");
    
    //NSString *nsFname = [NSString stringWithCString:pbfFilename.c_str() encoding:[NSString defaultCStringEncoding]];
    //NSLog(@"reading data from %@", nsFname);
    return DocFolder;
}
-(std::string)getTimestamp {
    NSDate *currentTime = [NSDate date];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy/MM/dd HH:mm:ss.SSS"];
    NSString *resultString = [dateFormatter stringFromDate: currentTime];
    std::string strTimeStamp = std::string([resultString UTF8String]);
    return strTimeStamp;
}
@end
std::string LFObjCFunctionsCPPWrapper::GetSettingsFilePath()
{
    LFObjCFunctions* ptrPbf;
    ptrPbf = [LFObjCFunctions new];
    //[ptrPbf getPbfFullPath:pbfFilename];
    return [ptrPbf GetSettingsFilePath];
    //return "";
}

GLubyte* LFObjCFunctionsCPPWrapper::loaduserpng(std::string fname, int& width, int& height)
{
    //std::cout<<"it worked";
    LFObjCFunctions* ptrPng;
    ptrPng = [LFObjCFunctions new];
    return [ptrPng loadUserPng:fname Width:width Height:height];
}
GLubyte* LFObjCFunctionsCPPWrapper::loadpng(std::string fname, int& width, int& height)
{
    //std::cout<<"it worked";
    LFObjCFunctions* ptrPng;
    ptrPng = [LFObjCFunctions new];
    return [ptrPng loadPng:fname Width:width Height:height];
}
std::string LFObjCFunctionsCPPWrapper::GetPbfPath(std::string pbfFilename)
{
    LFObjCFunctions* ptrPbf;
    ptrPbf = [LFObjCFunctions new];
    //[ptrPbf getPbfFullPath:pbfFilename];
    return [ptrPbf getPbfFullPath:pbfFilename];
}
void LFObjCFunctionsCPPWrapper::createPngFile(std::string fname, unsigned char* data, size_t sz)
{
    //std::cout<<"it worked";
    LFObjCFunctions* ptrPng;
    ptrPng = [LFObjCFunctions new];
    [ptrPng createPngFile:fname PtrToData:data Size:sz ];
}

std::map<unsigned char, boost::shared_ptr<CharDimensions> > LFObjCFunctionsCPPWrapper::loadCharDimensions()
{
    LFObjCFunctions* ptrOCUtils;
    ptrOCUtils = [LFObjCFunctions new];
    std::map<unsigned char, boost::shared_ptr<CharDimensions> > charDims = [ptrOCUtils loadCharDimensions];
    this->_fontSize = ptrOCUtils.fontSize;
    this->_fontBold = ptrOCUtils.fontBold;
    this->_fontName = ptrOCUtils.fontName;
    this->_fontWidth = ptrOCUtils.fontWidth;
    this->_fontHeight = ptrOCUtils.fontHeight;
    this->_fontItalic = ptrOCUtils.fontItalic;
    
    return charDims;
}

std::string GetBundleFolderPath()
{
    LFObjCFunctions* ptrOCUtils;
    ptrOCUtils = [LFObjCFunctions new];
    return [ptrOCUtils GetBundleFolderPath];
}

std::string GetDataFolderPath()
{
    //getDocumentFolderNS
    LFObjCFunctions* ptrOCUtils;
    ptrOCUtils = [LFObjCFunctions new];
    return [ptrOCUtils getDocumentFolder];
}

std::string GetSixdofTimeStamp()
{
    LFObjCFunctions* ptrOCUtils;
    ptrOCUtils = [LFObjCFunctions new];
    return [ptrOCUtils getTimestamp];
}

GLubyte* LFObjCFunctionsCPPWrapper::loadPngWithoutPath(std::string fnameNoPath, int& width, int& height)
{
    LFObjCFunctions* ptrOCUtils;
    ptrOCUtils = [LFObjCFunctions new];
    return [ptrOCUtils loadPngWithoutPath:fnameNoPath Width:width Height:height];
}
