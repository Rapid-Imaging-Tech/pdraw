#include "geotileservergl.h"
#include <iostream>
#include <boost/filesystem.hpp>

using namespace std;

void geotileservergl::SetUserSettings()
{
    //todo: fill in with any settings you want to grab from settings file
}

geotileservergl::geotileservergl(boost::asio::io_service& io_service,
                                  std::string server, std::string portNum,  std::string path,
                                  std::string filename, boost::shared_ptr<UserSettingsData> usettingsData, boost::shared_ptr<StatusLogger> logger)

      : resolver_(io_service),
        socket_(io_service),
        _saveFilename(filename),
        _doneGrabbingData(false),
        _error(false)
{
    {
        LoggableConfigurableObject::Init(usettingsData, logger, "GeoTileserver");
      // Form the request. We specify the "Connection: close" header so that the
      // server will close the socket after transmitting the response. This will
      // allow us to treat all data up until the EOF as the content.

        std::ostream request_stream(&request_);
      request_stream << "GET " << path << " HTTP/1.0\r\n";
      request_stream << "Host: " << server << "\r\n";
      request_stream << "Accept: */*\r\n";
      request_stream << "Connection: close\r\n\r\n";

      // Start an asynchronous resolve to translate the server and service names
      // into a list of endpoints.
      tcp::resolver::query query(server, portNum);
      resolver_.async_resolve(query,
          boost::bind(&geotileservergl::handle_resolve, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::iterator));
    }
}


void geotileservergl::handle_resolve(const boost::system::error_code& err,
    tcp::resolver::iterator endpoint_iterator)
{
    //WriteToStatusLog("handle_resolve", true, true);
   // cout<<"handleresolve"<<endl;
  if (!err)
  {
    // Attempt a connection to the first endpoint in the list. Each endpoint
    // will be tried until we successfully establish a connection.
    tcp::endpoint endpoint = *endpoint_iterator;
    socket_.async_connect(endpoint,
        boost::bind(&geotileservergl::handle_connect, this,
          boost::asio::placeholders::error, ++endpoint_iterator));
  }
  else
  {
      _error = true;
      _errorMsg="Could not locate host";
      _doneGrabbingData = true;
    std::cout << "Error: " << err.message() << "\n";
  }
}

void geotileservergl::handle_connect(const boost::system::error_code& err,
    tcp::resolver::iterator endpoint_iterator)
{
  if (!err)
  {
    // The connection was successful. Send the request.
    boost::asio::async_write(socket_, request_,
        boost::bind(&geotileservergl::handle_write_request, this,
          boost::asio::placeholders::error));
  }
  else if (endpoint_iterator != tcp::resolver::iterator())
  {
    // The connection failed. Try the next endpoint in the list.
    socket_.close();
    tcp::endpoint endpoint = *endpoint_iterator;
    socket_.async_connect(endpoint,
        boost::bind(&geotileservergl::handle_connect, this,
          boost::asio::placeholders::error, ++endpoint_iterator));
  }
  else
  {
      _error = true;
      _doneGrabbingData = true;
    std::cout << "Error: " << err.message() << "\n";
  }
}

void geotileservergl::handle_write_request(const boost::system::error_code& err)
{
  if (!err)
  {
    // Read the response status line.
    boost::asio::async_read_until(socket_, response_, "\r\n",
        boost::bind(&geotileservergl::handle_read_status_line, this,
          boost::asio::placeholders::error));
  }
  else
  {
      _error = true;
      _doneGrabbingData = true;
    std::cout << "Error: " << err.message() << "\n";
  }
}

void geotileservergl::handle_read_status_line(const boost::system::error_code& err)
{
  if (!err)
  {
    // Check that response is OK.
    std::istream response_stream(&response_);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
        _error = true;
        _errorMsg = "Invalid response";
      std::cout << "Invalid response\n";
      _doneGrabbingData = true;
      return;
    }
    if (status_code != 200)
    {

        _error = true;
        std::stringstream ss;
        ss<<"Response returned with status code "<<status_code;
        _errorMsg = ss.str();
      std::cout << "Response returned with status code ";
      std::cout << status_code << "\n";
    _doneGrabbingData = true;
      return;
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
        boost::bind(&geotileservergl::handle_read_headers, this,
          boost::asio::placeholders::error));
  }
  else
  {
      _error = true;
    std::cout << "Error: " << err << "\n";
  }
}
extern std::string AppDataDir();
void geotileservergl::handle_read_headers(const boost::system::error_code& err)
{
  if (!err)
  {
#if !defined(__APPLE__)
       outbin_.open(_saveFilename, ios::out|ios::binary);
#endif
    // Process the response headers.
    std::istream response_stream(&response_);
    std::string header;
    while (std::getline(response_stream, header) && header != "\r")
    {
    }

    // Write whatever content we already have to output.
    if (response_.size() > 0)
    {
    }

    // Start reading remaining data until EOF.
    boost::asio::async_read(socket_, response_,
        boost::asio::transfer_at_least(1),
        boost::bind(&geotileservergl::handle_read_content, this,
          boost::asio::placeholders::error));
  }
  else
  {
      _error = true;
      _doneGrabbingData = true;
    std::cout << "Error: " << err << "\n";
  }
}

void geotileservergl::handle_read_content(const boost::system::error_code& err)
{
  if (!err)
  {
    // Write all of the data that has been read so far.
#if !defined(__APPLE__)
    outbin_  << &response_;
#endif
    // Continue reading remaining data until EOF.
    boost::asio::async_read(socket_, response_,
        boost::asio::transfer_at_least(1),
        boost::bind(&geotileservergl::handle_read_content, this,
          boost::asio::placeholders::error));
  }
  else if (err != boost::asio::error::eof)
  {
    std::cout << "Error: " << err << "\n";
  }
  else if(err == boost::asio::error::eof)
  {
#if defined(__APPLE__)
 unsigned char* output = (unsigned char*)malloc(response_.size());
        memcpy(output, boost::asio::buffer_cast<const void*>(response_.data()), response_.size());
        ocWrapper.createPngFile(_saveFilename, output, response_.size());//give data ptr and size and do it in one shot
        delete output;
        _doneGrabbingData = true;
#else
      outbin_.close();
      _doneGrabbingData = true;
#endif
  }
}
