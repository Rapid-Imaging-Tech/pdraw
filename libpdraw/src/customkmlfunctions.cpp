#include "customkmlfunctions.h"
#include "ErrorCodes.h"
using Kml::CustomKmlFunctions;
using namespace kmldom;
using namespace kmlengine;
using namespace Kml;
namespace Kml {


CustomKmlFunctions::CustomKmlFunctions()
{
    //_statusLogger.reset();
}

CustomKmlFunctions::~CustomKmlFunctions()
{
}
#include "kml/dom/folder.h"
#include "kml/dom/kml_cast.h"
#include "kml/dom/kml_factory.h"
#include "kml/dom/kml_funcs.h"
#include "kml/dom/kml_ptr.h"

int CustomKmlFunctions::ParseXMLData(std::string xmlData)
{
    std::string errors;

    //  kmldom::ElementPtr root;// = Parse(xmlData, &errors);
    //AsFolder(root);
    // Parse it.
    //this->_placemark_vector
    this->_kmlFile.assign(xmlData);//copy kml string to this copy
    /*kmlengine::KmlFile* fle = kmlengine::KmlFile::CreateFromString(xmlData);
     kmldom::FeaturePtr feature;
     kmldom::StylePtr style = kmlengine::CreateResolvedStyle(feature, fle, kmldom::STYLESTATE_NORMAL);*/


    //std::string errors;
    KmlFilePtr kml_file;
    try
    {
        kml_file = KmlFile::CreateFromParse(xmlData, &errors);
        if (!kml_file)
        {
            std::string err("invalid kml from kmlserver: ");
            err.append(errors);
            //this->WriteToStatusLog(err, true, true);
            //if(_statusLogger)
            //this->_statusLogger->AddStatus(err);
            //cout << errors << endl;
            return INVALIDKML;
        }
        const kmldom::FolderPtr fldr = AsFolder(kml_file->get_root());
        // fldr->get_name();
        //std::cout<<"found folder"<<std::endl;
    }
    catch(...)
    {
        //this->WriteToStatusLog("invalid kml from kmlserver", true, true);
        return INVALIDKML;
    }
    if (!kml_file)
    {
        std::string err("invalid kml from kmlserver: ");
        err.append(errors);
        //this->WriteToStatusLog(err, true, true);
        //if(_statusLogger)
        //this->_statusLogger->AddStatus(err);
        //cout << errors << endl;
        return INVALIDKML;
    }
    FeaturePtr fp;
    try
    {
        fp = kmlengine::GetRootFeature(kml_file->get_root());
    }
    catch(...)
    {
        //        this->_statusLogger->AddStatus("invalid kml sent from server");
        //this->WriteToStatusLog("invalid kml from server", true, true);
        return INVALIDKML;
    }
    // SavePlacemarks(fp, &this->_placemark_vector, "");
    std::vector<std::string> pathElements;
    SavePlacemarks(fp, this->_placemarksWithPath, "", pathElements);
    //sort(this->_placemark_vector.begin(), this->_placemark_vector.end(), ComparePlacemarks());
    sort(this->_placemarksWithPath.begin(), this->_placemarksWithPath.end(), ComparePlacemarks());
    return this->_placemarksWithPath.size();
}

int CustomKmlFunctions::ParseFile(const char * filename, int uid, std::vector<std::string>& newIcons)
{
    //
    using namespace kmldom;
    using namespace kmlengine;
    _kmlFile.clear();
    _kmlFile.append(filename);
    std::string str("parsing file - ");
    std::string str2(filename);
    str.append(str2);
    std::vector<std::string> jj;

    FeaturePtr featurePtr = GetKmlFileRootFeature(filename, uid, newIcons);

    ////////////////////////////
    /// \brief fldr
    //    ContainerPtr cp = kmldom::AsContainer(featurePtr);
    //    //cp->feature_array_.size();
    //        if (cp) {
    //            std::string contName = cp->get_name();
    //            std::cout<<"**container name = "<<contName<<std::endl;
    //            //std::cout<<"recurse through container"<<std::endl;
    //            //cp->get_feature_array_at()
    //            for (int i = 0; i < cp->get_feature_array_size(); i++)
    //            {
    //                kmldom::FeaturePtr featPtr2 = cp->get_feature_array_at(i);
    //                if(featPtr2->Type() == Type_Placemark)
    //                {
    //                    std::cout<<"found a placemark";
    //                    const kmldom::PlacemarkPtr pmPtr = AsPlacemark(featPtr2);
    //                    std::string pmName = pmPtr->get_name();
    //                    std::cout<<"placemark Name ="<<pmName<<std::endl;
    //                }
    //                if(featPtr2->Type() == Type_Folder)
    //                {
    //                    const kmldom::FolderPtr fldr = AsFolder(featPtr2);
    //                    //std::string dgddgd = fldr->get_id();
    //                    std::string fldrName = fldr->get_name();
    //                    std::cout<<"folder name ="<<fldrName<<std::endl;
    //                    ContainerPtr cp2 = kmldom::AsContainer(featPtr2);

    //                    if (cp2) {
    //                        for (int ll = 0; ll < cp2->get_feature_array_size(); ll++) {
    //                            kmldom::FeaturePtr featPtr3 = cp2->get_feature_array_at(ll);
    //                            if(featPtr3->Type() == Type_Placemark)
    //                            {
    //                                std::cout<<"found a placemark";
    //                                const kmldom::PlacemarkPtr pmPtr2 = AsPlacemark(featPtr3);
    //                                std::string pmName2 = pmPtr2->get_name();
    //                                std::cout<<"placemark Name ="<<pmName2<<std::endl;
    //                            }
    //                            if(featPtr3->Type() == Type_Folder)
    //                            {
    //                                const kmldom::FolderPtr fldr2 = AsFolder(featPtr3);
    //                                //std::string dgddgd = fldr->get_id();
    //                                std::string fldrName2 = fldr2->get_name();
    //                                std::cout<<"folder name ="<<fldrName2<<std::endl;
    //                                //ContainerPtr cp2 = kmldom::AsContainer(featPtr3);

    //                            }
    //                        }
    //                    }
    //                }

    //              }
    //                  //VisitFeature($container->feature_array_at($i), $depth);
    //            }

    // std::cout<<"found a container";

    /////////////////////////
    /// \brief fldr
    ///



    if(featurePtr == NULL){
        return -1;
    }

    std::string strFileName(filename);
    //SavePlacemarks(featurePtr, &this->_placemark_vector, strFileName);
    //sort(this->_placemark_vector.begin(), this->_placemark_vector.end(), ComparePlacemarks());
    std::vector<std::string> pathElements;
    SavePlacemarks(featurePtr, this->_placemarksWithPath, strFileName, pathElements);
    sort(this->_placemarksWithPath.begin(), this->_placemarksWithPath.end(), ComparePlacemarks());
    return this->_placemarksWithPath.size();
}

boost::shared_ptr<std::vector<boost::shared_ptr<StreetLabel> > > CustomKmlFunctions::GetStreetLabels()
{

    ////////////////////////////////////////////////////
    //grab multigeom
    bool skip = false;
    boost::shared_ptr<std::vector<boost::shared_ptr<StreetLabel> > > _streetLabels(new std::vector<boost::shared_ptr<StreetLabel> >);

    std::vector<boost::shared_ptr<Point> > labelPoints;
    kmlengine::KmlFilePtr fle;
    try
    {
        fle = kmlengine::KmlFile::CreateFromString(_kmlFile);
    }
    catch(...)
    {
        boost::shared_ptr<std::vector<boost::shared_ptr<StreetLabel> > > empty;
        return empty;
    }

    //for(int i = 0 ; i < _placemark_vector.size() ; i++)
    for(int i = 0 ; i < _placemarksWithPath.size() ; i++)
    {
        PlacemarkPtr ptrPmark = static_cast<PlacemarkWithPath>(_placemarksWithPath[i])._placeMarkPtr;
        //kmldom::GeometryPtr geomptr1 = this->_placemark_vector[i]->get_geometry();
        kmldom::GeometryPtr geomptr1 = ptrPmark->get_geometry();
        if(!geomptr1)
            continue;
        kmldom::KmlDomType domtype = geomptr1->Type();
        if(domtype == kmldom::Type_Polygon)
        {
            //std::cout<<"found a polygon"<<std::endl;
        }
        if(domtype == kmldom::Type_Folder)
        {
            // std::cout<<"found folder"<<std::endl;
        }
        if(domtype == kmldom::Type_MultiGeometry)
        {
            const kmldom::MultiGeometryPtr kmlmulti = kmldom::AsMultiGeometry(geomptr1);
            if(kmlmulti )
            {
                // KML has one mutli geometry type
                // because map and mapguide can't display multigeomtry type
                // I need to try to create multi-point,line,polygon types when possiblle.
                //FdoGeometryType alltype = FdoGeometryType_None;
                //int counttypes = 0;

                //FdoPtr<FdoGeometryCollection> fdogmcoll = FdoGeometryCollection::Create();
                boost::shared_ptr<StreetLabel> stLabel(new StreetLabel);
                int count = kmlmulti->get_geometry_array_size();
                for(int ind=0 ; ind < count ; ind++)
                {
                    const kmldom::GeometryPtr geomptr2 = kmlmulti->get_geometry_array_at(ind);
                    kmldom::KmlDomType domtype = geomptr2->Type();
                    if(domtype == kmldom::Type_Folder)
                    {
                        //std::cout<<"found folder"<<std::endl;
                    }
                    //boost::shared_ptr<Kml::StreetLabel> stLabel(new Kml::StreetLabel);
                    if( domtype == kmldom::Type_Polygon ||domtype == kmldom::Type_LinearRing)
                    {
                        std::cout<<"found line ring"<<std::endl;
                    }
                    if( domtype == kmldom::Type_LineString )
                    {
                        std::vector<boost::shared_ptr<Point> > points;
                        kmldom::PointPtr poKmlPoint = boost::static_pointer_cast<kmldom::Point>(geomptr2);
                        if(poKmlPoint->has_coordinates())
                        {
                            kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoint->get_coordinates();
                            size_t arraySize = poKmlCoordinates->get_coordinates_array_size();
                            for(int j = 0 ; j < arraySize ; j++)
                            {
                                kmlbase::Vec3 oKmlVec = poKmlCoordinates->get_coordinates_array_at(j);
                                boost::shared_ptr<Point> point(new Point);
                                point->SetLat(oKmlVec.get_latitude());
                                point->SetLon(oKmlVec.get_longitude());
                                point->SetAlt(oKmlVec.get_altitude());
                                std::stringstream ss;
                                ss<<"vec"<<j;
                                point->SetName(ss.str());
                                points.push_back(point);
                            }
                            PlacemarkWithPath pp = _placemarksWithPath[i];
                            kmldom::StylePtr style = kmlengine::CreateResolvedStyle(pp._placeMarkPtr, fle, kmldom::STYLESTATE_NORMAL);
                            if(style->has_labelstyle())
                            {
                                stLabel->SetStyle(style);
                            }
                            stLabel->SetLabel(pp._placeMarkPtr->get_name());
                            stLabel->SetDirection(points);
                        }
                        else
                        {
                            skip = true;
                        }
                    }
                    if( domtype == kmldom::Type_Point )
                    {
                        kmldom::PointPtr poKmlPoint = boost::static_pointer_cast<kmldom::Point>(geomptr2);
                        kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoint->get_coordinates();
                        kmlbase::Vec3 oKmlVec = poKmlCoordinates->get_coordinates_array_at(0);
                        double lat = 0.00;
                        double lon = 0.00;
                        double altitude = 0.00;
                        boost::shared_ptr<Point> pt(new Point);
                        pt->SetLat(oKmlVec.get_latitude());
                        pt->SetLon(oKmlVec.get_longitude());
                        pt->SetAlt(oKmlVec.get_altitude());
                        PlacemarkWithPath pp = _placemarksWithPath[i];
                        pt->SetName(pp._placeMarkPtr->get_name());
                        stLabel->SetLabelPoint(pt);
                        //points.push_back(pt);
                    }

                }
                if(!skip)
                {
                    _streetLabels->push_back(stLabel);
                }
                skip = false;
            }
            else
            {
                std::cout<<"not multi"<<std::endl;
            }
        }
    }
    return _streetLabels;
}
boost::shared_ptr<std::vector<boost::shared_ptr<Vectors> > > CustomKmlFunctions::GetStreets(std::string filename)
{
    boost::shared_ptr<std::vector<boost::shared_ptr<Vectors> > > spStreetPoints(new std::vector<boost::shared_ptr<Vectors> >);
    kmlengine::KmlFilePtr fle = kmlengine::KmlFile::CreateFromString(_kmlFile);
    if(filename.length() == 0)
        fle = kmlengine::KmlFile::CreateFromString(_kmlFile);
    else
    {
        fle = kmlengine::KmlFile::CreateFromString(GetKmlFromFile(_kmlFile));
    }
    ///////////////////////////////////////////////////
    //for(int i = 0 ; i < _placemark_vector.size() ; i++)
    for(int i = 0 ; i < _placemarksWithPath.size() ; i++)
    {
        PlacemarkWithPath pp = _placemarksWithPath[i];
        kmldom::GeometryPtr gptr = pp._placeMarkPtr->get_geometry();
        if(!gptr)
            continue;
        kmldom::PointPtr poKmlPoint = boost::static_pointer_cast<kmldom::Point>(gptr);
        kmldom::PolygonPtr poKmlPoly = boost::static_pointer_cast<kmldom::Polygon>(gptr);

        kmldom::KmlDomType domtype = poKmlPoint->Type();
        //see this site for further details in how to parse out multigeometry
        //https://svn.osgeo.org/fdo/trunk/Providers/KingKML/src/Provider/c_KgFdoPlacemark.cpp
        if(poKmlPoint->has_coordinates() && domtype == kmldom::Type_Polygon )
        {
            std::cout<<"found polygon"<<std::endl;
        }
        if(poKmlPoint->has_coordinates() && domtype == kmldom::Type_LineString )
        {
            std::vector<boost::shared_ptr<Point> > points;
            kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoint->get_coordinates();
            //std::vector<kmlbase::Vec3> vc = poKmlCoordinates->coordinates_array_;
            size_t arraySize = poKmlCoordinates->get_coordinates_array_size();
            //kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoly->get;
            //size_t arraySize = poKmlCoordinates->get_coordinates_array_size();
            for(int j = 0 ; j < arraySize ; j++)
            {
                kmlbase::Vec3 oKmlVec = poKmlCoordinates->get_coordinates_array_at(j);
                boost::shared_ptr<Point> point(new Point);
                point->SetLat(oKmlVec.get_latitude());
                point->SetLon(oKmlVec.get_longitude());
                point->SetAlt(oKmlVec.get_altitude());
                std::stringstream ss;
                ss<<"vec"<<j;
                point->SetName(ss.str());
                points.push_back(point);
            }
            //////todo: grab the color from style
            PlacemarkWithPath pp = _placemarksWithPath[i];
            kmldom::StylePtr style = kmlengine::CreateResolvedStyle(pp._placeMarkPtr, fle, kmldom::STYLESTATE_NORMAL);

            boost::shared_ptr<Vectors> stVec(new Vectors);
            PlacemarkWithPath pp2 = _placemarksWithPath[i];

            stVec->_kmlPathDetails = pp2._kmlPathDetails;


            std::string nm = pp2._placeMarkPtr->get_name();
            if(nm.compare("") != 0)
            {
                std::stringstream ss;
                ss<<"num points = "<<points.size();
            }
            stVec->SetLabel(pp2._placeMarkPtr->get_name());
            stVec->SetStreetVectors(points);

            stVec->SetStyle(style);
            spStreetPoints->push_back(stVec);
        }
    }
    /////////////////////////////////////////////
    return spStreetPoints;
}
boost::shared_ptr<std::vector<boost::shared_ptr<Vectors> > > CustomKmlFunctions::GetPolygons()
{
    boost::shared_ptr<std::vector<boost::shared_ptr<Vectors> > > spPolygonVectors(new std::vector<boost::shared_ptr<Vectors> >);
    //std::vector<boost::shared_ptr<Kml::Vectors> > polygonVectors;
    //kmlengine::KmlFilePtr fle = kmlengine::KmlFile::CreateFromString(_kmlFile);
    kmlengine::KmlFilePtr fle = kmlengine::KmlFile::CreateFromString(GetKmlFromFile(_kmlFile));
    for(int i = 0 ; i < _placemarksWithPath.size() ; i++)
    {
        PlacemarkWithPath pp = _placemarksWithPath[i];
        kmldom::GeometryPtr gptr = pp._placeMarkPtr->get_geometry();
        if(!gptr)
            continue;
        kmldom::PolygonPtr poKmlPoint = boost::static_pointer_cast<kmldom::Polygon>(gptr);
        kmldom::KmlDomType domtype = poKmlPoint->Type();

        if(domtype == kmldom::Type_Polygon )
        {
            kmldom::OuterBoundaryIsPtr outerBoundaryIs = poKmlPoint->get_outerboundaryis();
            std::vector<boost::shared_ptr<Point> > points;
            if(outerBoundaryIs )
            {
                kmldom::LinearRingPtr linearRing = outerBoundaryIs->get_linearring();
                if(linearRing)
                {
                    kmldom::CoordinatesPtr coordinates = linearRing->get_coordinates();//();
                    if(coordinates)
                    {
                        for(int j = 0 ; j < coordinates->get_coordinates_array_size() ; j++)
                        {
                            const kmlbase::Vec3 vec3 = coordinates->get_coordinates_array_at(j);
                            boost::shared_ptr<Point> point(new Point);
                            point->SetLat(vec3.get_latitude());
                            point->SetLon(vec3.get_longitude());
                            point->SetAlt(vec3.get_altitude());
                            std::stringstream ss;
                            ss<<"vec"<<j;
                            point->SetName(ss.str());
                            points.push_back(point);
                        }
                    }
                }
            }

            PlacemarkWithPath pp2 = _placemarksWithPath[i];
            kmldom::StylePtr style = kmlengine::CreateResolvedStyle(pp2._placeMarkPtr, fle, kmldom::STYLESTATE_NORMAL);

            boost::shared_ptr<Vectors> polygonVec(new Vectors);

            // PlacemarkWithPath pp2 = _placemarksWithPath[i];

            polygonVec->_kmlPathDetails = pp2._kmlPathDetails;


            polygonVec->SetIsPolygon(true);
            PlacemarkWithPath pp3 = _placemarksWithPath[i];
            std::string nm = pp3._placeMarkPtr->get_name();
            if(nm.compare("") != 0)
            {
                std::stringstream ss;
                ss<<"num points = "<<points.size();
            }

            polygonVec->SetLabel(pp3._placeMarkPtr->get_name());
            polygonVec->SetStreetVectors(points);

            polygonVec->SetStyle(style);
            spPolygonVectors->push_back(polygonVec);
        }
    }
    /////////////////////////////////////////////
    return spPolygonVectors;
}

std::string CustomKmlFunctions::GetKmlFromFile(std::string filename)
{
    std::ifstream t(filename.c_str());
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;
}

boost::shared_ptr<std::vector<boost::shared_ptr<Point> > > CustomKmlFunctions::GetPoints(std::string filename)
{

    //todo: have to group kml::point with
    boost::shared_ptr<std::vector<boost::shared_ptr<Point> > > points(new std::vector<boost::shared_ptr<Point> >);


    //GetKmlFromFile(_kmlFile);
    //kmlengine::KmlFilePtr fle = kmlengine::KmlFile::CreateFromString(GetKmlFromFile(_kmlFile));
    kmlengine::KmlFilePtr fle;
    if(filename.length() == 0)
        fle = kmlengine::KmlFile::CreateFromString(_kmlFile);
    else
    {
        fle = kmlengine::KmlFile::CreateFromString(GetKmlFromFile(_kmlFile));
    }


    for(int i = 0 ; i < _placemarksWithPath.size() ; i++)
    {
        boost::shared_ptr<Point> pt(new Point);

        PlacemarkWithPath pp = _placemarksWithPath[i];
        //boost::shared_ptr<Kml::Point> pt = pp._filterItems->;//(new Kml::Point);
        //boost::shared_ptr<Kml::Point> pt(new Kml::Point);
        kmldom::GeometryPtr gptr = pp._placeMarkPtr->get_geometry();
        if(!gptr)
            continue;
        kmldom::PointPtr poKmlPoint = boost::static_pointer_cast<kmldom::Point>(gptr);
        kmldom::KmlDomType domtype = poKmlPoint->Type();
        //see this site for further details in how to parse out multigeometry
        //https://svn.osgeo.org/fdo/trunk/Providers/KingKML/src/Provider/c_KgFdoPlacemark.cpp
        if(poKmlPoint->has_coordinates() && domtype == kmldom::Type_Point)
        {
            //poKmlPoint->
            kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoint->get_coordinates();
            kmlbase::Vec3 oKmlVec = poKmlCoordinates->get_coordinates_array_at(0);
            double lat = 0.00;
            double lon = 0.00;
            double altitude = 0.00;
            //pt->_point->

            pt->SetLat(oKmlVec.get_latitude());
            pt->SetLon(oKmlVec.get_longitude());
            pt->SetAlt(oKmlVec.get_altitude());
            PlacemarkWithPath pp = _placemarksWithPath[i];
            pt->SetName(pp._placeMarkPtr->get_name());
            pt->_kmlPathDetails = pp._kmlPathDetails;
            kmldom::StylePtr style = kmlengine::CreateResolvedStyle(pp._placeMarkPtr, fle, kmldom::STYLESTATE_NORMAL);
            pt->SetStyle(style);
            points->push_back(pt);
        }

    }
    return points;
}

void CustomKmlFunctions::TestKmlOutput(std::vector<coordinate> coords)
{
    using kmldom::CoordinatesPtr;
    using kmldom::KmlPtr;
    using kmldom::KmlFactory;
    using kmldom::PlacemarkPtr;
    using kmldom::PointPtr;
    // Get the factory singleton to create KML elements.
    KmlFactory* factory = KmlFactory::GetFactory();
    KmlPtr kml = factory->CreateKml();
    placemark_vector_t vec;
    PlacemarkPtr  placemark;
    std::string statusString;
    statusString.append("<kml>");
    for(int i = 0 ; i < coords.size() ; i++)
    {
        statusString.append("<Placemark>");
        statusString.append("<Point>");
        statusString.append("<coordintates>");
        double lon = coords[i].longitude;
        double lat = coords[i].latitude;
        statusString.append("0");//place altitude here eventually
        statusString.append("</coordintates>");
        statusString.append("</Point>");
        statusString.append("</Placemark>");
    }
    statusString.append("</kml>");

    // Print to stdout
    std::cout << statusString;
}

double CustomKmlFunctions::GetValueDouble(TYPE type, int index)
{
    if(index >= this->_placemarksWithPath.size())
    {
        return 0.00;//todo: maybe throw exception instead??
    }
    else
    {
        PlacemarkWithPath pp = _placemarksWithPath[index];
        kmldom::GeometryPtr gptr = pp._placeMarkPtr->get_geometry();
        kmldom::PointPtr poKmlPoint = boost::static_pointer_cast<kmldom::Point>(gptr);
        kmldom::KmlDomType domtype = poKmlPoint->Type();
        //see this site for further details in how to parse out multigeometry
        //https://svn.osgeo.org/fdo/trunk/Providers/KingKML/src/Provider/c_KgFdoPlacemark.cpp
        if(poKmlPoint->has_coordinates() && (domtype == kmldom::Type_LineString ||domtype == kmldom::Type_Point))
        {
            //poKmlPoint->
            kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoint->get_coordinates();
            kmlbase::Vec3 oKmlVec = poKmlCoordinates->get_coordinates_array_at(0);
            double lat = 0.00;
            double lon = 0.00;
            double altitude = 0.00;
            switch(type)
            {
                case LATITUDE:
                    lat = oKmlVec.get_latitude();
                    return lat;
                    break;
                case LONGITUDE:
                    lon = oKmlVec.get_longitude();
                    return lon;
                    break;
                case ELEVATION:
                    altitude = oKmlVec.get_altitude();
                    return altitude;
                    break;
                default:
                    return 0.00;
                    break;
            }
        }
        else
        {
            return 0.00;
        }
    }
}
}



