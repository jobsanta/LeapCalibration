#include "ws-util.h"

//#include <winsock2.h>

#include <iostream>


////////////////////////////////////////////////////////////////////////
// Constants

const int kBufferSize = 20480;
const int kDefaultServerPort = 4242;

class NetworkClass
{
public:
	int EstrablishConnection();
	std::string Read();
	bool Shutdown();


private:

	SOCKET sd;
	SOCKET SetUpListener(const char* pcAddress, int nPort);
	SOCKET AcceptConnection(SOCKET ListeningSocket, sockaddr_in& sinRemote);

	bool EchoIncomingPackets(SOCKET sd);




};