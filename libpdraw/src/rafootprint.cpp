#include "rafootprint.h"
#include <boost/thread.hpp>
#include "consts.h"
#include "RotationToolKit.h"
#include "RotationTK.h"

void RAFootprint::SetUserSettings()
{
    //todo: fillin with settings needed from settings.ini file
}

double RAFootprint::GetElevationAtLocation(double lat, double lon, double& z)
{
    z = _EML->GetElevation(lat,lon);
    return 1;
}

int RAFootprint::SetEML(boost::shared_ptr<ElevationManagerLocal> eml) {
    _EML = eml;
    return 1;
}

RAFootprint::RAFootprint(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger):
_rtk(usettingsData, logger)
{
    LoggableConfigurableObject::Init(usettingsData, logger, "RAFootprint");
    // boost::shared_ptr<RotationTK> ntk(new RotationTK(usettingsData, logger));
    // _rtk.reset(new RotationTK(usettingsData, logger));
    // _rtk = (RotationToolKit *) new RotationTK(usettingsData, logger);
}

int RAFootprint::GetTerrainIntercept(double lat, double lon, double alt, double p, double h, double r, double *lt, double *ln, double *al, double *range)
{
    //need msl altitude
    double mslAlt = 0.00;
    int ret2 = GetElevationAtLocation(lat, lon, mslAlt);
    mslAlt += alt;
    
    double x,y,z, dx,dy,dz, nulat, nulon, nualt,dist=1.0,d,az,el,xo,yo,zo,surfalt;
    double dmax = 1000000.0, increment = 1.0;
    int ret;
    x = 0.0;
    y = 0.0;
    //if (!GetGroundElevation(lat, lon, z))
    
    // Get a unit vector to work with here.
    d = 0.0;
    
    double sinP = sin(- p*PI180);
    double cosH = cos(h*PI180);
    double sinH = sin(h*PI180);
    double cosP = cos(-p*PI180);
    double cosR = cos(r*PI180);
    double sinR = sin(r*PI180);
    
    //convert sdf info into vector components
    dx = sinH * cosP;
    dy = cosH * cosP;
    dz = -sinP;
    
    double latcosfact = cos(lat*PI180);
    
    if (dz > 0.0)
        return 0;
    while (d < dmax)
    {
        d += increment;
        x = dx * d;
        y = dy * d;
        z = dz * d;
        // create the new point
        nulat = lat + y / (111320.0); // convert to degrees
        nulon = lon + x / (111320.0 * latcosfact);
        
        
        ret = GetElevationAtLocation(nulat, nulon, surfalt);
        if (!ret)
        {
            //hit the edge of the grid
            
            
            *lt = nulat;
            *ln = nulon;
            *al = surfalt;
            *range = d;
            //if(nulat > (_kmlGridNorthLatLine - 0.0001) && nulat <  (_kmlGridNorthLatLine + 0.0001))
            //{
            //we hit the the grid line
            // *lt = _kmlGridNorthLatLine;
            // }
            return 0;
        }
        
        nualt = mslAlt + z;
        if (surfalt > nualt)
        {
            *lt = nulat;
            *ln = nulon;
            *al = surfalt;
            *range = d;
            return (int)d;
        }
    }
    return 0;
}

