/***********************************************************************
basic-server.cpp - Implements a fairly basic single-threaded Winsock
server program that waits for a connection, accepts it, echoes back
any data it receives, and then goes back to listening when the
client drops the connection.

Compiling:
VC++: cl -GX basic-server.cpp main.cpp ws-util.cpp wsock32.lib
BC++: bcc32 basic-server.cpp main.cpp ws-util.cpp

This program is hereby released into the public domain.  There is
ABSOLUTELY NO WARRANTY WHATSOEVER for this product.  Caveat hacker.
***********************************************************************/

#include "networkclass.h"

//#pragma comment (lib, "Ws2_32.lib")
using namespace std;


//// DoWinsock /////////////////////////////////////////////////////////
// The module's driver function -- we just call other functions and
// interpret their results.

int NetworkClass::EstrablishConnection()
{

	// Start Winsock up
	WSAData wsaData;
	int nCode;
	if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
		cerr << "WSAStartup() returned error code " << nCode << "." <<
			endl;
		return 255;
	}

	// Begin listening for connections
	cout << "Establishing the listener..." << endl;
	SOCKET ListeningSocket = SetUpListener("0.0.0.0", htons(kDefaultServerPort));
	if (ListeningSocket == INVALID_SOCKET) {
		cout << endl << WSAGetLastErrorMessage("establish listener") <<
			endl;
		return 3;
	}
	sd = INVALID_SOCKET;
	// Spin forever handling clients
	while (sd == INVALID_SOCKET) {
		// Wait for a connection, and accepting it when one arrives.
		cout << "Waiting for a connection..." << flush;
		sockaddr_in sinRemote;
		sd = AcceptConnection(ListeningSocket, sinRemote);
		if (sd != INVALID_SOCKET) {
			cout << "Accepted connection from " <<
				inet_ntoa(sinRemote.sin_addr) << ":" <<
				ntohs(sinRemote.sin_port) << "." << endl;
		}

	}

#if defined(_MSC_VER)
	return 0;       // warning eater
#endif
}

int NetworkClass::ClientConnet(const char* pcHost, int nPort)
{

	// Start Winsock up
	WSAData wsaData;
	int nCode;
	if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
		cerr << "WSAStartup() returned error code " << nCode << "." <<
			endl;
		return 255;
	}

	cout << "Looking up address..." << flush;
	u_long nRemoteAddress = LookupAddress(pcHost);
	if (nRemoteAddress == INADDR_NONE) {
		cerr << endl << WSAGetLastErrorMessage("lookup address") <<
			endl;
		return 3;
	}
	in_addr Address;
	memcpy(&Address, &nRemoteAddress, sizeof(u_long));
	cout << inet_ntoa(Address) << ":" << nPort << endl;

	// Connect to the server
	cout << "Connecting to remote host..." << flush;
	sd = SocketConnection(nRemoteAddress, htons(nPort));
	if (sd == INVALID_SOCKET) {
		const char* test = WSAGetLastErrorMessage("connect to server");
		return 3;
	}
	cout << "connected, socket " << sd << "." << endl;

}

SOCKET NetworkClass::SocketConnection(u_long nRemoteAddr, u_short nPort)
{
	// Create a stream socket
	SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd != INVALID_SOCKET) {
		sockaddr_in sinRemote;
		sinRemote.sin_family = AF_INET;
		sinRemote.sin_addr.s_addr = nRemoteAddr;
		sinRemote.sin_port = nPort;
		if (connect(sd, (sockaddr*)&sinRemote, sizeof(sockaddr_in)) ==
			SOCKET_ERROR) {
			sd = INVALID_SOCKET;
		}
	}

	return sd;
}

u_long NetworkClass::LookupAddress(const char* pcHost)
{
	u_long nRemoteAddr = inet_addr(pcHost);
	if (nRemoteAddr == INADDR_NONE) {
		// pcHost isn't a dotted IP, so resolve it through DNS
		hostent* pHE = gethostbyname(pcHost);
		if (pHE == 0) {
			return INADDR_NONE;
		}
		nRemoteAddr = *((u_long*)pHE->h_addr_list[0]);
	}

	return nRemoteAddr;
}

//// SetUpListener /////////////////////////////////////////////////////
// Sets up a listener on the given interface and port, returning the
// listening socket if successful; if not, returns INVALID_SOCKET.