//#include <customkmlfunctions.h>
//#include <ErrorCodes.h>
//using Kml::CustomKmlFunctions;
//using namespace kmldom;
//using namespace kmlengine;


//CustomKmlFunctions::CustomKmlFunctions()
//{
//    //_statusLogger.reset();
//}

//CustomKmlFunctions::~CustomKmlFunctions()
//{
//}
//#include "kml/dom/folder.h"
//#include "kml/dom/kml_cast.h"
//#include "kml/dom/kml_factory.h"
//#include "kml/dom/kml_funcs.h"
//#include "kml/dom/kml_ptr.h"

//int CustomKmlFunctions::ParseXMLData(std::string xmlData)
//{
//    std::string errors;

//  //  kmldom::ElementPtr root;// = Parse(xmlData, &errors);
////AsFolder(root);
//    // Parse it.
//    //this->_placemark_vector
//    this->_kmlFile.assign(xmlData);//copy kml string to this copy
//    /*kmlengine::KmlFile* fle = kmlengine::KmlFile::CreateFromString(xmlData);
//    kmldom::FeaturePtr feature;
//    kmldom::StylePtr style = kmlengine::CreateResolvedStyle(feature, fle, kmldom::STYLESTATE_NORMAL);*/


//    //std::string errors;
//    KmlFilePtr kml_file;
//    try
//    {
//        kml_file = KmlFile::CreateFromParse(xmlData, &errors);
//        if (!kml_file)
//        {
//            std::string err("invalid kml from kmlserver: ");
//            err.append(errors);
//            //this->WriteToStatusLog(err, true, true);
//            //if(_statusLogger)
//                //this->_statusLogger->AddStatus(err);
//            //cout << errors << endl;
//            return INVALIDKML;
//        }
//        const kmldom::FolderPtr fldr = AsFolder(kml_file->get_root());
//       // fldr->get_name();
//        //std::cout<<"found folder"<<std::endl;
//    }
//    catch(...)
//    {
//        //this->WriteToStatusLog("invalid kml from kmlserver", true, true);
//        return INVALIDKML;
//    }
//    if (!kml_file)
//    {
//        std::string err("invalid kml from kmlserver: ");
//        err.append(errors);
//        //this->WriteToStatusLog(err, true, true);
//        //if(_statusLogger)
//            //this->_statusLogger->AddStatus(err);
//        //cout << errors << endl;
//        return INVALIDKML;
//    }
//    FeaturePtr fp;
//    try
//    {
//        fp = kmlengine::GetRootFeature(kml_file->get_root());
//    }
//    catch(...)
//    {
////		this->_statusLogger->AddStatus("invalid kml sent from server");
//        //this->WriteToStatusLog("invalid kml from server", true, true);
//        return INVALIDKML;
//    }
//   // SavePlacemarks(fp, &this->_placemark_vector, "");
//    std::vector<std::string> pathElements;
//     SavePlacemarks(fp, this->_placemarksWithPath, "", pathElements);
//    //sort(this->_placemark_vector.begin(), this->_placemark_vector.end(), ComparePlacemarks());
//     sort(this->_placemarksWithPath.begin(), this->_placemarksWithPath.end(), ComparePlacemarks());
//    return this->_placemarksWithPath.size();
//}

