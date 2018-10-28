/*
 * Socket.cpp
 *
 *  Created on: November 4, 2017
 *      Author: mike
 */

#include "Socket.hpp"
// ------------------------------------------------------------------
// Create Socket
// Allocate a socket (kernel object) and install pointer in file table.
Socket::Socket() {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd<0) fatalp("Socket: Can't create socket");
}

// ------------------------------------------------------------------
// This is called by the server.   The ::'s are used to call the
// standard unix functions rather than local or OS versions.
void
Socket::listen( int port ) {
	suitcase.sin_family = AF_INET;
	suitcase.sin_port = htons(port);
	suitcase.sin_addr.s_addr = INADDR_ANY;

    // Info in sockaddr is used to set fields of kernel socket.
    int status = ::bind(fd, (sockaddr*)&suitcase, sizeof(sockaddr_in));
    if (status < 0) fatalp("Can't bind socket (%d)", fd);
    refresh();  //
    //cout << "Just bound socket " << fd << ": " << *this << endl;

    // Declare that this is the welcome socket and it listens for clients.
    status = ::listen(fd, 2);    // Limit waiting line to two callers.
    if (status < 0) fatalp("Socket: Unable to listen on socket %d.", fd);
    //cout << "Just called listen(); now waiting for a client to show up\n";
}

// ------------------------------------------------------------------
// This is called by the client.    The :: is used to call the
// standard unix connect() function rather than a local or OS version.
void
Socket::connect( const char* host, int port ) {

	// Use domain name server to get IP address associated with server.
	hostent* remoteHost = gethostbyname( host );
	if (remoteHost == NULL) fatalp("Socket: unknown host: %s\n", host);

    // Copy info about the host into this Socket's sockaddr\_in.
    // The name server returns a list of IP addresses.  We want the first.
    // h_addr_list[0] is the first address found by gethostbyname().
    // h_length is the length of addresses in the h_addr_list[].
	memmove(&suitcase.sin_addr, remoteHost->h_addr_list[0], remoteHost->h_length);
    suitcase.sin_family = AF_INET;

    // Copy the agreed-upon port number into this Socket's sockaddr\_in.
    // Convert first from the local representation to the internet standard.
	suitcase.sin_port = htons(port);

    // Info in client sockaddr structure is now ready to connect to server.
	cout << "Ready to connect socket " << fd << " to " << host << endl;
	int status = ::connect(fd, (sockaddr*)&suitcase, sizeof(sockaddr_in));
	if (status < 0) fatalp("Client: Connection to %s refused.", host);
	refresh();
	cout << "Socket: connection established to " << host << ".\n";
}


// ------------------------------------------------------------------
// Update suitcase with current socket info, including the new port #.
void Socket::refresh() {
    socklen_t addrlen = sizeof(sockaddr_in);
    int status = getsockname( fd, (sockaddr*)&suitcase, &addrlen);
    if ( status < 0 ) fatalp("Socket: getsockname failed on socket %d.", fd);
    cout << *this;
}

// ------------------------------------------------------------------
// Returns information about the peer of this connected socket
int
Socket::peer(Socket* ps) const {
    socklen_t addrlen = sizeof(sockaddr_in);
    sockaddr_in peeraddr;       // Send to getpeername as an output parameter.
    int status = getpeername(fd, (sockaddr*)&peeraddr, &addrlen);
    if (addrlen != sizeof(sockaddr_in))
        fatal("Buffer length error %d!=%d\n", addrlen, sizeof(sockaddr_in));

    *(sockaddr_in*)ps = peeraddr;
    return status;
}
// ------------------------------------------------------------------
ostream&
Socket::print(ostream& out) const {
    out <<  "\t{\n"
    "\tsin_family         = " << suitcase.sin_family << "\n"
    "\tsin_addr.s_addr    = " << inet_ntoa(suitcase.sin_addr) << "\n"
    "\tsin_port           = " << ntohs(suitcase.sin_port) << "\n"
    "\t}\n";
    return out;
}
// ------------------------------------------------------------------
ostream&
Socket::printPeer(ostream& out) const {
    Socket ps;
    int status = peer(&ps);    // Use temp socket to return info on peer.
    if (status==0)  out <<  "Peer socket:\n" << ps << endl;
    else            out << "\tCan't get peer." << strerror(errno) << "\n";
    out << "\t}\n";
    return out;
}
