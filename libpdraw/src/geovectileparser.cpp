#include "geovectileparser.h"
//#include <utilities.h>
#include <iostream>
using namespace std;
#include <boost/shared_ptr.hpp>
geovectileparser::geovectileparser(boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger)
{
    LoggableConfigurableObject::Init(usettingsData, logger, "geovectileparser");

}

void geovectileparser::SetUserSettings()
{
    //fill in with any settings needed from settings.ini file
}

int geovectileparser::getCommandInteger(int id, int count)
{
    int commandInteger = (id & 0x00000007) | (count << 3);
    return commandInteger;
}

int geovectileparser::getId(int CommandInteger)
{
    //cout<<"commandint ="<<CommandInteger<<endl;
    int id = CommandInteger & 0x00000007;
    return id;
}

int geovectileparser::getCount(int CommandInteger)
{
    int count = CommandInteger >> 3;
    return count;
}

int geovectileparser::getValue(int ParameterInteger)
{
    int value = ((ParameterInteger >>1) ^ (-(ParameterInteger & 1)));
    return value;
}

//these 2 can be 1 function, all they do is parse the x,y values
void geovectileparser::parseMoveTo(const boost::shared_ptr<std::vector<uint32_t> > commands, int& currentIndex, int& currentx, int& currenty)//, BoundingBox& boundBox, double& lat, double& lon)
{
    //grab 2 moveto points x,y
    //increment count

    int linetox = (*(commands.get()))[(currentIndex)];
    currentIndex++;
    int xval = getValue(linetox);
    int linetoy = (*(commands.get()))[(currentIndex)];
    currentIndex++;
    int yval = getValue(linetoy);
    //cout<<"ParamInts("<<linetox<<","<<linetoy<<")"<<endl;
    // cout<<"Moveto("<<xval<<","<<yval<<")"<<endl;
    currentx += xval;
    currenty += yval;
}

void geovectileparser::parseLineTo(const boost::shared_ptr<std::vector<uint32_t> > commands, int& currentIndex, int& currentx, int& currenty)//, BoundingBox& boundBox, double& lat, double& lon)
{
    //grap 2 lineto points x,y
    //increment counter

    int linetox = (*(commands.get()))[(currentIndex)];
    currentIndex++;
    int xval = getValue(linetox);
    int linetoy = (*(commands.get()))[(currentIndex)];
    currentIndex++;
    int yval = getValue(linetoy);
    //cout<<"ParamInts("<<linetox<<","<<linetoy<<")"<<endl;
    //cout<<"Lineto("<<xval<<","<<yval<<")"<<endl;
    currentx += xval;
    currenty += yval;
    //lat = boundBox.north - (deltaNS * proratedMoveY);
    //lon = boundBox.west + (deltaEW * proratedMoveX);

}
//returns true if point is inside vector tile, false if outside
bool geovectileparser::interpolateXY(const BoundingBox& boundBox, int currentx, int currenty, int tilesize, double& lat, double& lon)
{
    bool insideTile = true;
    double deltaNS = boundBox.north -  boundBox.south;
    double deltaEW = boundBox.east -  boundBox.west;
    double proratedMoveX = static_cast<double>(currentx)/static_cast<double>(tilesize);
    double proratedMoveY = static_cast<double>(currenty)/static_cast<double>(tilesize);//if y = 0 this is 0 if y is tilesize this is 100% move from north line to south tile line
    //currenty/tilesize is the fraction you have to move from north line southwards of deltaNS
    //lat = northlat - fraction*deltaNS;
    lat = boundBox.north - (deltaNS * proratedMoveY);
    lon = boundBox.west + (deltaEW * proratedMoveX);
    if(currentx < 0 || currentx > tilesize){
        //cout<<"outside tile"<<endl;
        insideTile = false;
    }
    if(currenty < 0 || currenty > tilesize){
        //cout<<"outside tile"<<endl;
        insideTile = false;
    }
    return insideTile;
}
#include <iomanip>

