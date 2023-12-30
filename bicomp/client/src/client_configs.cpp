//TCP client
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include "client.h"
#include "mergesort_v1.h"
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

std::vector<DTYPE> RecvArrayFromServer_bicomp(SOCKET& Connection, LARGE_INTEGER start)
{
	char* checkbuff = new char[1];
	*checkbuff = '\0';
	recv(Connection, checkbuff, 1, NULL);
	if (*checkbuff == SERVER_START_SENDING)
	{
		cout << "Start receiving array." << endl;
	}

	delete[] checkbuff;

	char recvArrBuffer[SEND_RECV_BUFFER_SIZE];
	char* arr_recv_tmp = new char[SORT_DATANUM * sizeof(DTYPE)];
	size_t recv_len = 0;
	while (recv_len < SORT_DATANUM * sizeof(DTYPE))
	{
		memset(recvArrBuffer, 0, SEND_RECV_BUFFER_SIZE);

		int bytesRecv = recv(Connection, recvArrBuffer, SEND_RECV_BUFFER_SIZE, NULL);
		if (recv_len == SORT_DATANUM * sizeof(DTYPE))
		{
			break;
		}
		else if (bytesRecv < 0)
		{
			std::cerr << "Error receiving data: " << WSAGetLastError() << std::endl;
			break;
		}
		memcpy(arr_recv_tmp + recv_len, recvArrBuffer, bytesRecv);
		recv_len += bytesRecv;
	}

	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	size_t time = end.QuadPart - start.QuadPart;
	cout << "Receiving Time Consumed:" << time << endl;

	// 将 char* 转换为 float*
	float* floatData = reinterpret_cast<float*>(arr_recv_tmp);
	// 创建一个存储 float 的 vector
	vector<DTYPE> arr_recv(floatData, floatData + SORT_DATANUM);
	delete[] arr_recv_tmp;
	return arr_recv;
}

int SendArrayBackToServer_bicomp(SOCKET& Connection, std::vector<DTYPE>& arr_recv)
{
	const char start_send_back = CLIENT_START_SENDING;
	cout << "Start Sending Array Back" << endl;
	send(Connection, &start_send_back, 1, NULL);

	int remainBytes = SORT_DATANUM * sizeof(DTYPE);
	while (remainBytes > 0)
	{
		int BytesToSend = min(SEND_RECV_BUFFER_SIZE, remainBytes);
		const char* dataToSend = reinterpret_cast<const char*>(arr_recv.data()) +
			(SORT_DATANUM * sizeof(DTYPE) - remainBytes); // 指向需要发送的数据的指针

		int bytesSent = send(Connection, dataToSend, BytesToSend, 0);
		if (checkSent(bytesSent, Connection) == SENT_ARRAY_SUCCESS)
		{
			remainBytes -= bytesSent;
			continue;
		}
		else
		{
			closesocket(Connection);
			WSACleanup();
			return -1;
		}
	}

	const char end_send = CLIENT_END_SENDING;
	cout << "End Sending Array" << endl;
	send(Connection, &end_send, 1, NULL);
	shutdown(Connection, SD_SEND);

	arr_recv.erase(arr_recv.begin(), arr_recv.end());
	return 0;
}



void CheckRemainArray_bicomp(std::vector<DTYPE>& arr_recv)
{
	cout << "Remaining Array Size is " << arr_recv.size() << endl;
	cout << "All data sent successfully." << endl;
	cout << "***********************************************" << endl;
}