// DTED object hold DTED file data.


#ifndef    DTEDtype
#include <stdio.h>

#include<string> // for string class
#include "LoggableObject.h"
using namespace std;

#define     YMAX     16384          /*  max length of an NCIC scan line */
#define     XMAX     16384          /*  max # of NCIC scan lines */

#define SW     0
#define NW     1
#define NE     2
#define SE     3


// Little file name generation utility
void MakeDTED1Name(string &name, double Lat, double Lon);

// we're going to build on the existing DEM functionality
class  dted_ops  : public LoggableConfigurableObject
{

public:
    char uhl[80];
    char dsi[648];
    char acc[2704];
    unsigned char elevation_record[15000];// leave a lil extra room
    int DesignatorLevel;
    char *memptr;
    int memoffset;

    dted_ops(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger);
    ~dted_ops();
    bool  dted_read(string name);
    bool  dted_read_compressed(string name);
    void LoadDEMAttributes();
    // Member function (method) to load an individual record
    // to memory
    void  store_record_to_memory(int record);
    void SetGMap();
    float GetElevation(double lat, double lon);


    int *                ActiveIndices;
    unsigned int                rowCount, columnCount, r, c;
    double                eastMost, westMost, southMost, northMost;
    unsigned int                eastMostSample,  westMostSample,
    southMostSample, northMostSample;
    float**                Elevations;
    unsigned int                planeUnitOfMeasure, elevUnitOfMeasure, polygonSizes;
    //        virtual double        GetElevation(int x, int y);
    int                DEMlevel, elevationPattern, groundSystem, groundZone;
    int                    iMin, iMax, jMin, jMax;    // index limits of this dem
    // with respect to a given ROI
    double                Lat1, Lat2, Lon1, Lon2; // not serialized, but initialized on deserialization
    string                mapLabel;
    double                projectParams[15];

    double                spatialResolution[3];
    int                    activeNx, activeNy;
    double                groundCoords[4][2], elevBounds[2], localRotation;
    int                    usePolyReduction;
    double                verticalScale;      /* to stretch or shrink elevations */
    //        GraphMap            GMap;  // not serialized, but initialized on deserialization



private:
    virtual void SetUserSettings();
    //        virtual void        getheader(FILE *fp);
    //        char*                Render(int type);
    //        virtual int            processProfiles(FILE *fp);
    //        virtual int            process75Profiles(FILE *fp);
    unsigned char        GetByte(int x, int y);
    unsigned char        GetByteSlope(int x, int y);

    // attributes
    long                ArraySize;
    unsigned char *        bitmap;
    int                base[XMAX];         /* 1 profile of base elevations */
    unsigned int                accuracyCode;
    unsigned int                profileDimension[2];
    unsigned int                firstRow, lastRow;
    unsigned int                wcount;
    double                deltaY, Convert, maxDelta;

    unsigned int               cellCount;
    unsigned int                rowStr,rowEnd, colStr, colEnd,
    colInt, rowInt, outType;
};

// Information Per Cell (File - read out once)
#define UHL_LENGTH                             80
#define DSI_LENGTH                             648
#define ACC_LENGTH                             2700


// Information Per Record
#define RECORD_LENGTH_DT1                  2414
#define RECORD_LENGTH_DT2                  7214
#define DTEDtype
#endif


//// DTED object hold DTED file data.


//#ifndef	DTEDtype
//#include <stdio.h>

//#include<string> // for string class
//#include "LoggableObject.h"
//using namespace std;

//#define     YMAX     16384          /*  max length of an NCIC scan line */
//#define     XMAX     16384          /*  max # of NCIC scan lines */

//#define SW     0
//#define NW     1
//#define NE     2
//#define SE     3


//// Little file name generation utility
//void MakeDTED1Name(string &name, double Lat, double Lon);

//// we're going to build on the existing DEM functionality
//class  dted_ops  : public LoggableConfigurableObject
//{

//       public:
//        char uhl[80];
//        char dsi[648];
//        char acc[2704];
//        unsigned char elevation_record[15000];// leave a lil extra room
//		int DesignatorLevel;
//        char *memptr;
//        int memoffset;

//        dted_ops(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger);
//        ~dted_ops();
//        bool  dted_read(string name);
//        bool  dted_read_compressed(string name);
//		void LoadDEMAttributes();
//        // Member function (method) to load an individual record
//        // to memory
//        void  store_record_to_memory(int record);
//        void SetGMap();
//        float GetElevation(double lat, double lon);


//        int *				ActiveIndices;
//        unsigned int				rowCount, columnCount, r, c;
//        double				eastMost, westMost, southMost, northMost;
//        unsigned int				eastMostSample,  westMostSample,
//                            southMostSample, northMostSample;
//        float**				Elevations;
//        unsigned int				planeUnitOfMeasure, elevUnitOfMeasure, polygonSizes;
////        virtual double		GetElevation(int x, int y);
//        int				DEMlevel, elevationPattern, groundSystem, groundZone;
//        int					iMin, iMax, jMin, jMax;	// index limits of this dem
//                                                // with respect to a given ROI
//        double				Lat1, Lat2, Lon1, Lon2; // not serialized, but initialized on deserialization
//        string				mapLabel;
//        double				projectParams[15];

//        double				spatialResolution[3];
//        int					activeNx, activeNy;
//        double				groundCoords[4][2], elevBounds[2], localRotation;
//        int					usePolyReduction;
//        double				verticalScale;      /* to stretch or shrink elevations */
////        GraphMap			GMap;  // not serialized, but initialized on deserialization



//    private:
//    virtual void SetUserSettings();
////        virtual void		getheader(FILE *fp);
////        char*				Render(int type);
////        virtual int			processProfiles(FILE *fp);
////        virtual int			process75Profiles(FILE *fp);
//        unsigned char		GetByte(int x, int y);
//        unsigned char		GetByteSlope(int x, int y);

//        // attributes
//        long				ArraySize;
//        unsigned char *		bitmap;
//        int				base[XMAX];         /* 1 profile of base elevations */
//        unsigned int				accuracyCode;
//        unsigned int				profileDimension[2];
//        unsigned int				firstRow, lastRow;
//        unsigned int				wcount;
//        double				deltaY, Convert, maxDelta;

//        unsigned int       		cellCount;
//        unsigned int				rowStr,rowEnd, colStr, colEnd,
//                            colInt, rowInt, outType;
//};

//// Information Per Cell (File - read out once)
//#define UHL_LENGTH                             80
//#define DSI_LENGTH                             648
//#define ACC_LENGTH                             2700


//// Information Per Record
//#define RECORD_LENGTH_DT1                  2414
//#define RECORD_LENGTH_DT2                  7214
//#define DTEDtype
//#endif

