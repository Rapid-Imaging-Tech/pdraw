#ifndef GEOTILESERVERGL_H
#define GEOTILESERVERGL_H
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <fstream>
#if defined(__APPLE__)
#include "objcUtilityWrapper.h"
#endif
#include "LoggableObject.h"
using boost::asio::ip::tcp;
using namespace std;
class geotileservergl : public LoggableConfigurableObject
{
private:
    
public:
    virtual void SetUserSettings();
#if defined(__APPLE__)
    LFObjCFunctionsCPPWrapper ocWrapper;
#else
        //virtual void SetUserSettings();
#endif
    std::string _errorMsg;
    bool _error;
    std::string _tempPath;
    bool _doneGrabbingData;

    /////////////////////////////////////
   // void queryTileServer(const std::string& server, const std::string& path);

      geotileservergl(boost::asio::io_service& io_service,
           std::string server, std::string portNum,  std::string path,  std::string filename,
                      boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger);

    private:
      void handle_write_request(const boost::system::error_code& err);
      void handle_resolve(const boost::system::error_code& err,
          tcp::resolver::iterator endpoint_iterator);

      void handle_read_status_line(const boost::system::error_code& err);

      void handle_read_headers(const boost::system::error_code& err);

      void handle_read_content(const boost::system::error_code& err);

      void handle_connect(const boost::system::error_code& err,
          tcp::resolver::iterator endpoint_iterator);



    /////////////////////////
    //geotileservergl();
      bool fileOpen_;

      //boost::asio::io_service io_service_;
      //void makerequest(){
        //  client c2(io_service_, "192.168.0.39", "/data/v3/13/1999/2887.pbf");
          //io_service_.run();


      std::string _saveFilename;
      ofstream outbin_;
      tcp::resolver resolver_;
      tcp::socket socket_;
      boost::asio::streambuf request_;
      boost::asio::streambuf response_;
};

#endif // GEOTILESERVERGL_H
