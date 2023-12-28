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
	LARGE_INTEGER start;

	vector<DTYPE> arr_full(DATANUM);
	cout << "Initiating the Array, Please wait." << endl;
	vector_init(arr_full, DATANUM);
	cout << "Initiation Finished." << endl;
	cout << "***********************************************" << endl;
	QueryPerformanceCounter(&start);

	if (Connection == 0)
	{
		std::cout << "bad connection." << std::endl;
		return -1;
	}
	
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
	
	const char end_send = SERVER_END_SENDING;
	cout << "End Sending Array" << endl;
	send(Connection, &end_send, 1, NULL);

	LARGE_INTEGER mark_server_send;
	QueryPerformanceCounter(&mark_server_send);
	size_t time_server_send = mark_server_send.QuadPart - start.QuadPart;

	arr_full.erase(arr_full.begin() + SORT_DATANUM, arr_full.end());
	cout << "Sending Time Consumed:" << time_server_send << endl;
	cout << "Remaining Array Size is " <<  arr_full.size() << endl;
	cout << "All data sent successfully." << endl;
	cout << "***********************************************" << endl;
	cout << "Server: start to sort" << endl;

	// sort the array
	test_parallel(arr_full, SORT_DATANUM);

	//start to receive back
	char recvArrBuffer[SEND_RECV_BUFFER_SIZE];

	char* checkbuff = new char[1];
	*checkbuff = '\0';
	recv(Connection, checkbuff, 1, NULL);
	if (*checkbuff == SERVER_START_SENDING)
	{
		cout << "Start receiving array." << endl;
	}
	delete[] checkbuff;

	LARGE_INTEGER mark_server_recv_back;
	QueryPerformanceCounter(&mark_server_recv_back);
	size_t time_server_recv = mark_server_recv_back.QuadPart - start.QuadPart;

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

		arr_full.insert(arr_full.end(), reinterpret_cast<DTYPE*>(recvArrBuffer),
			reinterpret_cast<DTYPE*>(recvArrBuffer) + bytesRecv / sizeof(DTYPE));
	}


	cout << "Back Array Size is " << arr_full.size() << endl;
	merge(arr_full, 0, SORT_DATANUM - 1, DATANUM - 1);
	bool is_parallel_sorted = checkSorted(arr_full, DATANUM);
	cout << (is_parallel_sorted ? "Array is sorted correctly after bi-computer parallel sorting"
		: "Array is not sorted correctly after bi-computer parallel sorting") << endl;

	//if (arr_full.size() == SORT_DATANUM)
	//{
	//	cout << "Received Array Size is " << arr_full.size() << endl;
	//	cout << "All data received successfully." << endl;
	//	cout << "***********************************************" << endl;
	//}
	//else
	//{
	//	cout << "Received Array Size is" << arr_recv.size() << endl;
	//	cerr << "Data received unsuccessfully." << endl;
	//	cout << "***********************************************" << endl;
	//}
	cout << "***********************************************" << endl;
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	size_t time_total = end.QuadPart - start.QuadPart;
	cout << "Total Time Consumed:" << time_total << endl;
	//cleanup
	closesocket(Connection);
	WSACleanup(); 
	return 0;
}