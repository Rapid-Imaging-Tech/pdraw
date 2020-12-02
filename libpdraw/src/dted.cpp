// DTED object hold DTED file data.

//#include "MessageManager.h"

#include "dted.h"
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

// Constructor
void dted_ops::SetUserSettings()
{

}

dted_ops :: dted_ops(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger)
{
    LoggableConfigurableObject::Init(usettingsData, logger, "dted_ops");
    Elevations = NULL;
    elevUnitOfMeasure = 2;
    spatialResolution[0] = 0;
    spatialResolution[1] = 0;
    spatialResolution[2] = 1.0;
    Lat1 = Lat2 = Lon1 = Lon2 = -0.0; // this setting lets us know if we have an uninitialized dt
}

dted_ops :: ~dted_ops()
{
}

/*---------------------------------------------------------------*/

bool  dted_ops :: dted_read(std::string cell_name)
{

    char temp_buff[80];
    int  memoffset, i, j, bytes_read, ilatitude, num_records, recLen;
    //    short * els = (short *) &(elevation_record[9]);
    FILE *fp;
    memoffset = 0;
    //   const char *str = cell_name.c_str();

    if (cell_name.find(".dt1") > 0)
    {
        int nn = cell_name.length();
        string compressedName = cell_name.substr(0,nn-4);
        string ap = ".dt1";
        compressedName.append(ap);  //replace(".dt1",".cdt");
        return this->dted_read_compressed(cell_name);
    }

    if ((fp = fopen( cell_name.c_str(), "rb")) == NULL)
    {
        {
            return false;
        }

    }

    bytes_read = (int)fread( this->uhl, 1, UHL_LENGTH, fp);
    bytes_read = (int)fread( this->dsi, 1, DSI_LENGTH, fp);
    bytes_read = (int)fread( this->acc, 1, ACC_LENGTH, fp);

    // compute the amount of data to be read
    //   strcpy( temp_buff, cell_name);
    strncpy(temp_buff,&(uhl[12]),3);
    temp_buff[3] = '\0';
    ilatitude = atoi( &temp_buff[0]);
    // Now we load the variables in the base class DEM
    // just so that a DTED will behave like a DEM, thru inheritance
    LoadDEMAttributes();
    if (DesignatorLevel == 1) recLen = RECORD_LENGTH_DT1;
    else recLen = RECORD_LENGTH_DT2;
    if (DesignatorLevel == 1) {
        if ((ilatitude >= 0) && (ilatitude < 50))
            num_records = 1201;            // Zone 1, 3 arcsecond separation
        else if ((ilatitude >= 50) && (ilatitude < 70))
            num_records = 601;             // Zone 2, 6 arcsecond separation
        else if ((ilatitude >= 70) && (ilatitude < 75))
            num_records = 401;             // Zone 3, 9 arcsecond separation
        else if ((ilatitude >= 75) && (ilatitude < 80))
            num_records = 301;             // Zone 4, 12 arcsecond separation
        else if ((ilatitude >= 80) && (ilatitude < 90))
            num_records = 201;
    } else if (DesignatorLevel == 2) {
        if ((ilatitude >= 0) && (ilatitude < 50))
            num_records = 3601;            // Zone 1, 3 arcsecond separation
        else if ((ilatitude >= 50) && (ilatitude < 70))
            num_records = 1801;             // Zone 2, 6 arcsecond separation
        else if ((ilatitude >= 70) && (ilatitude < 75))
            num_records = 1201;             // Zone 3, 9 arcsecond separation
        else if ((ilatitude >= 75) && (ilatitude < 80))
            num_records = 901;             // Zone 4, 12 arcsecond separation
        else if ((ilatitude >= 80) && (ilatitude < 90))
            num_records = 601;
    }
    // Zone 5, 18 arcsecond separation
    // Allocate enough memory to store all records of data - not needed - use
    // MFC collection class instead
    //memptr = (char *) malloc( RECORD_LENGTH * num_records);
    //
    if ((Elevations = new float*[rowCount]) == NULL){
        //        cout << "Allocation for Elevations failed. \n";
        return false;
    } else {
        // Allocate space for the rows
        for (i = 0; i < (signed)rowCount; i++) {
            if ((Elevations[i] = new float[columnCount]) == NULL)    {//If failure, release memory already allocated
                for (j = (i - 1); j >= 0; j--){
                    delete [] Elevations[j];
                }
                delete [] Elevations;
                Elevations = NULL;
                //                cout << "Allocation for rows of Elevations failed.\n";
                return false;
            }
        }
    }

    for (i = 0; i < num_records; i ++) {

        bytes_read = (int)fread( elevation_record, 1, recLen, fp);

        if (bytes_read == recLen)
            store_record_to_memory(i);
        else {
            //             spMsgMgr()->PostInfo("A full record could not be read\n","Error in DTED ");
            fclose(fp);
            return false;
        }

        // This is a debug, just want to see if correct info is there
        // Section replaced - use MSVC++ debugger
    }
    fclose( fp);
    return true;
}