FootPrint RAFootprint::ComputePrecisionFootprint(double lat, double lon, double alt, double pitch, double heading, double roll,
                                                 double camPitch, double camHeading, double camRoll, double hFOV, double FOV, double windowWidth, double windowHeight)
{
    // boost::mutex::scoped_lock lock(_footprintMutex);
    int i,n=4;
    _FP.fpValid = 0;
    double ha=0,  va=0;
    
    double hh = 0.5 * hFOV;
    double hv = 0.5 * FOV;
    double losPitch = 0.00;
    double losHeading = 0.00;
    double losRoll = 0.00;
    double range = 0.00;
    double X,Y,D;
    int nx = windowWidth, ny =windowHeight; /// HARDCODED!!!  REPAIR ASAP
    double ifov= hFOV / ((double) nx);
    // do the four corners of the field of view ll,ul,ur, ul.
    i=0;
    // here we see if the ELM is inited, and if not we load a good number of surrounding cells
    double halfSize = 2.0;
    
    if (_EML->GetCellsReadCount() == 0)
        if (_EML->GetCellsWaterOnlyCount() == 0) {
            // then this must be  unitialzed, so we should load the surrounding area
            _EML->LoadArea(lat-halfSize,lon-halfSize,lat+halfSize,lon+halfSize); // load 16 local cells
            printf("cells read %ld; water-only count %ld\n",_EML->GetCellsReadCount(),_EML->GetCellsWaterOnlyCount());
        }
    
    /*
     lat = 52.1;
     lon = 0.1;
     halfSize = 0.5;
     if (_EML->GetCellsReadCount() == 0)
     if (_EML->GetCellsWaterOnlyCount() == 0) {
     // then this must be  unitialzed, so we should load the surrounding area
     _EML->LoadArea(lat-halfSize,lon-halfSize,lat+halfSize,lon+halfSize); // load 16 local cells
     printf("cells read %ld; water-only count %ld\n",_EML->GetCellsReadCount(),_EML->GetCellsWaterOnlyCount());
     }
     */
    losPitch = losHeading = losRoll = 0.0;
    _rtk.GetLOSAngles(pitch, heading, roll, camPitch-hv, camHeading-hh, camRoll, &losPitch, &losHeading,&losRoll);
    if (losPitch != -0.0)
    {
        if (GetTerrainIntercept(lat,lon,alt,losPitch,losHeading,losRoll, &(_FP.fpLat[i]),&(_FP.fpLon[i]),&(_FP.fpAlt[i]),&range))
        {
            _FP.fpValid++;
            GetPixels4LatLon(lat,lon,alt,_FP.fpLat[i],_FP.fpLon[i],_FP.fpAlt[i],
                             camPitch, camHeading, camRoll,ifov,nx,ny,&X, &Y, &D);
        }
        i++;
    }
    losPitch = losHeading = losRoll = -0.0;
    _rtk.GetLOSAngles(pitch, heading, roll, camPitch + hv, camHeading-hh, camRoll, &losPitch, &losHeading, &losRoll);
    if(losPitch > -1.0)
    {
        losPitch = -1.0;
    }
    if (losPitch != -0.0)
    {
        if (GetTerrainIntercept(lat,lon,alt,losPitch,losHeading,losRoll, &(_FP.fpLat[i]),&(_FP.fpLon[i]),&(_FP.fpAlt[i]),&range))
        {
            _FP.fpValid++;
        }
    }
    i++;
    losPitch = losHeading = losRoll = -0.0;
    _rtk.GetLOSAngles(pitch ,heading, roll, camPitch + hv, camHeading + hh, camRoll, &losPitch, &losHeading,&losRoll);
    if(losPitch > -1.0)
    {
        losPitch = -1.0;
    }
    if (losPitch != -0.0)
    {
        if (GetTerrainIntercept(lat,lon,alt,losPitch,losHeading,losRoll, &(_FP.fpLat[i]),&(_FP.fpLon[i]),&(_FP.fpAlt[i]),&range))
        {
            _FP.fpValid++;
        }
        i++;
    }
    losPitch = losHeading = losRoll = -0.0;
    _rtk.GetLOSAngles(pitch, heading, roll, camPitch - hv, camHeading + hh, camRoll, &losPitch, &losHeading,&losRoll);
    if (losPitch != -0.0)
    {
        if (GetTerrainIntercept(lat,lon,alt,losPitch,losHeading,losRoll, &(_FP.fpLat[i]),&(_FP.fpLon[i]),&(_FP.fpAlt[i]),&range))
        {
            _FP.fpValid++;
            //            GetPixels4LatLon(lat,lon,alt,_FP.fpLat[i],_FP.fpLon[i],_FP.fpAlt[i],losPitch,losHeading,losRoll,ifov,nx,ny,&X, &Y, &D);
        }
        i++;
    }
    // compute the starepoint
    
    losPitch = losHeading = losRoll = -0.0;
    _rtk.GetLOSAngles(pitch, heading, roll, camPitch, camHeading, camRoll, &losPitch, &losHeading,&losRoll);
    if (losPitch != -0.0)
    {
        if (GetTerrainIntercept(lat,lon,alt,losPitch,losHeading,losRoll, &(_FP.SPLat),&(_FP.SPLon),&(_FP.MouseAlt),&range))
        {
            //            _FP.fpValid++;
            GetPixels4LatLon(lat,lon,alt,_FP.SPLat,_FP.SPLon,_FP.MouseAlt,
                             camPitch, camHeading, camRoll, ifov,nx,ny,&X, &Y, &D);
        }
    }
    
    return _FP;
}

