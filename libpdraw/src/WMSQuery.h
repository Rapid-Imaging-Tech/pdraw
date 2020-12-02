//
// WMSQuery.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2015 Eric Cox for Rapid Imaging Software Inc.
//
#pragma once
#include <string>
enum DATATYPE{NORTHAMERICA, DAE};
class WebMapURLGenerator
{
	//Represents the data necessary to perform a single WMS query, but can be reused
	//after changing values
public:
    std::string _host;
    std::string _request;
    std::string GenerateQueryString(bool locationText);
    void SetBoundingBox(double n, double e, double s, double w);
    void SetSize(int w, int h){ _width = w; _height = h; };
    WebMapURLGenerator(std::string host, std::string url);
    std::string _layer;
    std::string _version;
    std::string _format;
    DATATYPE _dataType;
    int _lod;

    std::string _path;
    std::string _crs;
    std::string _service;
    std::string _style;

	double _starePointLat;
	double _starePointLon;
	int _width, _height;
	double _north, _east, _west, _south;


};