bool  dted_ops :: dted_read_compressed(string cell_name)
{

    char temp_buff[80];
    int  memoffset, i, j, bytes_read, ilatitude, num_records, recLen;
    //    short * els = (short *) &(elevation_record[9]);
    gzFile gzFp;
    memoffset = 0;
    const char *nm = cell_name.c_str();
    if ((gzFp = gzopen(nm,"rb")) == NULL)
    {
        //             MessageBox(NULL,    "Cannot open file","Error -- ",MB_OK);
        return false;
    }

    bytes_read = (int)gzread( gzFp, this->uhl, UHL_LENGTH );
    bytes_read = (int)gzread( gzFp, this->dsi,  DSI_LENGTH);
    bytes_read = (int)gzread( gzFp, this->acc,  ACC_LENGTH);

    // compute the amount of data to be read
    //   strcpy( temp_buff, cell_name);
    strncpy(temp_buff,&(uhl[12]),3);
    temp_buff[3] = '\0';
    ilatitude = atoi( &temp_buff[0]);
    // Now we load the variables in the base class DEM
    // just so that a DTED will behave like a DEM, thru inheritance
    LoadDEMAttributes();
    if (DesignatorLevel == 1) recLen = RECORD_LENGTH_DT1;
    else recLen = RECORD_LENGTH_DT2;
    if (DesignatorLevel == 1) {
        if ((ilatitude >= 0) && (ilatitude < 50))
            num_records = 1201;            // Zone 1, 3 arcsecond separation
        else if ((ilatitude >= 50) && (ilatitude < 70))
            num_records = 601;             // Zone 2, 6 arcsecond separation
        else if ((ilatitude >= 70) && (ilatitude < 75))
            num_records = 401;             // Zone 3, 9 arcsecond separation
        else if ((ilatitude >= 75) && (ilatitude < 80))
            num_records = 301;             // Zone 4, 12 arcsecond separation
        else if ((ilatitude >= 80) && (ilatitude < 90))
            num_records = 201;
    } else if (DesignatorLevel == 2) {
        if ((ilatitude >= 0) && (ilatitude < 50))
            num_records = 3601;            // Zone 1, 3 arcsecond separation
        else if ((ilatitude >= 50) && (ilatitude < 70))
            num_records = 1801;             // Zone 2, 6 arcsecond separation
        else if ((ilatitude >= 70) && (ilatitude < 75))
            num_records = 1201;             // Zone 3, 9 arcsecond separation
        else if ((ilatitude >= 75) && (ilatitude < 80))
            num_records = 901;             // Zone 4, 12 arcsecond separation
        else if ((ilatitude >= 80) && (ilatitude < 90))
            num_records = 601;
    }
    // Zone 5, 18 arcsecond separation
    // Allocate enough memory to store all records of data - not needed - use
    // MFC collection class instead
    //memptr = (char *) malloc( RECORD_LENGTH * num_records);
    //
    if ((Elevations = new float*[rowCount]) == NULL){
        //        cout << "Allocation for Elevations failed. \n";
        return false;
    } else {
        // Allocate space for the rows
        for (i = 0; i < (signed)rowCount; i++) {
            if ((Elevations[i] = new float[columnCount]) == NULL)    {//If failure, release memory already allocated
                for (j = (i - 1); j >= 0; j--){
                    delete [] Elevations[j];
                }
                delete [] Elevations;
                Elevations = NULL;
                //                cout << "Allocation for rows of Elevations failed.\n";
                return false;
            }
        }
    }

    for (i = 0; i < num_records; i ++) {

        bytes_read = (int)gzread(gzFp, elevation_record, recLen);

        if (bytes_read == recLen)
            store_record_to_memory(i);
        else {
            //             spMsgMgr()->PostInfo("A full record could not be read\n","Error in DTED ");
            gzclose(gzFp);
            return false;
        }

        // This is a debug, just want to see if correct info is there
        // Section replaced - use MSVC++ debugger
    }
    gzclose(gzFp);
    /*  */
    return true;
}
void  dted_ops :: store_record_to_memory(int record)
{
    int i,j;
    short a, hibyte, lobyte;
    //    short * els = (short *) &(elevation_record[8]);
    //    CWordArray *cw = new CWordArray();
    //    cw->SetSize(rowCount+1,16);
    // Copy the record into memory
    // memcpy( memptr + memoffset, elevation_record, RECORD_LENGTH);
    // WES HELP this is where I swap the bytes to make the integer but it does not
    // handle a negative value
    for (i=0; i<((int) rowCount); i++) {
        j = i + i + 8;
        hibyte = elevation_record[j];
        lobyte = elevation_record[j+1];
        a = (hibyte << 8) & 0xFF00;
        a = a + lobyte;
        if (a < 0) {
            a = ~a | 0x8000;
            a = a + 1;
        }
        //        cw->SetAt(i,a);
        Elevations[i][record] = a;
    }
    //     Elevations.Add(cw);
    // Increment the offset so we keep moving along through memory
    // memoffset += RECORD_LENGTH;
}
void dted_ops::LoadDEMAttributes() {
    int i;
    int *pi;
    string Uhl = uhl;
    string Dsi = &(dsi[204]); // converting to CString just makes them easier to handle
    // let us set the max and min elevations to something sensable
    
    
    elevBounds[0] = -300.0;
    elevBounds[1] = 10000.0;
    mapLabel = uhl;
    verticalScale = 1.0;
    char cc = dsi[63];
    char cp[128];
    cp[0] = cc;
    cp[1] = 0 ;// terminate string
    string str = cp;
    DesignatorLevel = atoi(str.c_str()); //atoi(Dsi.substr(63,1));
    
    columnCount = atoi(Dsi.substr(81,4).c_str());
    rowCount = atoi(Dsi.substr(77,4).c_str());
    Lon1 = atoi(Dsi.substr(7,3).c_str());
    Lat1 = atoi(Dsi.substr(0,2).c_str());
    Lon2 = atoi(Dsi.substr(37,3).c_str());
    Lat2 = atoi(Dsi.substr(30,2).c_str());
    
    if ( this->dsi[218] != 'E') Lon1= -Lon1;
    if ( this->dsi[210] != 'N') Lat1= -Lat1;
    if ( this->dsi[248] != 'E') Lon2= -Lon2;
    if ( this->dsi[240] != 'N') Lat2= -Lat2;
    groundSystem = 0; // for dted level 1
    
    if (Elevations != NULL) {
        for (i = 0; i < (signed)rowCount; i++) {
            delete [] Elevations[i];
        }
        delete [] Elevations;
        Elevations = NULL;
    }
    //    SetGMap();
    
}
//void dted_ops::LoadDEMAttributes() {
//    int i;
//    string Uhl = uhl;
//    string Dsi = dsi; // converting to CString just makes them easier to handle
//    // let us set the max and min elevations to something sensable
//
//
//    elevBounds[0] = -300.0;
//    elevBounds[1] = 10000.0;
//    mapLabel = uhl;
//    verticalScale = 1.0;
//
//    DesignatorLevel = atoi(Dsi.substr(63,1).c_str()); //atoi(Dsi.substr(63,1));
//    columnCount = atoi(Dsi.substr(285,4).c_str());
//    rowCount = atoi(Dsi.substr(281,4).c_str());
//    Lon1 = atoi(Dsi.substr(211,3).c_str());
//    Lat1 = atoi(Dsi.substr(204,2).c_str());
//    Lon2 = atoi(Dsi.substr(241,3).c_str());
//    Lat2 = atoi(Dsi.substr(234,2).c_str());
//    if ( Dsi.substr(218,1) != "E") Lon1= -Lon1;
//    if ( Dsi.substr(210,1) != "N") Lat1= -Lat1;
//    if ( Dsi.substr(248,1) != "E") Lon2= -Lon2;
//    if ( Dsi.substr(240,1) != "N") Lat2= -Lat2;
//    groundSystem = 0; // for dted level 1
//
//    if (Elevations != NULL) {
//        for (i = 0; i < (signed)rowCount; i++) {
//            delete [] Elevations[i];
//        }
//        delete [] Elevations;
//        Elevations = NULL;
//    }
//    //    SetGMap();
//
//}
//
//void dted_ops::SetGMap()
//{
//    //char str[300];
//    //double dy=10000.0;
//    if (groundSystem != 1) { // 1 degree
//        if ((Lat1 == Lat2) || (Lon1 == Lon2)) {
//            //            MessageBox(NULL, "No difference in corner points.", "DEM Incorrectly written", MB_OK);
//            //            return;
//            Lat2 = Lat1+1.0;
//            Lon2 = Lon1+1.0;
//        };
//        /* ///
//         GMap.Set(Lon1,Lon2,Lat1,Lat2, // welcome the real world
//         0.0, (double) columnCount, 0.0, (double) rowCount); // map it to the array of values
//         sprintf(str,"Latitude %lg to %lg, Longitude %lg to %lg.",
//         Lat1,Lat2,Lon1,Lon2);
//         */
//    } else { // 7.5 min
//    }
//};