double RAFootprint::ComputePOIPitchAboveSP(double vfov, double windowheight, int y)
{
    //assuming y is up screenwise.
    double halfWindowHeight = windowheight / 2.0;//y coord of starepoint
    double deltay =  halfWindowHeight - (double)y  ;

    double percentOfHeight = deltay / windowheight;
    double deltaVFOV = percentOfHeight * vfov;

    return  deltaVFOV;
}

double RAFootprint::ComputePOIHorzDeltaRightOfSP(double vfov, double windowwidth, double windowheight, int x, int vpwidth, int vpheight)
{

    //take vpwidth and vpheight aspect, along with vfov to get hfov
    double vpAspect = (double)vpwidth/(double)vpheight;
    double hfov = vfov * vpAspect;
    //take aspect ratio of vpwidth and vpheight compared to aspect of windowwidth and windowheight.  This will give us the width
    // vp in window cordinates assuming window height
    //then assume the vp is centered horizontally and calculate the width of the black bars
    double vpWidthInWindowUnits = (double)windowheight*(vpAspect);
    
    //the x based on the opengl viewport
    int viewpointx = x - ((double)windowwidth - (double)vpWidthInWindowUnits )/ 2.0;
    double halfWindowWidth = vpWidthInWindowUnits / 2.0;
    double deltax = (double)viewpointx - halfWindowWidth;

    double percentOfWidth = deltax / vpWidthInWindowUnits;
    double deltaHFOV = percentOfWidth * hfov;

    return  deltaHFOV * 0.90;
}


int RAFootprint::GetLatLon4Pixels(boost::shared_ptr<SViewData> svd, double x, double y,
                                  double *lat, double  *lon, double  *elev, int windowWidth, int windowHeight, int vpwidth, int vpheight)