void geovectileparser::parseLineString(const boost::shared_ptr<std::vector<uint32_t> > commands,
                                       int vectilex, int vectiley, int zoomlevel, int tilesize,
                                       boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecStreets, bool& outsideTileN,
                                        bool& outsideTileS, bool& outsideTileE, bool& outsideTileW)
                                       //std::shared_ptr<Kml::Vectors> vecStreets)
{

    outsideTileN = false;
    outsideTileS = false;
    outsideTileE = false;
    outsideTileW = false;
    //std::shared_ptr<std::vector<std::shared_ptr<Kml::Vectors> > > vectorList(new std::vector<std::shared_ptr<Kml::Vectors> >);
    int s = commands->size();
    int currentx = 0;
    int currenty = 0;
    BoundingBox bb = tile2boundingBox(vectilex, vectiley, zoomlevel);
    std::vector<boost::shared_ptr<Kml::Point> > lineStringOutline;
    std::vector<boost::shared_ptr<Kml::Point> > lineStringStreet;
    for(int i = 0 ; i < commands->size() ; )
    {
        int id = getId((*(commands.get()))[i]);
        int count = getCount((*(commands.get()))[i]);
        int commandInd = getCommandInteger(id, count);
        double lat = 0.00;
        double lon = 0.00;
        boost::shared_ptr<Kml::Point> point;
        i++;
        for(int k = 0 ; k < count ; k++)
        {
            bool inside = true;
            switch(id)
            {
                case 1:
                    //start a line
                    //if linestring exists, push it onto the streetvertices stack
                    if(lineStringStreet.size() > 1)
                    {
                        //create a kml::vector from linestring and push it onto vectorlist
                        boost::shared_ptr<Kml::Vectors> vectors1(new Kml::Vectors);
                        vectors1->SetStreetVectors(lineStringStreet);
                        vecStreets->push_back(vectors1);//we're starting a new line, push the old one to the stack
                        //vecStreets = vectors1;
                    }
                    parseMoveTo(commands, i, currentx, currenty);//, bb, lat, lon);
                    inside = interpolateXY(bb, currentx, currenty, tilesize, lat, lon);

                    point.reset(new Kml::Point);
                    point->SetLat(lat);
                    point->SetLon(lon);
                    if(lat > bb.north )
                        outsideTileN = true;
                    if(lat < bb.south )
                        outsideTileS = true;
                    if(lon > bb.east )
                        outsideTileE = true;
                    if(lon < bb.west )
                        outsideTileW = true;

                    lineStringStreet.clear();
                    lineStringStreet.push_back(point);
                    break;
                case 2:
                    //add to existing line
                    parseLineTo(commands, i, currentx, currenty);//, bb, lat, lon);
                    interpolateXY(bb, currentx, currenty, tilesize, lat, lon);
                    point.reset(new Kml::Point);
                    point->SetLat(lat);
                    point->SetLon(lon);
                    if(lat > bb.north )
                        outsideTileN = true;
                    if(lat < bb.south )
                        outsideTileS = true;
                    if(lon > bb.east )
                        outsideTileE = true;
                    if(lon < bb.west )
                        outsideTileW = true;
                    lineStringStreet.push_back(point);
                    break;
                default:
                    break;
            }
        }
    }
    if(lineStringStreet.size() > 1)
    {
        //create a kml::vector from linestring and push it onto vectorlist
        boost::shared_ptr<Kml::Vectors> vectors2(new Kml::Vectors);

        vectors2->SetStreetVectors(lineStringStreet);
        vecStreets->push_back(vectors2);//we're starting a new line, push the old one to the stack
    }

    for(int i = 0 ; i < vecStreets->size() ; i++)
    {
        boost::shared_ptr<Kml::Vectors> linestringVec = (*(vecStreets.get()))[i];
        vector<boost::shared_ptr<Kml::Point> > points = linestringVec->GetStreetVectors();
        for(int k = 0 ; k < points.size() ; k++)
        {
            boost::shared_ptr<Kml::Point> singlepoint = points[k];
        }

    }
}
//
//void geovectileparser::parseLineString(const boost::shared_ptr<std::vector<uint32_t> > commands,
//                                       int vectilex, int vectiley, int zoomlevel, int tilesize,
//                                       boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecStreets)
//                                       //boost::shared_ptr<Kml::Vectors> vecStreets)
//{
//    //boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vectorList(new std::vector<boost::shared_ptr<Kml::Vectors> >);
//    int s = commands->size();
//    int currentx = 0;
//    int currenty = 0;
//    BoundingBox bb = tile2boundingBox(vectilex, vectiley, zoomlevel);
//    std::vector<boost::shared_ptr<Kml::Point> > lineStringOutline;
//    std::vector<boost::shared_ptr<Kml::Point> > lineStringStreet;
//    for(int i = 0 ; i < commands->size() ; )
//    {
//        int id = getId((*(commands.get()))[i]);
//        int count = getCount((*(commands.get()))[i]);
//        int commandInd = getCommandInteger(id, count);
//        double lat = 0.00;
//        double lon = 0.00;
//        boost::shared_ptr<Kml::Point> point;
//        i++;
//        for(int k = 0 ; k < count ; k++)
//        {
//            bool inside = true;
//            switch(id)
//            {
//                case 1:
//                    //start a line
//                    //if linestring exists, push it onto the streetvertices stack
//                    if(lineStringStreet.size() > 1)
//                    {
//                        //create a kml::vector from linestring and push it onto vectorlist
//                        boost::shared_ptr<Kml::Vectors> vectors1(new Kml::Vectors);
//                        vectors1->SetStreetVectors(lineStringStreet);
//                        vecStreets->push_back(vectors1);//we're starting a new line, push the old one to the stack
//                        //vecStreets = vectors1;
//                    }
//                    parseMoveTo(commands, i, currentx, currenty);//, bb, lat, lon);
//                    inside = interpolateXY(bb, currentx, currenty, tilesize, lat, lon);
//                    point.reset(new Kml::Point);
//                    point->SetLat(lat);
//                    point->SetLon(lon);
//                    lineStringStreet.clear();
//                    lineStringStreet.push_back(point);
//                    break;
//                case 2:
//                    //add to existing line
//                    parseLineTo(commands, i, currentx, currenty);//, bb, lat, lon);
//                    interpolateXY(bb, currentx, currenty, tilesize, lat, lon);
//                    point.reset(new Kml::Point);
//                    point->SetLat(lat);
//                    point->SetLon(lon);
//
//                    lineStringStreet.push_back(point);
//                    break;
//                default:
//                    break;
//            }
//        }
//    }
//    if(lineStringStreet.size() > 1)
//    {
//        //create a kml::vector from linestring and push it onto vectorlist
//        boost::shared_ptr<Kml::Vectors> vectors2(new Kml::Vectors);
//
//        vectors2->SetStreetVectors(lineStringStreet);
//        vecStreets->push_back(vectors2);//we're starting a new line, push the old one to the stack
//    }
//
//    for(int i = 0 ; i < vecStreets->size() ; i++)
//    {
//        boost::shared_ptr<Kml::Vectors> linestringVec = (*(vecStreets.get()))[i];
//        std::vector<boost::shared_ptr<Kml::Point> > points = linestringVec->GetStreetVectors();
//        for(int k = 0 ; k < points.size() ; k++)
//        {
//            boost::shared_ptr<Kml::Point> singlepoint = points[k];
//        }
//
//    }
//}