float dted_ops::GetElevation(double lat, double lon) {
    float e = -0.123456789; // special value to indicate when a dted cell doe not exist here and we treat it as just below AMSL
    if (Lat1 == Lat2) { // this only occurs when the dt is uninitialized - in this case we treat it as just below sealevel
        return e;
    }
    if (lat < Lat1) return e;
    if (lon < Lon1) return e;
    if (lat > Lat2) return e;
    if (lon > Lon2) return e;
    unsigned int i,j;
    double dlt,dln;
    dlt = (lat - Lat1) * (double) rowCount;
    dln = (lon - Lon1) * (double) columnCount;

    i = dlt;
    j = dln;
    if (i >= rowCount) i = rowCount-1; // occurs only when lat is negative and exactly on the line (no fraction)
    if (j >= columnCount) j = columnCount-1; // occurs only when lon is negative and exactly on the line (no fraction)
    e = Elevations[i][j];
    return e;
}


//// DTED object hold DTED file data.

////#include "MessageManager.h"

//#include "dted.h"
//#include <stdlib.h>
//#include <string.h>

//#include <zlib.h>

//// Constructor
// void dted_ops::SetUserSettings()
//{

//}

//dted_ops :: dted_ops(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger)
//{
//    LoggableConfigurableObject::Init(usettingsData, logger, "dted_ops");
//	Elevations = NULL;
//	elevUnitOfMeasure = 2;
//	spatialResolution[0] = 0;
//	spatialResolution[1] = 0;
//	spatialResolution[2] = 1.0;
//    Lat1 = Lat2 = Lon1 = Lon2 = -0.0; // this setting lets us know if we have an uninitialized dt
//}