//void ComputeGeoFromScreenCoords(int touchXInViewPort, int touchYInViewPort, double vehlat, double vehlon, double vehalt, double vehpitch, double vehheading, double vehroll,
//                                    double camPitch, double camHeading, double camRoll, double hFOV, double FOV,
//                                    double viewPortWidth, double viewPortHeight, //double testElevation,
//                                    CoordinateConverter coordConverter, double POILat[], double POILon[], double POIElev[])
    {
        //screeny = (int)(windowHeight / 2.0);
        double starepointlat = 0.00;
        double starepointlon = 0.00;
        double starepointalt = 0.00;
        double starepointrange = 0.00;

        double POIlatCenterHorizontal =0.00 ;
        double POIlonCenterHorizontal = 0.00;
        double POIaltCenterHorizontal = 0.00;
        double POIrangeCenterHorizontal = 0.00;
        double POIxCenterHorizontal = 0.00;
        double POIyCenterHorizontal = 0.00;
        double POIzCenterHorizontal = 0.00;

//        MikeRIRotationTK rotTK = new MikeRIRotationTK();
//        double losPitch[] = {0.00};
//        double losHeading[] = {0.00};
//        double losRoll[] = {0.00};

        double radHeading = degreeToRadians(svd->dCameraHeading);// losHeading[0]);
        //double radHFOV = degreeToRadians(svd->dFovHorizontalAngle);

        //rotTK.GetLOSAngles(vehpitch, vehheading, vehroll, camPitch, camHeading, camRoll, losPitch, losHeading, losRoll);

        //GetTerrainIntercept(vehlat,vehlon,vehalt,losPitch[0],losHeading[0],losRoll[0], starepointlat, starepointlon, starepointalt, starepointrange);
        GetTerrainIntercept(svd->dVehicleLat, svd->dVehicleLon, svd->dVehicldAltitudeAGL, svd->dCameraPitch, svd->dCameraHeading, svd->dCameraRoll,
                            &starepointlat, &starepointlon, &starepointalt, &starepointrange);


        //todo pitch is showing neg when you click above sp, and pos below
        //todo screen y is from top down is increasing y ie top of screen = y = 0
        double POIDeltaSPPitch = ComputePOIPitchAboveSP(svd->dfov, windowHeight, y);//TODO CALC PITCH ABOVE SP OF OUR SCREEN CLICK COORDINATE IE PRORATE OUT BASED ON VFOV
        //double POIPitch = losPitch[0] + POIDeltaSPPitch ;
        double POIPitch = svd->dCameraPitch + POIDeltaSPPitch ;
        
        //good to this point!!
        //poilat/lon/alt center l/r is the point
//        GetTerrainIntercept(vehlat,vehlon,vehalt,POIPitch,losHeading[0],losRoll[0], POIlatCenterHorizontal, POIlonCenterHorizontal,
//                POIaltCenterHorizontal, POIrangeCenterHorizontal);
        GetTerrainIntercept(svd->dVehicleLat, svd->dVehicleLon, svd->dVehicldAltitudeAGL, POIPitch, svd->dCameraHeading, svd->dCameraRoll,
                            &POIlatCenterHorizontal, &POIlonCenterHorizontal, &POIaltCenterHorizontal, &POIrangeCenterHorizontal);
        


        _coordConverter->GeodeticToCartesian(POIlatCenterHorizontal, POIlonCenterHorizontal, POIaltCenterHorizontal, POIxCenterHorizontal,
                POIyCenterHorizontal, POIzCenterHorizontal);

        //TODO DON'T USE RADHFOV, INSTEAD WE NEED TO GET DELTA HFOV IE PRORATE X COORD WITH HFOV
        //double halfcartWidthAtPOI =  (POIrangeCenter[0] * (tan(0.5 * radHFOV))) ;
        double POIdeltaHeading = ComputePOIHorzDeltaRightOfSP(svd->dfov, windowWidth, windowHeight, x, vpwidth, vpheight);
        double radPOIDeltaHeading = degreeToRadians(POIdeltaHeading);
        double cartWidthAtPOI = POIrangeCenterHorizontal * tan(radPOIDeltaHeading);

        double radLosHeading = degreeToRadians(svd->dCameraHeading);
        double sinhdg = sin(radLosHeading);
        double coshdg = cos(radLosHeading);

        double sin180minushdg = sin(PI - radLosHeading);//.78
        double cos180minushdg = cos(PI - radLosHeading);//.61
//        Log.d("DGD", "ComputeGeoFromScreenCoords: radlosheading" + Double.toString(radLosHeading));
//        Log.d("DGD", "ComputeGeoFromScreenCoords: sinhdg" + Double.toString(sinhdg));
//        Log.d("DGD", "ComputeGeoFromScreenCoords: coshdg" + Double.toString(coshdg));

        //calc lower left xy (in 3d space, not window coords) of screen at footprint corner
        double POIx = POIxCenterHorizontal - (cartWidthAtPOI * cos180minushdg);
        double POIy = POIyCenterHorizontal - (cartWidthAtPOI * sin180minushdg);

        double llat = 0.0;
        double llon = 0.0;
        double lelev = 0.0;
        _coordConverter->CartesianToGeodetic( POIx, POIy, POIzCenterHorizontal, llat, llon, lelev);
//        *lat = 46.68509444;//llat;
//        *lon = -92.36027778;//llon;
        *lat = llat;
        *lon = llon;
        *elev = lelev;
        return 1;
    }
