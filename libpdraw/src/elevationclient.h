#ifndef ELEVATIONCLIENT_H
#define ELEVATIONCLIENT_H

#include </usr/local/include/boost/asio.hpp>
#include </usr/local/include/boost/bind.hpp>
#include </usr/local/include/boost/thread.hpp>
#include </usr/local/include/boost/filesystem/fstream.hpp>
#include </usr/local/include/boost/filesystem/operations.hpp>
#include </usr/local/include/boost/shared_array.hpp>
#include </usr/local/include/boost/thread/mutex.hpp>
#include <DemTileCollection.h>
#include <WMSQuery.h>
using boost::asio::ip::tcp;

class ElevationClient: public LoggableConfigurableObject{
public:
    virtual void SetUserSettingsReference(boost::shared_ptr<UserSettingsData> usettingsData);
    bool _invalidElevTile;
    ElevationClient( WebMapURLGenerator * q, std::string tempPath):_requestInProgress(false),
        resolver_(_io_service),
        headerSize_(0),
        socket_(_io_service),
        _io_running(false),
        _tempPath("C:\\SourceCode\\"),
        _invalidElevTile(false)

    {
        _tempPath = tempPath;
        _q = q;
    };
    std::string _tempPath;
    std::string _urlPath;
     WebMapURLGenerator * _q;

boost::asio::io_service _io_service;
bool _io_running;// = false;
    void client_request(boost::shared_ptr<DemTileCollection> demTilesCollection, void (*completion)(void));

    void handle_resolve(const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator);

    void handle_connect(const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator);

    void handle_write_request(const boost::system::error_code& err);

    void handle_read_status_line(const boost::system::error_code& err);

    void handle_read_headers(const boost::system::error_code& err);

    void handle_read_content(const boost::system::error_code& err);

    bool _requestInProgress;

    tcp::resolver resolver_;
    tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;
    //These are related to the DEM tiles
    boost::shared_ptr<DemTileCollection> _demTileCollection;

    boost::shared_array<char> header_;
    int headerSize_;

private:
};

#endif // ELEVATIONCLIENT_H