void geovectileparser::parseLineStringStreetName(const boost::shared_ptr<std::vector<uint32_t> > commands,
                                                 int vectilex, int vectiley, int zoomlevel, int tilesize, boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > vecStreets,
                                                 std::string name)
{
    int s = commands->size();
    int currentx = 0;
    int currenty = 0;
    BoundingBox bb = tile2boundingBox(vectilex, vectiley, zoomlevel);
    std::vector<boost::shared_ptr<Kml::Point> > lineStringOutline;

    std::vector<boost::shared_ptr<Kml::Point> > lineStringStreet;
    for(int i = 0 ; i < commands->size() ; )
    {
        int id = getId((*(commands.get()))[i]);
        int count = getCount((*(commands.get()))[i]);
        int commandInd = getCommandInteger(id, count);
        double lat = 0.00;
        double lon = 0.00;
        boost::shared_ptr<Kml::Point> point;
        i++;
        for(int k = 0 ; k < count ; k++)
        {
            bool inside = true;
            switch(id)
            {
                case 1:
                    //start a line
                    //if linestring exists, push it onto the streetvertices stack
                    if(lineStringStreet.size() > 1)
                    {
                        //create a kml::vector from linestring and push it onto vectorlist
                        boost::shared_ptr<Kml::Vectors> vectors1(new Kml::Vectors);
                        vectors1->SetStreetVectors(lineStringStreet);
                        vectors1->SetLabel(name);
                        vecStreets->push_back(vectors1);//we're starting a new line, push the old one to the stack
                    }
                    parseMoveTo(commands, i, currentx, currenty);//, bb, lat, lon);
                    inside = interpolateXY(bb, currentx, currenty, tilesize, lat, lon);
                    point.reset(new Kml::Point);
                    point->SetLat(lat);
                    point->SetLon(lon);
                    lineStringStreet.clear();
                    lineStringStreet.push_back(point);
                    break;
                case 2:
                    //add to existing line
                    parseLineTo(commands, i, currentx, currenty);//, bb, lat, lon);
                    interpolateXY(bb, currentx, currenty, tilesize, lat, lon);
                    point.reset(new Kml::Point);
                    point->SetLat(lat);
                    point->SetLon(lon);

                    lineStringStreet.push_back(point);
                    break;
                default:
                    break;
            }
        }
    }
    if(lineStringStreet.size() > 1)
    {
        //create a kml::vector from linestring and push it onto vectorlist
        boost::shared_ptr<Kml::Vectors> vectors2(new Kml::Vectors);
        vectors2->SetStreetVectors(lineStringStreet);
        vectors2->SetLabel(name);
        vecStreets->push_back(vectors2);//we're starting a new line, push the old one to the stack
    }

    for(int i = 0 ; i < vecStreets->size() ; i++)
    {
        boost::shared_ptr<Kml::Vectors> linestringVec = (*(vecStreets.get()))[i];
        std::vector<boost::shared_ptr<Kml::Point> > points = linestringVec->GetStreetVectors();
        for(int k = 0 ; k < points.size() ; k++)
        {
            boost::shared_ptr<Kml::Point> singlepoint = points[k];
        }

    }
}

boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > geovectileparser::parsePlace(const boost::shared_ptr<std::vector<uint32_t> > commands, int vectilex, int vectiley, int zoomlevel,
                                                                                             int tilesize, kmldom::StylePtr ptrStyle, std::string name)
{
    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > placeList(new std::vector<boost::shared_ptr<Kml::Point> > );
    int s = commands->size();
    int currentx = 0;
    int currenty = 0;
    BoundingBox bb = tile2boundingBox(vectilex, vectiley, zoomlevel);

    for(int i = 0 ; i < commands->size() ; i++)
    {
        std::vector<uint32_t> vec = *(commands.get());
        uint32_t id = getId(vec[i]);
        int count = getCount((*(commands.get()))[i]);
        int commandInd = getCommandInteger(id, count);
        double lat = 0.00;
        double lon = 0.00;
        boost::shared_ptr<Kml::Point> point;
        bool insideTile = true;
        for(int k = 0 ; k < count ; k++)
        {
            switch(id)
            {
                case 1:
                    //move to point
                    parseMoveTo(commands, (++i), currentx, currenty);
                    insideTile = interpolateXY(bb, currentx, currenty, tilesize, lat, lon);
                    if(insideTile)
                    {
                        point.reset(new Kml::Point);
                        point->SetLat(lat);
                        point->SetLon(lon);
                        point->SetName(name);
                        point->SetStyle(ptrStyle);
                        placeList->push_back(point);
                        break;
                    }
                    break;
                case 2:
                    break;
                default:
                    break;
            }
        }
    }
    for(int i = 0 ; i < placeList->size() ; i++)
    {
        boost::shared_ptr<Kml::Point> placePoint = (*(placeList.get()))[i];
    }
    return placeList;
}
