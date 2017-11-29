/**
 * Parrot Drones Awesome Video Viewer Library
 * Demuxer interface
 *
 * Copyright (c) 2016 Aurelien Barre
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PDRAW_SOCKET_INET_HPP_
#define _PDRAW_SOCKET_INET_HPP_

#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libpomp.h>
#include <string>

namespace Pdraw {


class InetSocket {
public:
	InetSocket(
		const std::string& localAddress,
		int localPort,
		const std::string& remoteAddress,
		int remotePort,
		struct pomp_loop *loop,
		pomp_fd_event_cb_t fdCb,
		void *userdata);

	~InetSocket(
		void);

	int setRxBufferSize(
		size_t size);

	int setTxBufferSize(
		size_t size);

	int setClass(
		int cls);

	void *getRxBuffer(
		void) {
		return mRxBuffer;
	}

	std::string& getLocalAddress(
		void) {
		return mLocalAddressStr;
	}

	int getLocalPort(
		void) {
		return mLocalPort;
	}

	std::string& getRemoteAddress(
		void) {
		return mRemoteAddressStr;
	}

	int getRemotePort(
		void) {
		return mRemotePort;
	}

	size_t getRxBufferSize(
		void) {
		return mRxBufferSize;
	}

	ssize_t read(
		void);

	ssize_t write(
		const void *buf,
		size_t len);

private:
	std::string mLocalAddressStr;
	int mLocalPort;
	std::string mRemoteAddressStr;
	int mRemotePort;
	struct pomp_loop *mLoop;
	int mFd;
	pomp_fd_event_cb_t mFdCb;
	void *mUserdata;
	struct sockaddr_in mLocalAddress;
	struct sockaddr_in mRemoteAddress;
	void *mRxBuffer;
	size_t mRxBufferSize;
};

} /* namespace Pdraw */

#endif /* !_PDRAW_SOCKET_INET_HPP_ */