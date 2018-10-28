/*
 * Socket.hpp
 *
 *  Created on: November 4, 2017
 *      Author: mike
 */

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <unistd.h>
#include <netdb.h>
#include <sys/poll.h>
#include <arpa/inet.h>

#include "tools.hpp"

// Object of specified family, type, and protocol
class Socket {
private:
	int fd = -1;	// File descriptor for this socket
	sockaddr_in suitcase = {0};

public:
	Socket();
	void listen(int port);
	void connect(const char* host, int port);

	int sockfd() const   { return fd; }
	in_port_t   port()   { return suitcase.sin_port; }
	in_addr     addr()   { return suitcase.sin_addr; }

	void refresh();
	int peer(Socket* ps) const;

	ostream& print(ostream& out) const;
	ostream& printPeer(ostream& out) const;
};

inline ostream& operator <<(ostream& out, const Socket& sn) {
	return sn.print(out); }

#endif /* SOCKET_HPP_ */
