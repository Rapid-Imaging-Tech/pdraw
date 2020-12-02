//  This software is Copyrighted (c) by Michael F. Abernathy, 1996-2016
//  All rights reserved.  No part of this software may be used without the
//  written consent of the author.
#pragma once

struct FootPrint {
public:
	double fpLat[4],fpLon[4],fpAlt[4];
	int fpValid; // 0 invalid, 1, partially valid, 4 all valid
	int MouseX,MouseY;
	double MouseLat,MouseLon, MouseAlt, SPLat, SPLon;
};