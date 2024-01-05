#include <iostream>
#include <stdlib.h>
#include <string>
#include <iomanip>
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

	LARGE_INTEGER start, end;
	QueryPerformanceCounter(&start);
	non_parallel_merge_sort(arr_full,0, DATANUM-1);
	QueryPerformanceCounter(&end);
	size_t time_non_parallel = end.QuadPart - start.QuadPart;
	cout << "Time Consumed(non parallel):" << time_non_parallel << endl;
	bool is_sorted = checkSorted(arr_full, DATANUM);
	cout << (is_sorted ? "Array is sorted correctly after non parallel sorting"
		: "Array is not sorted correctly after non parallel sorting") << endl;

	vector_init(arr_full, DATANUM);
	QueryPerformanceCounter(&start);
	parallel_merge_sort(arr_full, 0, DATANUM - 1);
	QueryPerformanceCounter(&end);
	size_t time_parallel = end.QuadPart - start.QuadPart;
	cout << "Time Consumed(parallel):" << time_parallel << endl;
	is_sorted = checkSorted(arr_full, DATANUM);
	cout << (is_sorted ? "Array is sorted correctly after parallel sorting"
		: "Array is not sorted correctly after parallel sorting") << endl;
	cout << "The Speedup Ratio is " << std::fixed << std::setprecision(2)
		<< (float(time_non_parallel) / float(time_parallel)) * 100 << "%" << endl;
	cout << "***********************************************" << endl;

	LARGE_INTEGER start_max, end_max;
	QueryPerformanceCounter(&start_max);
	DTYPE max1 = arrayMaxSerial(arr_full, DATANUM);
	QueryPerformanceCounter(&end_max);
	size_t time_max_serial = end_max.QuadPart - start_max.QuadPart;

	QueryPerformanceCounter(&start_max);
	DTYPE max2 = arrayMaxParallel(arr_full, DATANUM);
	QueryPerformanceCounter(&end_max);	
	size_t time_max_parallel = end_max.QuadPart - start_max.QuadPart;

	QueryPerformanceCounter(&start_max);
	DTYPE max3 = arrayMaxSerialSIMD(arr_full, DATANUM);
	QueryPerformanceCounter(&end_max);
	size_t time_max_serial_simd = end_max.QuadPart - start_max.QuadPart;

	QueryPerformanceCounter(&start_max);
	DTYPE max4 = arrayMaxParallelSIMD(arr_full, DATANUM);
	QueryPerformanceCounter(&end_max);
	size_t time_max_parallel_simd = end_max.QuadPart - start_max.QuadPart;

	cout << "time for max(serial): " << time_max_serial << endl;
	cout << "time for max(parallel): " << time_max_parallel << endl;
	cout << "time for max(serial+simd): " << time_max_serial_simd << endl;
	cout << "time for max(parallel+simd): " << time_max_parallel_simd << endl;
	cout << "Max: " << max1 << ' ' << max2 << ' ' << max3 << ' ' << max4  << endl;

	cout << "***********************************************" << endl;

	LARGE_INTEGER start_sum, end_sum;
	QueryPerformanceCounter(&start_sum);
	DTYPE sum1 = arraySumKahanSerial(arr_full, DATANUM);
	QueryPerformanceCounter(&end_sum);
	size_t time_sum_serial = end_sum.QuadPart - start_sum.QuadPart;

	QueryPerformanceCounter(&start_sum);
	DTYPE sum2 = arraySumKahanParallel(arr_full, DATANUM);
	QueryPerformanceCounter(&end_sum);
	size_t time_sum_parallel = end_sum.QuadPart - start_sum.QuadPart;

	QueryPerformanceCounter(&start_sum);
	DTYPE sum3 = arraySumKahanSerialSIMD(arr_full, DATANUM);
	QueryPerformanceCounter(&end_sum);
	size_t time_sum_serial_simd = end_sum.QuadPart - start_sum.QuadPart;

	QueryPerformanceCounter(&start_sum);
	DTYPE sum4 = arraySumKahanParallelSIMD(arr_full, DATANUM);
	QueryPerformanceCounter(&end_sum);
	size_t time_sum_parallel_simd = end_sum.QuadPart - start_sum.QuadPart;

	cout << "time for max(serial): " << time_sum_serial << endl;
	cout << "time for max(parallel): " << time_sum_parallel << endl;
	cout << "time for max(serial+simd): " << time_sum_serial_simd << endl;
	cout << "time for max(parallel+simd): " << time_sum_parallel_simd << endl;
	cout << "Sum: " << sum1 << ' ' << sum2 << ' ' << sum3 << ' ' << sum4 << endl;

	return 0;
}