//int CustomKmlFunctions::ParseFile(const char * filename, int uid, std::vector<std::string>& newIcons)
//{
//    //
//    using namespace kmldom;
//    using namespace kmlengine;
//    _kmlFile.clear();
//    _kmlFile.append(filename);
//    std::string str("parsing file - ");
//    std::string str2(filename);
//    str.append(str2);
//    std::vector<std::string> jj;

//    FeaturePtr featurePtr = GetKmlFileRootFeature(filename, uid, newIcons);

//    ////////////////////////////
//    /// \brief fldr
////    ContainerPtr cp = kmldom::AsContainer(featurePtr);
////    //cp->feature_array_.size();
////        if (cp) {
////            std::string contName = cp->get_name();
////            std::cout<<"**container name = "<<contName<<std::endl;
////            //std::cout<<"recurse through container"<<std::endl;
////            //cp->get_feature_array_at()
////            for (int i = 0; i < cp->get_feature_array_size(); i++)
////            {
////                kmldom::FeaturePtr featPtr2 = cp->get_feature_array_at(i);
////                if(featPtr2->Type() == Type_Placemark)
////                {
////                    std::cout<<"found a placemark";
////                    const kmldom::PlacemarkPtr pmPtr = AsPlacemark(featPtr2);
////                    std::string pmName = pmPtr->get_name();
////                    std::cout<<"placemark Name ="<<pmName<<std::endl;
////                }
////                if(featPtr2->Type() == Type_Folder)
////                {
////                    const kmldom::FolderPtr fldr = AsFolder(featPtr2);
////                    //std::string dgddgd = fldr->get_id();
////                    std::string fldrName = fldr->get_name();
////                    std::cout<<"folder name ="<<fldrName<<std::endl;
////                    ContainerPtr cp2 = kmldom::AsContainer(featPtr2);

