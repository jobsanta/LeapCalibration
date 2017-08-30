#include "ws-util.h"

//#include <winsock2.h>

#include <iostream>
#include "message.pb.h"

////////////////////////////////////////////////////////////////////////
// Constants

const int kBufferSize = 20480;
const int kDefaultServerPort = 4242;

class NetworkClass
{
public:

	int EstrablishConnection();
	int ClientConnet(const char* pcHost, int nPort);

	std::string Read();
	bool Reply(std::string st, int length);

	bool SendFrame(FrameMsg msg);
	std::string ReadReply();
	bool Shutdown();



private:

	const int prefix_size = 2;
	char g_holding_buffer[20480];
	int g_held_bytes = 0;

	SOCKET sd;
	SOCKET SocketConnection(u_long nRemoteAddr, u_short nPort);
	u_long LookupAddress(const char * pcHost);
	SOCKET SetUpListener(const char* pcAddress, int nPort);
	SOCKET AcceptConnection(SOCKET ListeningSocket, sockaddr_in& sinRemote);

	//bool EchoIncomingPackets(SOCKET sd);

	int recv_packet(SOCKET sd, char* p_out_buffer, int buffer_size);




};