//
// async_client.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include <fstream>
using boost::asio::ip::tcp;
using namespace std;

class PngNetworkFetcher
{
	public:
		~PngNetworkFetcher();
		PngNetworkFetcher(const std::string& server, const std::string& path, std::string pngFileName);
		static bool _fetchingPng ;
		
	private:
		boost::asio::io_service io_service;
		tcp::resolver resolver_;
		tcp::socket socket_;
		boost::asio::streambuf request_;
		boost::asio::streambuf response_;
		void handle_resolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);
		void handle_connect(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);
		void handle_write_request(const boost::system::error_code& err);
		void handle_read_status_line(const boost::system::error_code& err);
		void handle_read_headers(const boost::system::error_code& err);
		void handle_read_content(const boost::system::error_code& err);
};


//int main(int argc, char* argv[])
//{
//  try
//  {
//    if (argc != 3)
//    {
//      std::cout << "Usage: async_client <server> <path>\n";
//      std::cout << "Example:\n";
//      std::cout << "  async_client www.boost.org /LICENSE_1_0.txt\n";
//      return 1;
//    }
//
//    boost::asio::io_service io_service;
//    client c(io_service, argv[1], argv[2]);
//    io_service.run();
//  }
//  catch (std::exception& e)
//  {
//    std::cout << "Exception: " << e.what() << "\n";
//  }
//
//  return 0;
//}