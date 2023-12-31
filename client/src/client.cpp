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
		if (bytesRecv != SEND_RECV_BUFFER_SIZE)
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

	// Sorting Array
	cout << "Client: start to sort" << endl;
	test_parallel(arr_recv, SORT_DATANUM); 

	//Send Array back
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

	QueryPerformanceCounter(&end);

	arr_recv.erase(arr_recv.begin(), arr_recv.end());

	cout << "Remaining Array Size is " << arr_recv.size() << endl;
	cout << "All data sent successfully." << endl;
	cout << "***********************************************" << endl;



	//cleanup
	closesocket(Connection);
	WSACleanup();
	return 0;
}