//dted_ops :: ~dted_ops()
//{
//}

///*---------------------------------------------------------------*/

//bool  dted_ops :: dted_read(std::string cell_name)
//{

//   char temp_buff[80];
//   int  memoffset, i, j, bytes_read, ilatitude, num_records, recLen;
////	short * els = (short *) &(elevation_record[9]);
//   FILE *fp;
//   memoffset = 0;
////   const char *str = cell_name.c_str();

//   if (cell_name.find(".dt1") > 0)
//   {
//       int nn = cell_name.length();
//       string compressedName = cell_name.substr(0,nn-4);
//       string ap = ".dt1";
//       compressedName.append(ap);  //replace(".dt1",".cdt");
//       return this->dted_read_compressed(cell_name);
//   }

//   if ((fp = fopen( cell_name.c_str(), "rb")) == NULL)
//  {
//	  {
//        return false;
//	  }

//  }

//  bytes_read = (int)fread( this->uhl, 1, UHL_LENGTH, fp);
//  bytes_read = (int)fread( this->dsi, 1, DSI_LENGTH, fp);
//  bytes_read = (int)fread( this->acc, 1, ACC_LENGTH, fp);

//  // compute the amount of data to be read
////   strcpy( temp_buff, cell_name);
//   strncpy(temp_buff,&(uhl[12]),3);
//   temp_buff[3] = '\0';
//   ilatitude = atoi( &temp_buff[0]);
//// Now we load the variables in the base class DEM
//// just so that a DTED will behave like a DEM, thru inheritance
//	LoadDEMAttributes();
//	if (DesignatorLevel == 1) recLen = RECORD_LENGTH_DT1;
//	else recLen = RECORD_LENGTH_DT2;
//	if (DesignatorLevel == 1) {
//		if ((ilatitude >= 0) && (ilatitude < 50))
//			num_records = 1201;            // Zone 1, 3 arcsecond separation
//		else if ((ilatitude >= 50) && (ilatitude < 70))
//			num_records = 601;             // Zone 2, 6 arcsecond separation
//		else if ((ilatitude >= 70) && (ilatitude < 75))
//			num_records = 401;             // Zone 3, 9 arcsecond separation
//		else if ((ilatitude >= 75) && (ilatitude < 80))
//			num_records = 301;             // Zone 4, 12 arcsecond separation
//		else if ((ilatitude >= 80) && (ilatitude < 90))
//			num_records = 201;
//	} else if (DesignatorLevel == 2) {
//		if ((ilatitude >= 0) && (ilatitude < 50))
//			num_records = 3601;            // Zone 1, 3 arcsecond separation
//		else if ((ilatitude >= 50) && (ilatitude < 70))
//			num_records = 1801;             // Zone 2, 6 arcsecond separation
//		else if ((ilatitude >= 70) && (ilatitude < 75))
//			num_records = 1201;             // Zone 3, 9 arcsecond separation
//		else if ((ilatitude >= 75) && (ilatitude < 80))
//			num_records = 901;             // Zone 4, 12 arcsecond separation
//		else if ((ilatitude >= 80) && (ilatitude < 90))
//			num_records = 601;
//	}
// // Zone 5, 18 arcsecond separation
//  // Allocate enough memory to store all records of data - not needed - use
//// MFC collection class instead
//  //memptr = (char *) malloc( RECORD_LENGTH * num_records);
//  //
//	if ((Elevations = new float*[rowCount]) == NULL){
////		cout << "Allocation for Elevations failed. \n";
//        return false;
//	} else {
//		// Allocate space for the rows
//		for (i = 0; i < (signed)rowCount; i++) {
//		    if ((Elevations[i] = new float[columnCount]) == NULL)	{//If failure, release memory already allocated
//		        for (j = (i - 1); j >= 0; j--){
//		            delete [] Elevations[j];
//		        }
//		        delete [] Elevations;
//				Elevations = NULL;
////		        cout << "Allocation for rows of Elevations failed.\n";
//                return false;
//			}
//		}
//	}

