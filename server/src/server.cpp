//TCP server
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <iostream>
#include <Windows.h>
#include <vector>
#include "server.h"
#include "mergesort_v1.h"


using namespace std;

int main()
{
	SOCKET Connection = server_init();
	LARGE_INTEGER start, end;
	vector<DTYPE> arr_full(DATANUM);
	cout << "Initiating the Array, Please wait." << endl;
	vector_init(arr_full, DATANUM);
	cout << "Initiation Finished." << endl;
	cout << "***********************************************" << endl;
	QueryPerformanceCounter(&start);

	if (Connection == 0)
	{
		std::cout << "bad connection." << std::endl;
	}
	else
	{
		const char start_send = SERVER_START_SENDING;
		cout << "Start Sending Array" << endl;
		send(Connection, &start_send, 1, NULL);
		
		int remainBytes = SORT_DATANUM * sizeof(DTYPE);
		while (remainBytes > 0)
		{
			int BytesToSend = min(SEND_RECV_BUFFER_SIZE, remainBytes);
			const char* dataToSend = reinterpret_cast<const char*>(arr_full.data()) +
				(SORT_DATANUM * sizeof(DTYPE) - remainBytes); // 指向需要发送的数据的指针

			int bytesSent = send(Connection, dataToSend, BytesToSend, 0);
			if (checkSent(bytesSent, Connection) == SERVER_SENT_SUCCESS)
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
	}

	QueryPerformanceCounter(&end);
	size_t time = end.QuadPart - start.QuadPart;

	arr_full.erase(arr_full.begin() + SORT_DATANUM, arr_full.end());
	cout << "Sending Time Consumed:" << time << endl;
	cout << "Remaining Array Size is " <<  arr_full.size() << endl;
	cout << "All data sent successfully." << endl;
	cout << "***********************************************" << endl;
	cout << "Server: start to sort" << endl;

	test_parallel(arr_full, SORT_DATANUM);


	//cleanup
	closesocket(Connection);
	WSACleanup();
	return 0;
}