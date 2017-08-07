/***********************************************************************
 ws-util.h - Declarations for the Winsock utility functions module.
***********************************************************************/

#if !defined(WS_UTIL_H)
#define WS_UTIL_H

// Uncomment one.
#include <winsock2.h>
#pragma comment (lib, "Ws2_32")

extern const char* WSAGetLastErrorMessage(const char* pcMessagePrefix,
        int nErrorID = 0);
extern bool ShutdownConnection(SOCKET sd);

#endif // !defined (WS_UTIL_H)