//	for (i = 0; i < num_records; i ++) {

//      bytes_read = (int)fread( elevation_record, 1, recLen, fp);

//      if (bytes_read == recLen)
//                      store_record_to_memory(i);
//         else {
////             spMsgMgr()->PostInfo("A full record could not be read\n","Error in DTED ");
//			 fclose(fp);
//             return false;
//		 }
	  
//         // This is a debug, just want to see if correct info is there
//		 // Section replaced - use MSVC++ debugger
//  }
//  fclose( fp);
//  return true;
//}

//bool  dted_ops :: dted_read_compressed(string cell_name)
//{

//   char temp_buff[80];
//   int  memoffset, i, j, bytes_read, ilatitude, num_records, recLen;
////	short * els = (short *) &(elevation_record[9]);
//   gzFile gzFp;
//   memoffset = 0;
//   const char *nm = cell_name.c_str();
//   if ((gzFp = gzopen(nm,"rb")) == NULL)
//  {
//  //             MessageBox(NULL,	"Cannot open file","Error -- ",MB_OK);
//      return false;
//  }

//  bytes_read = (int)gzread( gzFp, this->uhl, UHL_LENGTH );
//  bytes_read = (int)gzread( gzFp, this->dsi,  DSI_LENGTH);
//  bytes_read = (int)gzread( gzFp, this->acc,  ACC_LENGTH);

