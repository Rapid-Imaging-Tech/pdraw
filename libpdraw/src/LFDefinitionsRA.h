//
//  LFDefinitions.h
//  LFClientEngine
//
//  Created by David Dvergsten on 11/30/16.
//  Copyright © 2016 Rapid Imaging Software Inc. All rights reserved.
//
#if defined(__APPLE__)
#else
#include <qvector4d.h>
#include <qvector3d.h>
#include <qvector2d.h>
#endif
#ifndef LFDefinitions_h
#define LFDefinitions_h
//#import <stdlib.h>
//#import <iostream>
//#import <GLKit/GLKit.h>
//#import "ROIManRA.h"
//#import "ElevationTIFFRA.h"
//#import "KmlClientRA.h"
//@interface OpenGLEntity : NSObject
//-(id)initWithRoiMgr:(ROIManRA*)roiMan ElevTiff:(ElevationTIFFRA*)elTiff KMLClient:(KmlClientRA*)kmlClient;
//-(void)filterEntities:(NSDictionary*)filteredEntities;
//@property (atomic, readonly)NSDictionary* filteredEntities;
//@property (atomic, readonly)ROIManRA* roiMan;
//@end
struct VertexDataLW
{
#if defined(__APPLE__)

    double position[3];
    double texCoord[2];
#else
    QVector3D position;
    QVector2D texCoord;
#endif
};
enum OVERLAYTYPE
{
  OT_STREETLINE,
    OT_STREETLABEL,
    OT_STREETSHIELD,
    OT_POINTOFINTEREST
};
enum SHIELDTYPES{USHWY, INTERSTATE, STATEROAD, COUNTYROAD, NONE};
enum MODEOFOPERATION{BASIC, FREE, MDUNKNOWN};
enum LFCLIENTSTATE { WAITINGFORVALIDLOC, COLLECTINGDATA, ASSIGNPLACEMARKELEVS, BUILDINGSYNTH, STATELOADED};
enum
{
    UNIFORM_MODELVIEWPROJECTION_MATRIXRA = 0,
    UNIFORM_NORMAL_MATRIXRA,
    UNIFORM_TEXTURE0_SAMPLER2DRA,
    UNIFORM_TEXTURE1_SAMPLER2DRA,
    UNIFORM_LINECOLORRA,
    NUM_UNIFORMSRA
};

enum {
    ATTRIBUTE_VERTEX,
    ATTRIBUTE_TEXCOORD,
};

//typedef struct{
//    GLKVector3 positionCoords;
//}SceneVertexRA;

//typedef struct{
//    GLKVector3 positionCoords;
//    GLKVector2 textureCoords;
//}SceneVertexLetterRA;


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

#endif /* LFDefinitions_h */



////
////  LFDefinitions.h
////  LFClientEngine
////
////  Created by David Dvergsten on 11/30/16.
////  Copyright © 2016 Rapid Imaging Software Inc. All rights reserved.
////
//#include <qvector4d.h>
//#include <qvector3d.h>
//#include <qvector2d.h>
//#ifndef LFDefinitions_h
//#define LFDefinitions_h
////#import <stdlib.h>
////#import <iostream>
////#import <GLKit/GLKit.h>
////#import "ROIManRA.h"
////#import "ElevationTIFFRA.h"
////#import "KmlClientRA.h"
////@interface OpenGLEntity : NSObject
////-(id)initWithRoiMgr:(ROIManRA*)roiMan ElevTiff:(ElevationTIFFRA*)elTiff KMLClient:(KmlClientRA*)kmlClient;
////-(void)filterEntities:(NSDictionary*)filteredEntities;
////@property (atomic, readonly)NSDictionary* filteredEntities;
////@property (atomic, readonly)ROIManRA* roiMan;
////@end
//struct VertexDataLW
//{
//    QVector3D position;
//    QVector2D texCoord;
//};
//enum SHIELDTYPES{USHWY, INTERSTATE, STATEROAD, COUNTYROAD, NONE};
//enum MODEOFOPERATION{BASIC, FREE, MDUNKNOWN};
//enum LFCLIENTSTATE { WAITINGFORVALIDLOC, COLLECTINGDATA, ASSIGNPLACEMARKELEVS, BUILDINGSYNTH, STATELOADED};
//enum
//{
//    UNIFORM_MODELVIEWPROJECTION_MATRIXRA = 0,
//    UNIFORM_NORMAL_MATRIXRA,
//    UNIFORM_TEXTURE0_SAMPLER2DRA,
//    UNIFORM_TEXTURE1_SAMPLER2DRA,
//    UNIFORM_LINECOLORRA,
//    NUM_UNIFORMSRA
//};

//enum {
//    ATTRIBUTE_VERTEX,
//   	ATTRIBUTE_TEXCOORD,
//};

////typedef struct{
////    GLKVector3 positionCoords;
////}SceneVertexRA;

////typedef struct{
////    GLKVector3 positionCoords;
////    GLKVector2 textureCoords;
////}SceneVertexLetterRA;


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

//#endif /* LFDefinitions_h */