SOCKET NetworkClass::SetUpListener(const char* pcAddress, int nPort)
{
	u_long nInterfaceAddr = inet_addr(pcAddress);
	if (nInterfaceAddr != INADDR_NONE) {
		SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);
		if (sd != INVALID_SOCKET) {
			sockaddr_in sinInterface;
			sinInterface.sin_family = AF_INET;
			sinInterface.sin_addr.s_addr = nInterfaceAddr;
			sinInterface.sin_port = nPort;
			if (bind(sd, (sockaddr*)&sinInterface,
				sizeof(sockaddr_in)) != SOCKET_ERROR) {
				listen(sd, 1);
				return sd;
			}
		}
	}

	return INVALID_SOCKET;
}


//// AcceptConnection //////////////////////////////////////////////////
// Waits for a connection on the given socket.  When one comes in, we
// return a socket for it.  If an error occurs, we return 
// INVALID_SOCKET.

SOCKET NetworkClass::AcceptConnection(SOCKET ListeningSocket, sockaddr_in& sinRemote)
{
	int nAddrSize = sizeof(sinRemote);
	return accept(ListeningSocket, (sockaddr*)&sinRemote, &nAddrSize);
}


std::string NetworkClass::Read()
{
	char acReadBuffer[20480];

	u_short nLength;

	int packet_size = -1;

	
	packet_size = recv_packet(sd, acReadBuffer, 20480);
	if (packet_size > 0)
	{
		string str(acReadBuffer + 2, packet_size - 2);
		return str;
	}
	else
	{
		return string();
	}



}

bool NetworkClass::Reply(string st, int length)
{
	u_short nLength = htons(length + 2);
	string header((char*)&nLength, 2);
	string combine(header + st);

	if (send(sd, combine.c_str(), combine.size(), 0) == SOCKET_ERROR) {

		return false;
	}

}

bool NetworkClass::SendFrame(FrameMsg framemsg)
{
	std::string st;
	framemsg.SerializeToString(&st);

	u_short hostshort = framemsg.ByteSize() + 2;
	u_short nlength = htons(hostshort);
	string header((char*)&nlength, 2);
	string combine(header + st);

	if (send(sd, combine.c_str(), combine.size(), 0) == SOCKET_ERROR) {

		return false;
	}
}

std::string NetworkClass::ReadReply()
{
	char frameBuffer[20480];
	int packet_size = -1;
	packet_size = recv_packet(sd, frameBuffer, 20480);
	return string (frameBuffer, packet_size);
}

bool NetworkClass::Shutdown()
{
	if (ShutdownConnection(sd)) {
		cout << "Connection is down." << endl;
	}
	else {
		cout << endl << WSAGetLastErrorMessage(
			"shutdown connection") << endl;
		return 3;
	}
	// Shut Winsock back down and take off.
	WSACleanup();
}



int NetworkClass::recv_packet(SOCKET sd, char* p_out_buffer, int buffer_size)
{
	int bytes_read = 0;
	int packet_size;
	bool building_prefix = true;

	assert(buffer_size == sizeof(g_holding_buffer));

	// Copy any data remaining from previous call into output buffer.
	if (g_held_bytes > 0) {
		assert(buffer_size >= g_held_bytes);
		memcpy(p_out_buffer, g_holding_buffer, g_held_bytes);
		bytes_read += g_held_bytes;
		g_held_bytes = 0;
	}

	// Read the packet
	while (1) {
		if (building_prefix) {
			if (bytes_read >= prefix_size) {
				packet_size = 0;
				for (int i = 0; i < prefix_size; ++i) {
					packet_size <<= 8;
					packet_size |= p_out_buffer[i] & 0xff;
				}
				building_prefix = false;
				if (packet_size > buffer_size) {
					// Buffer's too small to hold the packet!
					// Do error handling here.
					return 0;
				}
			}
		}

		if (!building_prefix && (bytes_read >= packet_size)) {
			break;  // finished building packet
		}

		int new_bytes_read = recv(sd, p_out_buffer + bytes_read,
			buffer_size - bytes_read, 0);
		if ((new_bytes_read == 0) || (new_bytes_read == SOCKET_ERROR)) {
			return 0;       // do proper error handling here!
		}
		bytes_read += new_bytes_read;
	}

	// If anything is left in the read buffer, keep a copy of it
	// for the next call.
	g_held_bytes = bytes_read - packet_size;
	memcpy(g_holding_buffer, p_out_buffer + packet_size, g_held_bytes);

	return packet_size;
}