//  // compute the amount of data to be read
////   strcpy( temp_buff, cell_name);
//   strncpy(temp_buff,&(uhl[12]),3);
//   temp_buff[3] = '\0';
//   ilatitude = atoi( &temp_buff[0]);
//// Now we load the variables in the base class DEM
//// just so that a DTED will behave like a DEM, thru inheritance
//	LoadDEMAttributes();
//	if (DesignatorLevel == 1) recLen = RECORD_LENGTH_DT1;
//	else recLen = RECORD_LENGTH_DT2;
//	if (DesignatorLevel == 1) {
//		if ((ilatitude >= 0) && (ilatitude < 50))
//			num_records = 1201;            // Zone 1, 3 arcsecond separation
//		else if ((ilatitude >= 50) && (ilatitude < 70))
//			num_records = 601;             // Zone 2, 6 arcsecond separation
//		else if ((ilatitude >= 70) && (ilatitude < 75))
//			num_records = 401;             // Zone 3, 9 arcsecond separation
//		else if ((ilatitude >= 75) && (ilatitude < 80))
//			num_records = 301;             // Zone 4, 12 arcsecond separation
//		else if ((ilatitude >= 80) && (ilatitude < 90))
//			num_records = 201;
//	} else if (DesignatorLevel == 2) {
//		if ((ilatitude >= 0) && (ilatitude < 50))
//			num_records = 3601;            // Zone 1, 3 arcsecond separation
//		else if ((ilatitude >= 50) && (ilatitude < 70))
//			num_records = 1801;             // Zone 2, 6 arcsecond separation
//		else if ((ilatitude >= 70) && (ilatitude < 75))
//			num_records = 1201;             // Zone 3, 9 arcsecond separation
//		else if ((ilatitude >= 75) && (ilatitude < 80))
//			num_records = 901;             // Zone 4, 12 arcsecond separation
//		else if ((ilatitude >= 80) && (ilatitude < 90))
//			num_records = 601;
//	}
// // Zone 5, 18 arcsecond separation
//  // Allocate enough memory to store all records of data - not needed - use
//// MFC collection class instead
//  //memptr = (char *) malloc( RECORD_LENGTH * num_records);
//  //
//	if ((Elevations = new float*[rowCount]) == NULL){
////		cout << "Allocation for Elevations failed. \n";
//        return false;
//	} else {
//		// Allocate space for the rows
//		for (i = 0; i < (signed)rowCount; i++) {
//		    if ((Elevations[i] = new float[columnCount]) == NULL)	{//If failure, release memory already allocated
//		        for (j = (i - 1); j >= 0; j--){
//		            delete [] Elevations[j];
//		        }
//		        delete [] Elevations;
//				Elevations = NULL;
////		        cout << "Allocation for rows of Elevations failed.\n";
//                return false;
//			}
//		}
//	}

//	for (i = 0; i < num_records; i ++) {

//      bytes_read = (int)gzread(gzFp, elevation_record, recLen);

//      if (bytes_read == recLen)
//                      store_record_to_memory(i);
//         else {
////             spMsgMgr()->PostInfo("A full record could not be read\n","Error in DTED ");
//			 gzclose(gzFp);
//             return false;
//		 }
	  
