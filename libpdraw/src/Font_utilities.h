//=====================================================================================================
//
//	Miscellaneous functions ... header file
//
//=====================================================================================================


//#if defined(CONFIG_FREETYPE)
#ifndef FONT_UTILITIES_H
#define FONT_UTILITIES_H

#if defined(WIN_32)
#include <windows.h>	//	Standart libraries
#endif // defined(WIN_32)

//#include <gl\gl.h>		// Open GL libraries
//#include <gl\glu.h>		//

void PrintString(int, int, char*);  // Print a string into LandForm window
void PrintFreeTypeString(double,double,double,char*);
#endif
//#endif // defined(CONFIG_FREETYPE)