////                    if (cp2) {
////                        for (int ll = 0; ll < cp2->get_feature_array_size(); ll++) {
////                            kmldom::FeaturePtr featPtr3 = cp2->get_feature_array_at(ll);
////                            if(featPtr3->Type() == Type_Placemark)
////                            {
////                                std::cout<<"found a placemark";
////                                const kmldom::PlacemarkPtr pmPtr2 = AsPlacemark(featPtr3);
////                                std::string pmName2 = pmPtr2->get_name();
////                                std::cout<<"placemark Name ="<<pmName2<<std::endl;
////                            }
////                            if(featPtr3->Type() == Type_Folder)
////                            {
////                                const kmldom::FolderPtr fldr2 = AsFolder(featPtr3);
////                                //std::string dgddgd = fldr->get_id();
////                                std::string fldrName2 = fldr2->get_name();
////                                std::cout<<"folder name ="<<fldrName2<<std::endl;
////                                //ContainerPtr cp2 = kmldom::AsContainer(featPtr3);

////                            }
////                        }
////                    }
////                }

////              }
////                  //VisitFeature($container->feature_array_at($i), $depth);
////            }

//           // std::cout<<"found a container";

//            /////////////////////////
//            /// \brief fldr
//            ///



//    if(featurePtr == NULL){
//        return -1;
//    }

