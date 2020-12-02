#define ATTR_SET ".<xmlattr>"
#define XML_PATH1 "./DavesPlacesdf.kml"
#include "KMLParser.h"
#include <iostream>
#include <string>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <string>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::property_tree;

const ptree& empty_ptree(){
    static ptree t;
    return t;
}

PlaceMark::PlaceMark()
	:
	_latitude(0.00),
	_longitude(0.00)
{

}
std::vector<PlaceMark> KMLParser::ParseKML(const std::string& filename)
{
	std::vector<PlaceMark> placeMarks;
	placeMarks.clear();
    ptree tree;
	try
	{
		double x = 0;
		double y = 1;
		double result = x / y;
		read_xml(filename, tree);
		const ptree & formats = tree.get_child("kml", empty_ptree());
		double lat = 0.00;
		double lon = 0.00;
        BOOST_FOREACH(const ptree::value_type & f, formats)
		{
			if(f.first == "Document")
			{
                boost::property_tree::ptree subtree = f.second;
                BOOST_FOREACH(const ptree::value_type & v, subtree.get_child(""))
				{
					if(v.first == "Placemark")
					{
                        boost::property_tree::ptree subtreeb = v.second;
                        BOOST_FOREACH(const ptree::value_type & q, subtreeb.get_child(""))
						{
							if(q.first == "Point")
							{
								PlaceMark pm;
                                boost::property_tree::ptree subtreed = q.second;
								std::string str = subtreed.get<std::string>("coordinates");
								std::string parsed;
								stringstream input_stringstream(str);
								if(getline(input_stringstream, parsed, ','))
								{
									lon = atof(parsed.c_str());
									pm.SetLongitude(lon);
									std::cout<<"lon="<<lon<<endl;
								}
								if(getline(input_stringstream, parsed, ','))
								{
									lat = atof(parsed.c_str());
									pm.SetLatitude(lat);
									std::cout<<"lat="<<lat<<endl;
								}
								placeMarks.push_back(pm);
							}
						}
					}
					if(v.first == "Folder")
					{
                        boost::property_tree::ptree subtreeb = v.second;
                        BOOST_FOREACH(const ptree::value_type & q, subtreeb.get_child(""))
						{
							if(q.first == "Placemark")
							{
                                boost::property_tree::ptree subtreec = q.second;
                                BOOST_FOREACH(const ptree::value_type & r, subtreec.get_child(""))
								{
									if(r.first == "Point")
									{
										PlaceMark pm;
                                        boost::property_tree::ptree subtreed = r.second;
										std::string str = subtreed.get<std::string>("coordinates");
										std::string parsed;
										stringstream input_stringstream(str);

										if(getline(input_stringstream, parsed, ','))
										{
											lon = atof(parsed.c_str());
											std::cout<<"lon="<<lon<<endl;
											pm.SetLongitude(lon);

										}
										if(getline(input_stringstream, parsed, ','))
										{
											

											lat = atof(parsed.c_str());
											std::cout<<"lat="<<lat<<endl;
											pm.SetLatitude(lat);
										}
										placeMarks.push_back(pm);
									}
								}
							}
						}
					}
				}
			}
		}
		
	}
	catch(xml_parser_error error)
	{
		std::cout<<error.what();
	}
	catch(...)
	{
		std::cout<<"unknown error";
	}
	return placeMarks;
}
