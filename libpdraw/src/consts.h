//  This software is Copyrighted (c) by Michael F. Abernathy, 1996
//  All rights reserved.  No part of this software may be used without the
//  written consent of the author.
#ifndef CONSTSDEFD

#define EarthRadius 6370997
#define PI180   0.0174532925
#define PINV    57.29577951
#define PI     3.1415926536
#define Half_Pi 1.5707963268            //
#define Two_Pi 6.28318530718            //

#define DegreeNorthInMeters 110574.2727
#define DegreeWestInMeters 111319.458

#define NODEMCODE -0.123456789
#define Feet2Meters 3.2808399
#define NauticalMiles2Meters 1852.0
#define    rad_deg    57.2957795131            //    from radians to degres
#define deg_rad 1.74532925199e-2        //    from degres to radians

#define VSMULT(a,b,c) a[0]=b[0]*(c);a[1]=b[1]*(c);a[2]=b[2]*(c);

#define VADD(a,b,c) a[0]=b[0]+c[0];a[1]=b[1]+c[1];a[2]=b[2]+c[2];

#define VSUB(a,b,c) a[0]=b[0]-c[0];a[1]=b[1]-c[1];a[2]=b[2]-c[2];

#define VREPL(a,b) a[0]=b[0];a[1]=b[1];a[2]=b[2];

#define VZERO(a) a[0]=0.0;a[1]=0.0;a[2]=0.0;

#define VVMULT(a,b,c) a[0]=b[0]*c[0];a[1]=b[1]*c[1];a[2]=b[2]*c[2];

#define NEW_POINT(a,b,c,d) a[0]=b[0]+c[0]*d;a[1]=b[1]+c[1]*d;a[2]=b[2]+c[2]*d;

#define DOTPR(a,b) (a[0]*b[0]+a[1]*b[1]+a[2]*b[2])

#define NEGVECT(a) a[0]=-a[0];a[1]=-a[1];a[2]=-a[2];

#define CONSTSDEFD 1
const double DEM_LAT_PIXEL_RES = 1200;
const int IMAGE_TILE_SIZE = 512;

#endif


////  This software is Copyrighted (c) by Michael F. Abernathy, 1996
////  All rights reserved.  No part of this software may be used without the
////  written consent of the author.
//#ifndef CONSTSDEFD

//#define EarthRadius 6370997
//#define PI180   0.0174532925
//#define PINV    57.29577951
//#define PI 	3.1415926536
//#define Half_Pi 1.5707963268			//
//#define Two_Pi 6.28318530718			//

//#define DegreeNorthInMeters 110574.2727
//#define DegreeWestInMeters 111319.458

//#define NODEMCODE -0.123456789
//#define Feet2Meters 3.2808399
//#define NauticalMiles2Meters 1852.0
//#define	rad_deg	57.2957795131			//	from radians to degres
//#define deg_rad 1.74532925199e-2		//	from degres to radians

//#define VSMULT(a,b,c) a[0]=b[0]*(c);a[1]=b[1]*(c);a[2]=b[2]*(c);

//#define VADD(a,b,c) a[0]=b[0]+c[0];a[1]=b[1]+c[1];a[2]=b[2]+c[2];

//#define VSUB(a,b,c) a[0]=b[0]-c[0];a[1]=b[1]-c[1];a[2]=b[2]-c[2];

//#define VREPL(a,b) a[0]=b[0];a[1]=b[1];a[2]=b[2];

//#define VZERO(a) a[0]=0.0;a[1]=0.0;a[2]=0.0;

//#define VVMULT(a,b,c) a[0]=b[0]*c[0];a[1]=b[1]*c[1];a[2]=b[2]*c[2];

//#define NEW_POINT(a,b,c,d) a[0]=b[0]+c[0]*d;a[1]=b[1]+c[1]*d;a[2]=b[2]+c[2]*d;

//#define DOTPR(a,b) (a[0]*b[0]+a[1]*b[1]+a[2]*b[2])

//#define NEGVECT(a) a[0]=-a[0];a[1]=-a[1];a[2]=-a[2];

//#define CONSTSDEFD 1
//const double DEM_LAT_PIXEL_RES = 1200;
//const int IMAGE_TILE_SIZE = 512;

//#endif