//    std::string strFileName(filename);
//    //SavePlacemarks(featurePtr, &this->_placemark_vector, strFileName);
//    //sort(this->_placemark_vector.begin(), this->_placemark_vector.end(), ComparePlacemarks());
//    std::vector<std::string> pathElements;
//    SavePlacemarks(featurePtr, this->_placemarksWithPath, strFileName, pathElements);
//    sort(this->_placemarksWithPath.begin(), this->_placemarksWithPath.end(), ComparePlacemarks());
//    return this->_placemarksWithPath.size();
//}

//boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > CustomKmlFunctions::GetStreetLabels()
//{
//    ////////////////////////////////////////////////////
//    //grab multigeom
//    bool skip = false;
//    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > _streetLabels(new std::vector<boost::shared_ptr<Kml::StreetLabel> >);

//    std::vector<boost::shared_ptr<Kml::Point> > labelPoints;
//    kmlengine::KmlFilePtr fle;
//    try
//    {
//        fle = kmlengine::KmlFile::CreateFromString(_kmlFile);
//    }
//    catch(...)
//    {
//        boost::shared_ptr<std::vector<boost::shared_ptr<Kml::StreetLabel> > > empty;
//        return empty;
//    }

//    //for(int i = 0 ; i < _placemark_vector.size() ; i++)
//    for(int i = 0 ; i < _placemarksWithPath.size() ; i++)
//    {
//        PlacemarkPtr ptrPmark = static_cast<PlacemarkWithPath>(_placemarksWithPath[i])._placeMarkPtr;
//        //kmldom::GeometryPtr geomptr1 = this->_placemark_vector[i]->get_geometry();
//        kmldom::GeometryPtr geomptr1 = ptrPmark->get_geometry();
//        if(!geomptr1)
//            continue;
//        kmldom::KmlDomType domtype = geomptr1->Type();
//        if(domtype == kmldom::Type_Polygon)
//        {
//            //std::cout<<"found a polygon"<<std::endl;
//        }
//        if(domtype == kmldom::Type_Folder)
//        {
//           // std::cout<<"found folder"<<std::endl;
//        }
//        if(domtype == kmldom::Type_MultiGeometry)
//        {
//            const kmldom::MultiGeometryPtr kmlmulti = kmldom::AsMultiGeometry(geomptr1);
//            if(kmlmulti )
//            {
//                // KML has one mutli geometry type
//                // because map and mapguide can't display multigeomtry type
//                // I need to try to create multi-point,line,polygon types when possiblle.
//                //FdoGeometryType alltype = FdoGeometryType_None;
//                //int counttypes = 0;

