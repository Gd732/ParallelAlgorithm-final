//TCP server
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <iostream>
#include "server.h"
using namespace std;

void addr_init(SOCKADDR_IN &addr)
{
	addr.sin_family = AF_INET; //IPv4 Socket
	addr.sin_port = htons(SOCKET_PORT); // sever Port
	addr.sin_addr.s_addr = inet_addr(SOCKET_IPADDR); //server PC
}

SOCKET server_init()
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "WinSock startup error", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	addr_init(addr);

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);//接待处 
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));

	listen(sListen, SOMAXCONN);

	SOCKET newConnection; //业务员  build a new socket do new connection.the sListen is just listenning not used to exchange data
	newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //newConnection is used to exchange data with client

	return newConnection;
}

bool checkSent(int bytesSent, SOCKET& Connection)
{
	if (bytesSent == SOCKET_ERROR)
	{
		cerr << "Error sending data: " << WSAGetLastError() << endl;
		return SERVER_SENT_FAILED;
	}
	else
	{
		return SERVER_SENT_SUCCESS;
	}
}