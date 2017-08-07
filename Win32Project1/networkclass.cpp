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

		//// Bounce packets from the client back to it.
		//if (EchoIncomingPackets(sd)) {
		//	// Successfully bounced all connections back to client, so
		//	// close the connection down gracefully.
		//	cout << "Shutting connection down..." << flush;
		//	if (ShutdownConnection(sd)) {
		//		cout << "Connection is down." << endl;
		//	}
		//	else {
		//		cout << endl << WSAGetLastErrorMessage(
		//			"shutdown connection") << endl;
		//		return 3;
		//	}
		//}
		//else {
		//	cout << endl << WSAGetLastErrorMessage(
		//		"echo incoming packets") << endl;
		//	return 3;
		//}
	}

#if defined(_MSC_VER)
	return 0;       // warning eater
#endif
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


//// EchoIncomingPackets ///////////////////////////////////////////////
// Bounces any incoming packets back to the client.  We return false
// on errors, or true if the client closed the socket normally.

bool NetworkClass::EchoIncomingPackets(SOCKET sd)
{
	// Read data from client
	char acReadBuffer[kBufferSize];
	int nReadBytes;

		if (nReadBytes > 0) {
			cout << "Received " << nReadBytes <<
				" bytes from client." << endl;

			int nSentBytes = 0;
			while (nSentBytes < nReadBytes) {
				int nTemp = send(sd, acReadBuffer + nSentBytes,
					nReadBytes - nSentBytes, 0);
				if (nTemp > 0) {
					cout << "Sent " << nTemp <<
						" bytes back to client." << endl;
					nSentBytes += nTemp;
				}
				else if (nTemp == SOCKET_ERROR) {
					return false;
				}
				else {
					// Client closed connection before we could reply to
					// all the data it sent, so bomb out early.
					cout << "Peer unexpectedly dropped connection!" <<
						endl;
					return true;
				}
			}
		}
		else if (nReadBytes == SOCKET_ERROR) {
			return false;
		}


	cout << "Connection closed by peer." << endl;
	return true;
}

std::string NetworkClass::Read()
{
	char acReadBuffer[kBufferSize];
	int nReadBytes = 0;
	int nTotalBytes = 0;

	do {
		nReadBytes = recv(sd, acReadBuffer, kBufferSize, 0);
	} while (nReadBytes == 0);

	if (nReadBytes > 0) {
		cout << "Received " << nReadBytes <<
			" bytes from client." << endl;

		int nSentBytes = 0;
		while (nSentBytes < nReadBytes) {
			int nTemp = send(sd, acReadBuffer + nSentBytes,
				nReadBytes - nSentBytes, 0);
			if (nTemp > 0) {
				cout << "Sent " << nTemp <<
					" bytes back to client." << endl;
				nSentBytes += nTemp;
			}
			else if (nTemp == SOCKET_ERROR) {
				return string();
			}
			else {
				// Client closed connection before we could reply to
				// all the data it sent, so bomb out early.
				cout << "Peer unexpectedly dropped connection!" <<
					endl;
				return string();
			}
		}
	}
	else if (nReadBytes == SOCKET_ERROR) {
		return string();
	}
		

	return string(acReadBuffer, nReadBytes);
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
