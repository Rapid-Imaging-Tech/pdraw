#include "elevationmanagerlocal.h"
#include <sstream>
#include "boost/shared_ptr.hpp"
#if defined(__APPLE__)
#include "objcUtilityWrapper.h"
#endif
#include "utilities.h"
string iTostr(int i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

void ElevationManagerLocal::SetUserSettings()
{
     //_usettingsdata = usettings;
     if(!_userSettingsData->_stringSettings["CDTFOLDER"].empty()){
         this->DTEDpath = _userSettingsData->_stringSettings["CDTFOLDER"];
         std::string str("loading dted from: ");
         str.append(DTEDpath);
         WriteToStatusLog(str, false, false);
     }
     else
         this->DTEDpath = "/mapdata/CDT/";
}

string ElevationManagerLocal::MakeSRTM_CDTName(double Lat, double Lon) {
    string name,lathemi,lonhemi,latnum,lonnum;
    int ilt,iln,ign,igt;
    
    if (Lat < 0.0) { // s
        lathemi = "S";
        // figure out if it is exactly on the line
        ilt =  (int) -Lat;
        if (Lat != (double) ((int) Lat))
            ilt ++;
        latnum = std::to_string(ilt);
        
    } else {
        lathemi = "N";
        ilt = (int) Lat;
        latnum = std::to_string(ilt);
    }
    
    if (Lon < 0.0) {
        lonhemi = "W";
        iln = ((int) -Lon);    // figure out if it is exactly on the line
        if (Lon != (double) ((int) Lon))
            iln ++;
        lonnum = std::to_string(iln);
    } else {
        lonhemi = "E";
        iln = (int) Lon;
        lonnum = std::to_string(iln);
    }
    std::string lonn = std::string(3 - lonnum.length(), '0') + lonnum;
    std::string latt = std::string(2 - latnum.length(), '0') + latnum;
    
    name = lathemi+latt+lonhemi+lonn+".cdt";
    
    return name;
}

ElevationManagerLocal::ElevationManagerLocal(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger)
{
    LoggableConfigurableObject::Init(usettingsData, logger, "ElevationManagerLocal");
    CellsReadCount = 0;
    CellsWaterOnly = 0;
    //DTEDpath = "/mapdata/CDT/";
    int i,j;
    for (i=0;i<180;i++)
            for (j=0;j<360;j++)
                World[i][j] = NULL;
}

string ElevationManagerLocal::MakeCDTName(double Lat, double Lon)
{
    string name,lathemi,lonhemi,latnum,lonnum;
    int ilt,iln,ign,igt;
    std::stringstream ss;
    return MakeSRTM_CDTName(Lat,Lon);

    if (Lat < 0.0) { // s
        lathemi = "S";
        ilt = (int) -Lat;
        ilt += 1;
        latnum = iTostr(ilt);
        igt = ((int)(0.2 * (-Lat)) * 5);

    } else {
        lathemi = "N";
        ilt = (int) Lat;
        //    ilt += 1;
        latnum = iTostr(ilt);
        igt = ((int)(0.2 * (ilt+5)) * 5);
    }

    if (Lon < 0.0) {
        lonhemi = "W";
        iln = ((int) -Lon);
        iln +=1;
        lonnum = iTostr(iln);
        //    ign = ((int)(0.2 * (iln+5)) * 5);
        ign = ((int)(0.2 * (-Lon+5)) * 5);
    } else {
        lonhemi = "E";
        iln = (int) Lon;

        lonnum = iTostr(iln);
        //    ign = ((int)(0.2 * iln) * 5);
        ign = ((int)(0.2 * iln) * 5);
    }
    name = "_"+lonhemi+lonnum+"_"+lathemi+latnum+".CDT";

    latnum = iTostr(igt);
    lonnum = iTostr(ign);

    std::string lonn;// = std::string(3 - lonnum.length(), '0') + lonnum;
    int len = lonnum.length();
    lonn = std::string(3 - lonnum.length(), '0') + lonnum;
    std::string latt = std::string(2 - latnum.length(), '0') + latnum;
    string prenomen = lathemi + latt + lonhemi + lonn + name;
    name = prenomen;
    cout<<"attempt to load cdt "<<name<<endl;
    return name;
}

ElevationManagerLocal::~ElevationManagerLocal(){
    Clear();
}

void ElevationManagerLocal::Clear()
{
//    int i,j;
//    for (i=0;i<180;i++)
//            for (j=0;j<360;j++) {
//                if (World[i][j] != NULL) {
//                    //delete World[i][j];
//                    //World[i][j] = NULL;
//                }
//            }

}

int ElevationManagerLocal::GetLatIndex(double lat) {
    int l = (int) (lat+90.0);
    if (lat < 0.0) {
//        lat = -lat;
//        l = (int) lat;
    }
    return l;
}

int ElevationManagerLocal::GetLonIndex(double lon) {
    int l = (int) (lon+180.0);
    if (lon < 0.0) {
//        lon = -lon;
//        l = (int) lon;
    }
    return l;

}

float ElevationManagerLocal::GetElevation(double lat, double lon)
{

    float e = -32677.0;
    int ilt=GetLatIndex(lat);
    int iln=GetLonIndex(lon);
    boost::shared_ptr<dted_ops> dt = World[ilt][iln];
    if (dt == NULL) {
        /// YOU COULD LOAD THE DTED IF IT IS NOT HERE
//        printf("unable to load ilt %d iln %d   lat %f lon %f\n",ilt,iln,lat,lon);
        return -0.1;
    }
    e = dt->GetElevation(lat,lon);
    return e;
}

int ElevationManagerLocal::LoadArea(double lat1, double lon1, double lat2, double lon2) {
    int i,j,iln,ilt;
    double dlt, dln,lt,ln;
    dlt = lat2-lat1;
    dln = lon2-lon1;
    double size = 1.0;
#if defined(__APPLE__)
    size = 2.0;
#endif
    int nlt = (dlt + size);
    int nln = (dln + size);
    for (i=0;i<nlt;i++) {
        lt = lat1 + i;
        ilt = GetLatIndex(lt);
        for (j=0;j<nln; j++) {
            ln = lon1 + j;
            iln = GetLonIndex(ln);
            LoadCell(lt,ln);
//            printf("loading %d %d\n",ilt,iln);
        }
    }
    return 1;
}
#include "geotileservergl.h"
extern std::string GetDataDirectory();
int ElevationManagerLocal::LoadCell(double lat, double lon) {
    int iln,ilt;

    ilt = GetLatIndex(lat);
    iln = GetLonIndex(lon);
    if (World[ilt][iln] != NULL)
    {
        // Attempted to load cell that is already loaded
        return 2;
    }
#if defined(__APPLE__)
    DTEDpath = GetDataFolderPath();
    DTEDpath.append("/");
#else
#endif
    string cellpath;// = DTEDpath;
    cellpath += MakeCDTName(lat,lon);
    /////////
    //WriteToStatusLog("loading vector data", false, false);
    boost::asio::io_service io_service2_;
    std::stringstream ssPathToResource;
    std::stringstream ssDTEDFileName;
    //http://storage.googleapis.com/rapid_imaging_elevation/
    //pbf filename for icons/placemarks we parse ourselves
    ssDTEDFileName<<cellpath;//<<DTEDpath<<cellpath;
    ssPathToResource<<"/rapid_imaging_elevation/";
    ssPathToResource<<cellpath;
    //vtsTileSS<<"/data/v3/"<<_zoomLevel<<"/"<<_x<<"/"<<_y<<".pbf";
    //std::string pbfss(vtsTileSS.str());
    //std::string pbf(pbfFileName.str());
    std::string pathToResource(ssPathToResource.str());
    std::string DTEDFileName = ssDTEDFileName.str();
    
    std::string dtedPath = GetDataDirectory();
    dtedPath.append("/");
    dtedPath.append(DTEDFileName);
    std::string errorMsg;
    if(!FileExists(dtedPath, errorMsg))
    {
        //std::string rismsg("can't find file ");
        //rismsg.append(fullPath);
        //WriteToStatusLog(errorMsg, true, true);
        //this->WriteToStatusLog(rismsg, true, true);
        //return 0x00;
        
        //if file doesn't exist, lets go to the tileserver to grab it
//        geotileservergl tileserver2(io_service2_, _tileserverIP, _tileserverPort, pbfss, pbf, _userSettingsData, _statusLogger);//TS: GONNA NEED TO GRAB ADDRESS FROM SETTINGS.INI FILE
//        io_service2_.run();
//        io_service2_.stop();
        geotileservergl tileserver2(io_service2_, "storage.googleapis.com", "80", pathToResource, DTEDFileName, _userSettingsData, _statusLogger);
        cout<<"loading tile "<<DTEDFileName<<endl;
        io_service2_.run();
        io_service2_.stop();
    }
    else
    {
        std::cout<<"Found elevation dted "<<dtedPath<<" from cache"<<std::endl;
    }
    
   
    
    //GOOGLE_PROTOBUF_VERIFY_VERSION;
    std::stringstream ss;
    
    //bool verbose = true;
    
    //try
    //{
//#if defined(__APPLE__)
        LFObjCFunctionsCPPWrapper ocWrapper;
        std::stringstream ss2;
    std::string filename = DTEDFileName;
        std::string pbfFullPath = ocWrapper.GetPbfPath(filename);
    
    //////////
    //string cellpath = DTEDpath;
    //cellpath += MakeCDTName(lat,lon);
    boost::shared_ptr<dted_ops> dt(new dted_ops(_userSettingsData, _statusLogger));
    if (! dt->dted_read_compressed(pbfFullPath)) { // cell is missing from collection - assume it is only ocean therein
        std::string errorMsg("Not finding cell: ");
        errorMsg.append(cellpath);
        WriteToStatusLog(cellpath, true, true);
        //printf("not finding cell: %s \n",cellpath.c_str());
        CellsWaterOnly++;
    } else CellsReadCount++;
    std::stringstream ss3;
    ss3<<"loading "<<cellpath<<" ilt "<<ilt<<" iln "<<iln<<" lat "<<lat<<" lon "<<lon<<" within: "<<dt->Lat1<<" "<<dt->Lon1<<" "<<dt->Lat2<<" "<<dt->Lon2;
    std::string strMsg(ss3.str());
    WriteToStatusLog(strMsg, false, true);
//    printf("loading %s ilt %d iln %d   lat %f lon %f within: %f,%f %f,%f\n",cellpath.c_str(),ilt,iln,lat,lon,dt->Lat1,dt->Lon1,dt->Lat2,dt->Lon2);
    World[ilt][iln] = dt;
    return 1;
}

int ElevationManagerLocal::GetCellsReadCount() {
    return CellsReadCount;
}

int ElevationManagerLocal::GetCellsWaterOnlyCount(){
    return (CellsWaterOnly);
}

int ElevationManagerLocal::SetCDTpath(string pth) {
    DTEDpath = pth;
    return 1;
}
