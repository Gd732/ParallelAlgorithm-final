#include <iostream>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <vector>
#include <time.h>
#include "mergesort_v1.h"
#include "max_sum.h"
using namespace std;

int main()
{
	vector<DTYPE> arr_full(DATANUM);
	vector_init(arr_full, DATANUM);
	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);


	// sorting array
	cout << "Start to sort" << endl;
	test_non_parallel(arr_full, DATANUM);
	DTYPE client_max = arrayMaxSerial(arr_full, DATANUM);
	DTYPE client_sum = arraySumSerial(arr_full, DATANUM);

	// get the final time cost
	GetFinalTimeCost(start, true);
	return 0;
}