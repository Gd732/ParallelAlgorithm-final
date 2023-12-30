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
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup error", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	vector<DTYPE> arr_recv0; vector<DTYPE> arr_recv1; vector<DTYPE> arr_recv2;

	{
#pragma omp parallel
		{
#pragma omp sections
			{
#pragma omp section
				{
					SOCKET Connection0 = CreateConnection(SOCKET_PORT0);
					cout << "con0 is created!" << endl;
					vector<DTYPE> returnedVectorRef = RecvArrayFromServer_multicomp(Connection0, SOCKET_PORT0);
					arr_recv0.assign(returnedVectorRef.begin(), returnedVectorRef.end());
					returnedVectorRef.clear();
					test_parallel(arr_recv0, DATANUM / 4);
					cout << "This is con0" << endl;
					SendArrayBackToServer_multicomp(Connection0, arr_recv0);
					closesocket(Connection0);
				}
#pragma omp section
				{
					SOCKET Connection1 = CreateConnection(SOCKET_PORT1);
					cout << "con1 is created!" << endl;
					vector<DTYPE> returnedVectorRef = RecvArrayFromServer_multicomp(Connection1, SOCKET_PORT1);
					arr_recv1.assign(returnedVectorRef.begin(), returnedVectorRef.end());
					returnedVectorRef.clear();
					test_parallel(arr_recv1, DATANUM / 4);
					cout << "This is con1" << endl;
					SendArrayBackToServer_multicomp(Connection1, arr_recv1);
					closesocket(Connection1);

				}
#pragma omp section
				{
					SOCKET Connection2 = CreateConnection(SOCKET_PORT2);
					cout << "con2 is created!" << endl;
					vector<DTYPE> returnedVectorRef = RecvArrayFromServer_multicomp(Connection2, SOCKET_PORT2);
					cout << "successfully received" << endl;
					arr_recv2.assign(returnedVectorRef.begin(), returnedVectorRef.end());
					cout << "recv2 is " <<  arr_recv2.size() << endl;
					returnedVectorRef.clear();
					test_parallel(arr_recv2, DATANUM / 4);
					cout << "This is con2" << endl;
					SendArrayBackToServer_multicomp(Connection2, arr_recv2);
					closesocket(Connection2);
				}
#pragma omp taskwait
			}
		}
	}

	WSACleanup();
	return 0;
}

