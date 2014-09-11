// libpklts_ctrl.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "st_ctrlmsg.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <time.h>
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512

int __cdecl RemoteFunctionCall(
	const char * address, 
	unsigned __int16 port,
	const unsigned __int8 * sendbuf,
	unsigned __int16 len,
	std::vector<unsigned __int8> & vec_result
	) 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
	char str_port[32];
	sprintf_s(str_port,"%d",(int)port);
    // Resolve the server address and port
    iResult = getaddrinfo(address, str_port, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
	int totalSent= 0;
    // Send an initial buffer
	do
	{
		iResult = send( ConnectSocket,(const char *) sendbuf + totalSent, (int)len -totalSent , 0 );
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		totalSent += iResult;
	}while (totalSent < len );

    printf("Bytes Sent: %ld\n", iResult);

	int totalRecieved = 0;
	int totalWant = 0x7fffffff;
    // Receive until the peer closes the connection
    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
		{
			for (int i=0;i<iResult;++i) vec_result.push_back(recvbuf[i]);
			totalRecieved += iResult;
			if (totalRecieved >= sizeof(ParkinglotsSvr::PKLTS_Trans_Header))
			{
				ParkinglotsSvr::PKLTS_Trans_Header * pHeader = (ParkinglotsSvr::PKLTS_Trans_Header *)vec_result.data();
				if (pHeader->Mark==0x55AA)
					totalWant = pHeader->DataLen + sizeof(ParkinglotsSvr::PKLTS_Trans_Header);
				else
				{
					//Send End Message
					char bufferEnd[] = {0,0,0,0};
					if (send( ConnectSocket,(const char *) bufferEnd, (int)sizeof(bufferEnd) , 0 ) == SOCKET_ERROR) {
						printf("send failed with error: %d\n", WSAGetLastError());
						closesocket(ConnectSocket);
						WSACleanup();
						return 1;
					}
				}
			}
			if (totalRecieved >=  totalWant)
			{
				//Send End Message
				char bufferEnd[] = {0,0,0,0};
				if (send( ConnectSocket,(const char *) bufferEnd, (int)sizeof(bufferEnd) , 0 ) == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}
				totalWant = 0x7fffffff;
			}
		}
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( iResult > 0 );

	// shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

unsigned __int32 getUniqueSrcID()
{
	unsigned __int32 dwProcessID = ::GetCurrentProcessId();
	unsigned __int32 dwThreadID = ::GetCurrentThreadId();
	unsigned __int32 dwALLid = 0;

	do{
		unsigned __int16 seed = rand();
		dwALLid = (unsigned __int32(seed & 0x7f00) << 16) + (unsigned __int32(seed & 0x00ff) << 8);
		dwALLid ^=((dwProcessID <<16) + dwThreadID);
		dwALLid |= 0X80000000;
	}while (dwALLid >=0xFFFFFFFC);
	return dwALLid;
}	