//                //FdoPtr<FdoGeometryCollection> fdogmcoll = FdoGeometryCollection::Create();
//                boost::shared_ptr<Kml::StreetLabel> stLabel(new Kml::StreetLabel);
//                int count = kmlmulti->get_geometry_array_size();
//                for(int ind=0 ; ind < count ; ind++)
//                {
//                    const kmldom::GeometryPtr geomptr2 = kmlmulti->get_geometry_array_at(ind);
//                    kmldom::KmlDomType domtype = geomptr2->Type();
//                    if(domtype == kmldom::Type_Folder)
//                    {
//                        //std::cout<<"found folder"<<std::endl;
//                    }
//                    //boost::shared_ptr<Kml::StreetLabel> stLabel(new Kml::StreetLabel);
//                    if( domtype == kmldom::Type_Polygon ||domtype == kmldom::Type_LinearRing)
//                    {
//                        std::cout<<"found line ring"<<std::endl;
//                    }
//                    if( domtype == kmldom::Type_LineString )
//                    {
//                        std::vector<boost::shared_ptr<Kml::Point> > points;
//                        kmldom::PointPtr poKmlPoint = boost::static_pointer_cast<kmldom::Point>(geomptr2);
//                        if(poKmlPoint->has_coordinates())
//                        {
//                            kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoint->get_coordinates();
//                            size_t arraySize = poKmlCoordinates->get_coordinates_array_size();
//                            for(int j = 0 ; j < arraySize ; j++)
//                            {
//                                kmlbase::Vec3 oKmlVec = poKmlCoordinates->get_coordinates_array_at(j);
//                                boost::shared_ptr<Kml::Point> point(new Kml::Point);
//                                point->SetLat(oKmlVec.get_latitude());
//                                point->SetLon(oKmlVec.get_longitude());
//                                point->SetAlt(oKmlVec.get_altitude());
//                                std::stringstream ss;
//                                ss<<"vec"<<j;
//                                point->SetName(ss.str());
//                                points.push_back(point);
//                            }
//                            PlacemarkWithPath pp = _placemarksWithPath[i];
//                            kmldom::StylePtr style = kmlengine::CreateResolvedStyle(pp._placeMarkPtr, fle, kmldom::STYLESTATE_NORMAL);
//                            if(style->has_labelstyle())
//                            {
//                                stLabel->SetStyle(style);
//                            }
//                            stLabel->SetLabel(pp._placeMarkPtr->get_name());
//                            stLabel->SetDirection(points);
//                        }
//                        else
//                        {
//                            skip = true;
//                        }
//                    }
//                    if( domtype == kmldom::Type_Point )
//                    {
//                        kmldom::PointPtr poKmlPoint = boost::static_pointer_cast<kmldom::Point>(geomptr2);
//                        kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoint->get_coordinates();
//                        kmlbase::Vec3 oKmlVec = poKmlCoordinates->get_coordinates_array_at(0);
//                        double lat = 0.00;
//                        double lon = 0.00;
//                        double altitude = 0.00;
//                        boost::shared_ptr<Kml::Point> pt(new Kml::Point);
//                        pt->SetLat(oKmlVec.get_latitude());
//                        pt->SetLon(oKmlVec.get_longitude());
//                        pt->SetAlt(oKmlVec.get_altitude());
//                        PlacemarkWithPath pp = _placemarksWithPath[i];
//                        pt->SetName(pp._placeMarkPtr->get_name());
//                        stLabel->SetLabelPoint(pt);
//                        //points.push_back(pt);
//                    }