//         // This is a debug, just want to see if correct info is there
//		 // Section replaced - use MSVC++ debugger
//  }
//  gzclose(gzFp);
///*  */
//  return true;
//}
//void  dted_ops :: store_record_to_memory(int record)
//{
//	int i,j;
//	short a, hibyte, lobyte;
////	short * els = (short *) &(elevation_record[8]);
////	CWordArray *cw = new CWordArray();
////	cw->SetSize(rowCount+1,16);
//  // Copy the record into memory
//  // memcpy( memptr + memoffset, elevation_record, RECORD_LENGTH);
//// WES HELP this is where I swap the bytes to make the integer but it does not
//// handle a negative value
//	for (i=0; i<((int) rowCount); i++) {
//		j = i + i + 8;
//		hibyte = elevation_record[j];
//		lobyte = elevation_record[j+1];
//		a = (hibyte << 8) & 0xFF00;
//		a = a + lobyte;
//		if (a < 0) {
//			a = ~a | 0x8000;
//			a = a + 1;
//		}
////		cw->SetAt(i,a);
//		Elevations[i][record] = a;
//	}
////	 Elevations.Add(cw);
//  // Increment the offset so we keep moving along through memory
//  // memoffset += RECORD_LENGTH;
//}

//void dted_ops::LoadDEMAttributes() {
//	int i;
//    string Uhl = uhl;
//    string Dsi = dsi; // converting to CString just makes them easier to handle
//	// let us set the max and min elevations to something sensable


//    elevBounds[0] = -300.0;
//	elevBounds[1] = 10000.0;
//	mapLabel = uhl;
//    verticalScale = 1.0;

//    DesignatorLevel = atoi(Dsi.substr(63,1).c_str()); //atoi(Dsi.substr(63,1));
//    columnCount = atoi(Dsi.substr(285,4).c_str());
//    rowCount = atoi(Dsi.substr(281,4).c_str());
//    Lon1 = atoi(Dsi.substr(211,3).c_str());
//    Lat1 = atoi(Dsi.substr(204,2).c_str());
//    Lon2 = atoi(Dsi.substr(241,3).c_str());
//    Lat2 = atoi(Dsi.substr(234,2).c_str());
//    if ( Dsi.substr(218,1) != "E") Lon1= -Lon1;
//    if ( Dsi.substr(210,1) != "N") Lat1= -Lat1;
//    if ( Dsi.substr(248,1) != "E") Lon2= -Lon2;
//    if ( Dsi.substr(240,1) != "N") Lat2= -Lat2;
//	groundSystem = 0; // for dted level 1

//	if (Elevations != NULL) {
//		for (i = 0; i < (signed)rowCount; i++) {
//	        delete [] Elevations[i];
//	    }
//	    delete [] Elevations;
//		Elevations = NULL;
//	}
////	SetGMap();

//}

//void dted_ops::SetGMap()
//{
////char str[300];
////double dy=10000.0;
//	if (groundSystem != 1) { // 1 degree
//		if ((Lat1 == Lat2) || (Lon1 == Lon2)) {
////			MessageBox(NULL, "No difference in corner points.", "DEM Incorrectly written", MB_OK);
////			return;
//			Lat2 = Lat1+1.0;
//			Lon2 = Lon1+1.0;
//		};
///* ///
//        GMap.Set(Lon1,Lon2,Lat1,Lat2, // welcome the real world
//			0.0, (double) columnCount, 0.0, (double) rowCount); // map it to the array of values
//		sprintf(str,"Latitude %lg to %lg, Longitude %lg to %lg.",
//				Lat1,Lat2,Lon1,Lon2);
//                */
//	} else { // 7.5 min
//	}
//};

//float dted_ops::GetElevation(double lat, double lon) {
//    float e = -0.123456789; // special value to indicate when a dted cell doe not exist here and we treat it as just below AMSL
//    if (Lat1 == Lat2) { // this only occurs when the dt is uninitialized - in this case we treat it as just below sealevel
//        return e;
//    }
//    if (lat < Lat1) return e;
//    if (lon < Lon1) return e;
//    if (lat > Lat2) return e;
//    if (lon > Lon2) return e;
//    unsigned int i,j;
//    double dlt,dln;
//    dlt = (lat - Lat1) * (double) rowCount;
//    dln = (lon - Lon1) * (double) columnCount;

//    i = dlt;
//    j = dln;
//    if (i >= rowCount) i = rowCount-1; // occurs only when lat is negative and exactly on the line (no fraction)
//    if (j >= columnCount) j = columnCount-1; // occurs only when lon is negative and exactly on the line (no fraction)
//     e = Elevations[i][j];
//    return e;
//}
