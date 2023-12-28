//TCP client
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <vector>
#include <time.h>
#include "client.h"
#include "mergesort_v1.h"

using namespace std;

int main()
{
	SOCKET Connection = client_init();
	vector<DTYPE> arr_recv;
	char recvArrBuffer[SEND_RECV_BUFFER_SIZE];

	LARGE_INTEGER start, end;
	char* checkbuff = new char[1];
	*checkbuff = '\0';
	recv(Connection, checkbuff, 1, NULL);
	if (*checkbuff == SERVER_START_SENDING)
	{
		cout << "Start receiving array." << endl;
	}

	delete[] checkbuff;

	QueryPerformanceCounter(&start);
	while (true) {
		int bytesRecv = recv(Connection, recvArrBuffer, SEND_RECV_BUFFER_SIZE, NULL);
		if (bytesRecv == 0)
		{
			break;
		}
		else if (bytesRecv < 0)
		{
			std::cerr << "Error receiving data: " << WSAGetLastError() << std::endl;
			break;
		}

		arr_recv.insert(arr_recv.end(), reinterpret_cast<DTYPE*>(recvArrBuffer),
			reinterpret_cast<DTYPE*>(recvArrBuffer) + bytesRecv / sizeof(DTYPE));
	}

	QueryPerformanceCounter(&end);
	size_t time = end.QuadPart - start.QuadPart;
	cout << "Receiving Time Consumed:" << time << endl;

	if (arr_recv.size() == SORT_DATANUM)
	{
		cout << "Received Array Size is " << arr_recv.size() << endl;
		cout << "All data received successfully." << endl;
		cout << "***********************************************" << endl;
	}
	else
	{
		cout << "Received Array Size is" << arr_recv.size() << endl;
		cerr << "Data received unsuccessfully." << endl;
		cout << "***********************************************" << endl;
	}

	cout << "Client: start to sort" << endl;
	test_parallel(arr_recv, SORT_DATANUM);

	//cleanup
	closesocket(Connection);
	WSACleanup();
	return 0;
}

