#ifndef KMLPATHDETAILS
#define KMLPATHDETAILS
#include <string>
#include <vector>

/**
 * basically a copy of placemarkInfo but a non-qt version
 */
class KmlPathDetails{
public:
    std::string _filename;
    std::vector<std::string> _path;
    std::string _element;
    KmlPathDetails()
    {
        _filename = "";
        _element = "";
    }

    bool operator == (KmlPathDetails& rhs)const
    {
        bool emptyFilename = false;
        bool emptyPath = false;
        bool emptyElement = false;

        bool equalFilename = false;
        bool equalPath = true;
        bool equalElement = false;

        if(rhs._filename.compare("") == 0){
            emptyFilename = true;
        }
        if(rhs._element.compare("") == 0){
            emptyElement = true;
        }
        if(rhs._path.size() == 0){
            emptyPath = true;
        }

        if(rhs._filename.compare(this->_filename) == 0){
            equalFilename = true;
        }
        if(rhs._element.compare(this->_element) == 0){
            equalElement = true;
        }
        if(this->_path.size() == rhs._path.size()){
            //check each element
            for(int i = 0 ; i < this->_path.size() ; i++)
            {
                std::string strThisPath = this->_path[i];
                std::string strRHSPath = rhs._path[i];
                if(strThisPath.compare(strRHSPath) != 0)
                {
                    equalPath = false;
                }
            }
        }
        else{
            equalPath = false;
        }

        //now figure out the logic behind if pathdetails are a match for this entity
        if(equalPath && equalElement && equalFilename){
            return true;//it's a complete match for this entity
        }
        if(equalFilename && equalPath && emptyElement){
            return true;//the containing folder is equal, empty element means use anything matching this folder
        }
        if(equalFilename && emptyPath && emptyElement){
            return true;//we're using filename as the filter since path and element are blank, since filenames match they're equal
        }

        return false;

        //        /////////////////////
        //        if(this->_filename.compare(rhs._filename) != 0){
        //            return false;
        //        }
        //        if(this->_element.compare(rhs._element) != 0 && rhs._element.compare("") != 0){
        //            //if element is "" then this could still be a match
        //            //since it could match the path.  ie "" means try to hide everything under path
        //            return false;
        //        }
        //        if(this->_path.size() != rhs._path.size()){
        //            return false;
        //        }
        //        else
        //        {
        //            //check each element
        //            for(int i = 0 ; i < this->_path.size() ; i++)
        //            {
        //                std::string strThisPath = this->_path[i];
        //                std::string strRHSPath = rhs._path[i];
        //                if(strThisPath.compare(strRHSPath) != 0){
        //                    return false;
        //                }
        //            }
        //        }
        //        //if we made it this far all elements are equal
        //        return true;
    }
};
#endif // KMLPATHDETAILS