//                }
//                if(!skip)
//                {
//                    _streetLabels->push_back(stLabel);
//                }
//                skip = false;
//            }
//            else
//            {
//                std::cout<<"not multi"<<std::endl;
//            }
//        }
//    }
//    return _streetLabels;
//}
//boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > CustomKmlFunctions::GetStreets(std::string filename)
//{
//    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > spStreetPoints(new std::vector<boost::shared_ptr<Kml::Vectors> >);
//    kmlengine::KmlFilePtr fle = kmlengine::KmlFile::CreateFromString(_kmlFile);
//    if(filename.length() == 0)
//        fle = kmlengine::KmlFile::CreateFromString(_kmlFile);
//    else
//    {
//        fle = kmlengine::KmlFile::CreateFromString(GetKmlFromFile(_kmlFile));
//    }
//    ///////////////////////////////////////////////////
//    //for(int i = 0 ; i < _placemark_vector.size() ; i++)
//    for(int i = 0 ; i < _placemarksWithPath.size() ; i++)
//    {
//        PlacemarkWithPath pp = _placemarksWithPath[i];
//        kmldom::GeometryPtr gptr = pp._placeMarkPtr->get_geometry();
//        if(!gptr)
//            continue;
//        kmldom::PointPtr poKmlPoint = boost::static_pointer_cast<kmldom::Point>(gptr);
//        kmldom::PolygonPtr poKmlPoly = boost::static_pointer_cast<kmldom::Polygon>(gptr);

//        kmldom::KmlDomType domtype = poKmlPoint->Type();
//        //see this site for further details in how to parse out multigeometry
//        //https://svn.osgeo.org/fdo/trunk/Providers/KingKML/src/Provider/c_KgFdoPlacemark.cpp
//        if(domtype == kmldom::Type_Polygon )
//        {
//            //std::cout<<"found linear ring"<<std::endl;
//        }
//        if(poKmlPoint->has_coordinates() && domtype == kmldom::Type_LineString )
//        {
//            std::vector<boost::shared_ptr<Kml::Point> > points;
//            kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoint->get_coordinates();
//            size_t arraySize = poKmlCoordinates->get_coordinates_array_size();
//            for(int j = 0 ; j < arraySize ; j++)
//            {
//                kmlbase::Vec3 oKmlVec = poKmlCoordinates->get_coordinates_array_at(j);
//                boost::shared_ptr<Kml::Point> point(new Kml::Point);
//                point->SetLat(oKmlVec.get_latitude());
//                point->SetLon(oKmlVec.get_longitude());
//                point->SetAlt(oKmlVec.get_altitude());
//                std::stringstream ss;
//                ss<<"vec"<<j;
//                point->SetName(ss.str());
//                points.push_back(point);
//            }
//            //////todo: grab the color from style
//            PlacemarkWithPath pp = _placemarksWithPath[i];
//            kmldom::StylePtr style = kmlengine::CreateResolvedStyle(pp._placeMarkPtr, fle, kmldom::STYLESTATE_NORMAL);

//            boost::shared_ptr<Kml::Vectors> stVec(new Kml::Vectors);
//            PlacemarkWithPath pp2 = _placemarksWithPath[i];

//            stVec->_kmlPathDetails = pp2._kmlPathDetails;


//            std::string nm = pp2._placeMarkPtr->get_name();
//            if(nm.compare("") != 0)
//            {
//                std::stringstream ss;
//                ss<<"num points = "<<points.size();
//            }
//            stVec->SetLabel(pp2._placeMarkPtr->get_name());
//            stVec->SetStreetVectors(points);

//            stVec->SetStyle(style);
//            spStreetPoints->push_back(stVec);
//        }
//    }
//    /////////////////////////////////////////////
//    return spStreetPoints;
//}
//boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > CustomKmlFunctions::GetPolygons()
//{
//    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Vectors> > > spPolygonVectors(new std::vector<boost::shared_ptr<Kml::Vectors> >);
//    //std::vector<boost::shared_ptr<Kml::Vectors> > polygonVectors;
//    //kmlengine::KmlFilePtr fle = kmlengine::KmlFile::CreateFromString(_kmlFile);
//    kmlengine::KmlFilePtr fle = kmlengine::KmlFile::CreateFromString(GetKmlFromFile(_kmlFile));
//    for(int i = 0 ; i < _placemarksWithPath.size() ; i++)
//    {
//        PlacemarkWithPath pp = _placemarksWithPath[i];
//        kmldom::GeometryPtr gptr = pp._placeMarkPtr->get_geometry();
//        if(!gptr)
//            continue;
//        kmldom::PolygonPtr poKmlPoint = boost::static_pointer_cast<kmldom::Polygon>(gptr);
//        kmldom::KmlDomType domtype = poKmlPoint->Type();

//        if(domtype == kmldom::Type_Polygon )
//        {
//            kmldom::OuterBoundaryIsPtr outerBoundaryIs = poKmlPoint->get_outerboundaryis();
//            std::vector<boost::shared_ptr<Kml::Point> > points;
//            if(outerBoundaryIs )
//            {
//                kmldom::LinearRingPtr linearRing = outerBoundaryIs->get_linearring();
//                if(linearRing)
//                {
//                    kmldom::CoordinatesPtr coordinates = linearRing->get_coordinates();//();
//                    if(coordinates)
//                    {
//                        for(int j = 0 ; j < coordinates->get_coordinates_array_size() ; j++)
//                        {
//                            const kmlbase::Vec3 vec3 = coordinates->get_coordinates_array_at(j);
//                            boost::shared_ptr<Kml::Point> point(new Kml::Point);
//                            point->SetLat(vec3.get_latitude());
//                            point->SetLon(vec3.get_longitude());
//                            point->SetAlt(vec3.get_altitude());
//                            std::stringstream ss;
//                            ss<<"vec"<<j;
//                            point->SetName(ss.str());
//                            points.push_back(point);
//                        }
//                    }
//                }
//            }

//            PlacemarkWithPath pp2 = _placemarksWithPath[i];
//            kmldom::StylePtr style = kmlengine::CreateResolvedStyle(pp2._placeMarkPtr, fle, kmldom::STYLESTATE_NORMAL);

//            boost::shared_ptr<Kml::Vectors> polygonVec(new Kml::Vectors);

//           // PlacemarkWithPath pp2 = _placemarksWithPath[i];

//            polygonVec->_kmlPathDetails = pp2._kmlPathDetails;


//            polygonVec->SetIsPolygon(true);
//            PlacemarkWithPath pp3 = _placemarksWithPath[i];
//            std::string nm = pp3._placeMarkPtr->get_name();
//            if(nm.compare("") != 0)
//            {
//                std::stringstream ss;
//                ss<<"num points = "<<points.size();
//            }

