//TCP client
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string> 
#include "client.h"
using namespace std;

void addr_init(SOCKADDR_IN& addr)
{
	addr.sin_family = AF_INET; //IPv4 Socket
	addr.sin_port = htons(SOCKET_PORT); // sever Port
	addr.sin_addr.s_addr = inet_addr(SOCKET_IPADDR); //server PC
}

SOCKET client_init()
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup error", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	SOCKADDR_IN addr; //Adres przypisany do socketu Connection
	int sizeofaddr = sizeof(addr);
	addr_init(addr);

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //Connection
	{
		MessageBoxA(NULL, "Bad Connection", "Error", MB_OK | MB_ICONERROR);
		return NULL;
	}
	return Connection;
}

bool checkSent(int bytesSent, SOCKET& Connection)
{
	if (bytesSent == SOCKET_ERROR)
	{
		cerr << "Error sending data: " << WSAGetLastError() << endl;
		return SENT_ARRAY_FAILED;
	}
	else
	{
		return SENT_ARRAY_SUCCESS;
	}
}