#ifndef PLACEMARKINFO
#define PLACEMARKINFO
//#include <qstring.h>
//#include <qstringlist.h>
#include <string>
#include <vector>
class PlacemarkInfo{
public:
#if defined(__APPLE__)

    std::string _filename;
    std::vector<std::string> _path;
    std::string _element;
#else
    QString _filename;
    QStringList _path;
    QString _element;
#endif

};
#endif // PLACEMARKINFO