//            polygonVec->SetLabel(pp3._placeMarkPtr->get_name());
//            polygonVec->SetStreetVectors(points);

//            polygonVec->SetStyle(style);
//            spPolygonVectors->push_back(polygonVec);
//        }
//    }
//    /////////////////////////////////////////////
//    return spPolygonVectors;
//}

//std::string CustomKmlFunctions::GetKmlFromFile(std::string filename)
//{
//    std::ifstream t(filename.c_str());
//    std::string str((std::istreambuf_iterator<char>(t)),
//                 std::istreambuf_iterator<char>());
//    return str;
//}
//boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > CustomKmlFunctions::GetPoints(std::string filename)
//{
//    //todo: have to group kml::point with
//    boost::shared_ptr<std::vector<boost::shared_ptr<Kml::Point> > > points(new std::vector<boost::shared_ptr<Kml::Point> >);


//    //GetKmlFromFile(_kmlFile);
//    //kmlengine::KmlFilePtr fle = kmlengine::KmlFile::CreateFromString(GetKmlFromFile(_kmlFile));
//    kmlengine::KmlFilePtr fle;
//    if(filename.length() == 0)
//        fle = kmlengine::KmlFile::CreateFromString(_kmlFile);
//    else
//    {
//        fle = kmlengine::KmlFile::CreateFromString(GetKmlFromFile(_kmlFile));
//    }


//    for(int i = 0 ; i < _placemarksWithPath.size() ; i++)
//    {
//        boost::shared_ptr<Kml::Point> pt(new Kml::Point);

//        PlacemarkWithPath pp = _placemarksWithPath[i];
//        //boost::shared_ptr<Kml::Point> pt = pp._filterItems->;//(new Kml::Point);
//        //boost::shared_ptr<Kml::Point> pt(new Kml::Point);
//        kmldom::GeometryPtr gptr = pp._placeMarkPtr->get_geometry();
//        if(!gptr)
//            continue;
//        kmldom::PointPtr poKmlPoint = boost::static_pointer_cast<kmldom::Point>(gptr);
//        kmldom::KmlDomType domtype = poKmlPoint->Type();
//        //see this site for further details in how to parse out multigeometry
//        //https://svn.osgeo.org/fdo/trunk/Providers/KingKML/src/Provider/c_KgFdoPlacemark.cpp
//        if(poKmlPoint->has_coordinates() && domtype == kmldom::Type_Point)
//        {
//            //poKmlPoint->
//            kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoint->get_coordinates();
//            kmlbase::Vec3 oKmlVec = poKmlCoordinates->get_coordinates_array_at(0);
//            double lat = 0.00;
//            double lon = 0.00;
//            double altitude = 0.00;
//            //pt->_point->

//            pt->SetLat(oKmlVec.get_latitude());
//            pt->SetLon(oKmlVec.get_longitude());
//            pt->SetAlt(oKmlVec.get_altitude());
//            PlacemarkWithPath pp = _placemarksWithPath[i];
//            pt->SetName(pp._placeMarkPtr->get_name());
//            pt->_kmlPathDetails = pp._kmlPathDetails;
//            kmldom::StylePtr style = kmlengine::CreateResolvedStyle(pp._placeMarkPtr, fle, kmldom::STYLESTATE_NORMAL);
//            pt->SetStyle(style);
//            points->push_back(pt);
//        }

//    }
//    return points;
//}

//void CustomKmlFunctions::TestKmlOutput(std::vector<coordinate> coords)
//{
//    using kmldom::CoordinatesPtr;
//    using kmldom::KmlPtr;
//    using kmldom::KmlFactory;
//    using kmldom::PlacemarkPtr;
//    using kmldom::PointPtr;
//    // Get the factory singleton to create KML elements.
//    KmlFactory* factory = KmlFactory::GetFactory();
//    KmlPtr kml = factory->CreateKml();
//    placemark_vector_t vec;
//    PlacemarkPtr  placemark;
//    std::string statusString;
//    statusString.append("<kml>");
//    for(int i = 0 ; i < coords.size() ; i++)
//    {
//        statusString.append("<Placemark>");
//        statusString.append("<Point>");
//        statusString.append("<coordintates>");
//        double lon = coords[i].longitude;
//        double lat = coords[i].latitude;
//        statusString.append("0");//place altitude here eventually
//        statusString.append("</coordintates>");
//        statusString.append("</Point>");
//        statusString.append("</Placemark>");
//    }
//    statusString.append("</kml>");

//    // Print to stdout
//    std::cout << statusString;
//}

//double CustomKmlFunctions::GetValueDouble(Kml::TYPE type, int index)
//{
//    if(index >= this->_placemarksWithPath.size())
//    {
//        return 0.00;//todo: maybe throw exception instead??
//    }
//    else
//    {
//        PlacemarkWithPath pp = _placemarksWithPath[index];
//        kmldom::GeometryPtr gptr = pp._placeMarkPtr->get_geometry();
//        kmldom::PointPtr poKmlPoint = boost::static_pointer_cast<kmldom::Point>(gptr);
//        kmldom::KmlDomType domtype = poKmlPoint->Type();
//        //see this site for further details in how to parse out multigeometry
//        //https://svn.osgeo.org/fdo/trunk/Providers/KingKML/src/Provider/c_KgFdoPlacemark.cpp
//        if(poKmlPoint->has_coordinates() && (domtype == kmldom::Type_LineString ||domtype == kmldom::Type_Point))
//        {
//            //poKmlPoint->
//            kmldom::CoordinatesPtr poKmlCoordinates = poKmlPoint->get_coordinates();
//            kmlbase::Vec3 oKmlVec = poKmlCoordinates->get_coordinates_array_at(0);
//            double lat = 0.00;
//            double lon = 0.00;
//            double altitude = 0.00;
//            switch(type)
//            {
//            case Kml::LATITUDE:
//                lat = oKmlVec.get_latitude();
//                return lat;
//                break;
//            case Kml::LONGITUDE:
//                lon = oKmlVec.get_longitude();
//                return lon;
//                break;
//            case Kml::ELEVATION:
//                altitude = oKmlVec.get_altitude();
//                return altitude;
//                break;
//            default:
//                return 0.00;
//                break;
//            }
//        }
//        else
//        {
//            return 0.00;
//        }
//    }
//}
