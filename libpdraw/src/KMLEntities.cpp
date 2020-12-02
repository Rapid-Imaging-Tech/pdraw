#include "KMLEntities.h"
#include "utilities.h"
#include "ErrorCodes.h"
#if defined(__APPLE__)
#include "boost/format.hpp"
#include "boost/lexical_cast.hpp"
#include "objcUtilityWrapper.h"
#else
#include "/usr/local/include/boost/format.hpp"
#include "/usr/local/include/boost/lexical_cast.hpp"

#endif
//#include "geodatagrid.h"
#include <strstream>


#include "kml/dom/folder.h"
#if !defined(__APPLE__)
//using Kml::CustomKmlFunctions;

#endif
using namespace kmldom;
// Return a FeaturePtr to the root Feature in the kmlfile.  If the kmlfile
// does not parse or has no root Feature then an empty FeaturePtr is returned.
FeaturePtr GetKmlFileRootFeature(const char* kmlfile, int uid, std::vector<std::string>& newIcons)
{
   // Type_Folder f;
    KmlDomType t = Type_Folder;
  // Read the file. f
  std::string file_data;
  if (!kmlbase::File::ReadFileToString(kmlfile, &file_data)) {
    cout << kmlfile << " read failed" << endl;

    return NULL;
  }

  // If the file was KMZ, extract the KML file.
  std::string kml;
  if (KmzFile::IsKmz(file_data)) {
    KmzFilePtr kmz_file = KmzFile::OpenFromString(file_data);
    std::vector<std::string> files;
    kmz_file->List(&files);

    std::string contents;
#if defined(__APPLE__)
        std::string iconFolder = GetDataFolderPath();//data folder is where ios app stores files
#else
        std::string iconFolder = AppDataDir();
#endif
    for(int i = 0 ; i < files.size() ; i++)
    {
        const char* file = files[i].c_str();
        std::string strFile(file);
        std::string fullPath(iconFolder);
#if defined(__APPLE__)
            fullPath.append("/");
#else
            
            fullPath.append("/icons/");
#endif
        std::stringstream ss;
        ss<<"custom-";
        ss<<uid;
        ss<<"-";
        std::replace( strFile.begin(), strFile.end(), '/', '-');
        ss<<strFile;
        fullPath.append(ss.str());

        if(DoesStringContainSubstring(strFile, ".png")||DoesStringContainSubstring(strFile, ".PNG"))
        {
            newIcons.push_back(ss.str());
        }

        kmz_file->ReadFile(files[i].c_str(), &contents);
        std::ofstream outfile (fullPath.c_str(), std::ofstream::binary);
        outfile.write (contents.c_str(), contents.size());
    }

    if (!kmz_file) {
      cout << "Failed opening KMZ file" << endl;
      return NULL;
    }
    if (!kmz_file->ReadKml(&kml)) {
      cout << "Failed to read KML from KMZ" << endl;
      return NULL;
    }
  } else {
    kml = file_data;
  }

  // Parse it.
  std::string errors;
  KmlFilePtr kml_file = KmlFile::CreateFromParse(kml, &errors);
  if (!kml_file) {
    cout << errors << endl;
    return FeaturePtr();
  }

  // Get the root
  return kmlengine::GetRootFeature(kml_file->get_root());
}
void CreateDirectoryTree(std::string path){

}
//typedef std::vector<PlacemarkPtr> placemark_vector_t;
void SavePlacemarks(const FeaturePtr& feature,
                           //placemark_vector_t* placemarks, std::string filename)
                    std::vector<PlacemarkWithPath>& placemarksWithPath, std::string filename, std::vector<std::string> path)
{
    if (LineStringPtr linestring = kmldom::AsLineString(feature))
    {
        std::cout<<"found linestring";
    }
    if (PlacemarkPtr placemark = kmldom::AsPlacemark(feature))
    {

        std::string pmName = placemark->get_name();
        PlacemarkWithPath pmarkWPath;
        pmarkWPath._placeMarkPtr = placemark;

        pmarkWPath._kmlPathDetails->_element = pmName;
        pmarkWPath._kmlPathDetails->_filename = filename;
        pmarkWPath._kmlPathDetails->_path = path;
        //pmarkWPath._path.push_back("todo:path2");
        placemarksWithPath.push_back(pmarkWPath);
    }
    else if (const ContainerPtr container = kmldom::AsContainer(feature))
    {
        //if(feature->Type() == Type_Folder)
        //                    {
        //                        const kmldom::FolderPtr fldr = AsFolder(featPtr);
        //                        //std::string dgddgd = fldr->get_id();
        //                        std::string fldrName = fldr->get_name();
        //                        std::cout<<"folder name ="<<fldrName<<std::endl;
        //                        //std::cout<<"found a folder";
        //                    }


        //todo: get folder name and add to end of path and send it recursiveley to saveplacemarks
        if(feature->Type() == Type_Folder)
        {
            const kmldom::FolderPtr fldr = AsFolder(feature);
            const std::string strFldr = fldr->get_name();
            std::string currentPathElement("");
            currentPathElement.append(fldr->get_name());
            //const string currentPathElement = fldr->get_name();
            //std::stringstream ss;
            //std::string strNewPathElementW("/");
           // strNewPathElement.append(newPathElement);
            //ss<<path;
            //ss<<"/";
            //ss<<newPathElement;
            //std::string addPath("pathnew");
            path.push_back(currentPathElement);
            //path.append(currentPathElement);

        }
        for (size_t i = 0; i < container->get_feature_array_size(); ++i)
        {
            SavePlacemarks(container->get_feature_array_at(i), placemarksWithPath, filename, path);
        }
    }
}
