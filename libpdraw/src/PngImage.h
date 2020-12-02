#pragma once
#include <iostream>
#include <string>
#include <GL/gl.h>
#include "boost/shared_array.hpp"
#include "/usr/include/png.h"
#define TEXTURE_LOAD_ERROR 0

using namespace std;
class PngImage
{
public:
	PngImage();
	bool LoadTexture(const string filename);//, int &width, int &height);
	GLuint GetTexture(){return _textureNum; } ;
	boost::shared_array<unsigned char> GetImage(){ return this->_imageData; };
	int GetWidth(){ return _width; } ;
	int GetHeight(){return _height; } ;
	void SetName(std::string name){this->_name = name; } ;
	void SetTexture(GLuint tex){_textureNum = tex; } ;
private:
    png_byte *_image_data;
	//GLuint _texture;
	std::string _name;
	boost::shared_array<unsigned char> _imageData;
	GLuint _textureNum;
	int _width;
	int _height;
};
