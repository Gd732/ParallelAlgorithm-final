#include <iostream>
#include <random>
#include <iomanip>
#include <time.h>
#include <windows.h>
#include "mergesort_v1.h"
using namespace std;

timeval start, end;

float GetRandomFloat()
{
    float min = 0.0;
    float max = 100.0;
    float number = (float)rand() / (float)RAND_MAX * (max - min) + min;
    return number;
}

void vector_init(vector<DTYPE>& arr, size_t size)
{
    
    for (size_t i = 0; i < size; ++i)
    {
        arr[i] = GetRandomFloat();
    }
}
//void init(float arr[], size_t size)
//{
//    for (size_t i = 0; i < size; ++i)
//    {
//        arr[i] = GetRandomFloat();
//    }
//}

bool checkSorted(vector<DTYPE>& arr, size_t size) 
{
    for (size_t i = 0; i < size - 1; i++) 
    {
        if (arr[i] > arr[i + 1]) {
            return false;
        }
    }
    return true;
}

void merge(vector<DTYPE>& arr, /*vector<DTYPE>& tmp,*/ size_t low, size_t mid, size_t high)
{
    vector<DTYPE> tmp(high - low + 1);
    size_t i = low, j = mid + 1, k = 0;
    while (i <= mid && j <= high)
    {
        if (arr[i] < arr[j])
        {
            tmp[k] = arr[i];
            k++; i++;
        }
        else
        {
            tmp[k] = arr[j];
            k++; j++;
        }
    }

    while (i <= mid)
    {
        tmp[k] = arr[i];
        k++; i++;
    }

    while (j <= high)
    {
        tmp[k] = arr[j];
        k++; j++;
    }

    for (i = low, k = 0; i <= high; i++, k++)
    {
        arr[i] = tmp[k];
    }
}

void non_parallel_merge_sort(vector<DTYPE>& arr, /*vector<DTYPE>& tmp,*/ size_t low, size_t high)
{
    if (low >= high)
    {
        return;
    }

    size_t mid = (low + high) / 2;
    non_parallel_merge_sort(arr, /*tmp,*/ low, mid);
    non_parallel_merge_sort(arr, /*tmp,*/ mid + 1, high);
    merge(arr, /*tmp,*/ low, mid, high);
}

void parallel_merge_sort(vector<DTYPE>& arr, /*vector<DTYPE>& tmp,*/ size_t low, size_t high, size_t level)
{
    if (high - low <= 10) {
        insertSort(arr, low, high);
        return;
    }
#pragma omp parallel num_threads(NUM_THREADS)
    {
#pragma omp single nowait
        {

            size_t mid = (low + high) / 2;
#pragma omp task if (level <= MAX_LEVEL)
            {
                parallel_merge_sort(arr, /*tmp,*/ low, mid, level + 1);
            }
            parallel_merge_sort(arr, /*tmp,*/ mid + 1, high, level + 1);
#pragma omp taskwait

            if (arr[mid] <= arr[mid + 1])
            {
                return;
            }
            merge(arr, /*tmp,*/ low, mid, high);
        }
    }
}


size_t test_non_parallel(vector<DTYPE>&arr, /*vector<DTYPE>&tmp,*/ size_t size)
{
    LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);
    non_parallel_merge_sort(arr, /*tmp,*/ 0, size - 1);
    QueryPerformanceCounter(&end);
    size_t time = end.QuadPart - start.QuadPart;
    cout << "Time Consumed(non parallel):" << time << endl;

    bool is_nonparallel_sorted = checkSorted(arr, size);
    cout << (is_nonparallel_sorted ? "Array is sorted correctly after non parallel sorting"
        : "Array is not sorted correctly after non parallel sorting") << endl;

    return time;
}

size_t test_parallel(vector<DTYPE>& arr, /*vector<DTYPE>& tmp,*/ size_t size)
{
    LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);//start  
    parallel_merge_sort(arr, /*tmp,*/ 0, size - 1, 0);
    QueryPerformanceCounter(&end);//end
    size_t time = end.QuadPart - start.QuadPart;
    cout << "Time Consumed(parallel):" << time << endl;

    bool is_parallel_sorted = checkSorted(arr, size);
    cout << (is_parallel_sorted ? "Array is sorted correctly after parallel sorting"
        : "Array is not sorted correctly after parallel sorting") << endl;

    return time;
}


void insertSort(vector<DTYPE>& arr, size_t low, size_t high)
{
    for (size_t i = low + 1; i <= high; ++i) {
        DTYPE key = arr[i];
        size_t j = i - 1;

        // Move elements of array[low..i-1] that are greater than key to one position ahead of their current position
        while (j >= low && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

void compareSort()
{
    cout << "Array size: " << DATANUM << endl;
    vector<DTYPE> arr(DATANUM);


    vector_init(arr, DATANUM);
    cout << "Successfully assigned values to the array" << endl;
    size_t time_parl = test_parallel(arr, /*tmp,*/ DATANUM);
    cout << endl << "***************************************" << endl << endl;

    vector_init(arr, DATANUM);
    cout << "Successfully assigned values to the array" << endl;
    size_t time_nparl = test_non_parallel(arr, /*tmp,*/ DATANUM);

    cout << endl << "***************************************" << endl << endl;


    cout << "The Speedup Ratio is " << std::fixed << std::setprecision(2)
        << (float(time_nparl) / float(time_parl)) * 100 << "%" << endl;
}