//int RAFootprint::GetLatLon4Pixels(boost::shared_ptr<SViewData> svd, double x, double y,
//                                  double *lat, double  *lon, double  *elev, int windowWidth, int windowHeight)
//{
//    double hfov = svd->dFovHorizontalAngle - 8.0;
//    double radHFOV = degreeToRadians(hfov);
//    double radHalfHFOV = degreeToRadians(hfov / 2.0);
//    double radVFOV = degreeToRadians(svd->dFovVerticalAngle);
//    double radHalfVFOV = degreeToRadians(svd->dFovVerticalAngle / 2.0);
//    double radHeading = degreeToRadians(svd->dCameraHeading);
//    
//    double halfWinWidth = windowWidth / 2.0;
//    double focalPoint = halfWinWidth / tan(radHalfHFOV);
//    //x =windowWidth ;//- 40;
//    
//    //get total screen width/height, use x/y to get percent of distance across screen
//    //use that percent times svd.->hfov and vfov.  use that angle for pitch heading roll given to
//    //getterrainintercept
//    double ha=0,  va=0;
//    //double percentHorizontal = static_cast<double>(x) / static_cast<double>(windowWidth);
//    double percentVertical = static_cast<double>(y) / static_cast<double>(windowHeight);
//    
//    
//    //find horizontally centered userpoint position finding y position.  use percent of fov based
//    // on yscreencoord/screen height * fov.  this gives us the deviation from the camera pitch ie starepoint
//    
//    //double hh = (-1.0) * (svd->dFovHorizontalAngle * (0.5 - percentHorizontal));
//    //verDevFromCenter is deviation from starepoint in degrees vertically
//    double verDevFromCenter = (-1.0) * (svd->dFovVerticalAngle * (0.5 - percentVertical));
//    //horDevFromCenter is horizontal deviation from starepoint in degrees
//    //double horDevFromCenter = (percentHorizontal - 0.5) * hfov;
//    
//    //get the range and lat/lon/alt of the horizontally centered point of y user coordinate
//    double latCenter = 0.00;
//    double lonCenter = 0.00;
//    double altCenter = 0.00;
//    double rangeCenter = 0.00;
//    //TODO: VERIFY HV SHOULD BE + OR -
//    GetTerrainIntercept(svd->dVehicleLat, svd->dVehicleLon, svd->dVehicleAltitude, svd->dCameraPitch - verDevFromCenter ,svd->dCameraHeading, svd->dCameraRoll, &latCenter, &lonCenter, &altCenter, &rangeCenter);
//    
//    double newW = x - halfWinWidth;
//    double newHorThetaRad = atan(newW/focalPoint);
//    //double newHorThetaDegree = radiansToDegrees(newHorThetaRad);
//    //geographic distance in the screen x direction from horizontally centered screen y point to get to user point
//    //double xdistanceFromYCenter = rangeCenter*(tan(degreeToRadians(horDevFromCenter)));
//    double xdistanceFromYCenter = rangeCenter*(tan(newHorThetaRad));
//    
//    //at this point we have the geographic location of the horizontally centered y location of the user point.  ie we have
//    // the ground location lat/lon where the user clicked if we centered the screen x location at the center of the screen
//    // we also have the geographic width in the screen x direction to get to our user point.  Now use
//    // trig to calculate the rest.
//    double sinhdg = sin(radHeading);
//    double coshdg = cos(radHeading);
//    double xCenter = 0.00;
//    double yCenter = 0.00;
//    double zCenter = 0.00;
//    _coordConverter->GeodeticToCartesian(latCenter, lonCenter, altCenter, xCenter, yCenter, zCenter);
//    
//    double userX = xCenter + (xdistanceFromYCenter * coshdg);
//    double userY = yCenter - (xdistanceFromYCenter * sinhdg);
//    //find the x distance from y centered point where the user point is located
//    //double  =  (rangeCenter * (tan(0.5 * radHFOV))) ;//- fpshrinkage*50
//    
//    //double lp, lh, lr, range = 0.0;
//    // do the four corners of the field of view ll,ul,ur, ul.
//    //lp = lh = lr = -0.0;
//    double userLat = 0.00;
//    double userLon = 0.00;
//    double userElev = 0.0;
//    _coordConverter->CartesianToGeodetic(userX, userY, altCenter, userLat, userLon, userElev) ;
//    *lat = userLat;
//    *lon = userLon;
//    *elev = userElev;
//    return 1;
//}

int RAFootprint::GetPixels4LatLon(double clat, double clon, double calt,
                                  double lat, double lon, double alt,
                                  double  lp, double lh, double lr,
                                  double  ifov, int nx, int ny,
                                  double *x, double*y, double *d){
    double vx,vy,vz, px,py,pz, ppx, ppy, ppz;
    double c[3],p[3];
    _coordConverter->GeodeticToCartesian(clat,clon,calt,vx,vy,vz);
    _coordConverter->GeodeticToCartesian(lat,lon,alt,px,py,pz);
    ppx = px-vx;
    ppy = py-vy;
    ppz = pz-vz;
    
    c[0] = vx;
    c[1] = vy;
    c[2] = vz;
    
    p[0] = px;
    p[1] = py;
    p[2] = pz;
    //((RotationTK )(*(_rtk.get())).Pixels4Geo(c,p,lp,lh,lr,ifov,nx,ny,x,y,d);
    _rtk.Pixels4Geo(c,p,lp,lh,lr,ifov,nx,ny,x,y,d);
    //    _rtk->Pixels4Geo(c,p,lp,lh,lr,ifov,nx,ny,x,y,d);
    return 1;
}
