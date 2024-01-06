#include <iostream>
#include <random>
#include <iomanip>
#include <time.h>
#include <stdio.h>
#include <windows.h>
#include <immintrin.h>
#include <intrin.h>
#include "mergesort_v1.h"
using namespace std;

timeval start, end;

float GetRandomFloat()
{
    float min = 1;
    float max = 10000.0;
    float number = (float)rand() / (float)RAND_MAX * (max - min) + min;
    return number;
}

void vector_init(vector<DTYPE>& arr, size_t size)
{
    cout << "Initiating the Array, Please wait." << endl;
    srand(1);
     for (size_t i = 0; i < size; ++i)
     {
         arr[i] = GetRandomFloat();
     }
    //for (size_t i = 0; i < DATANUM; i++)//数据初始化
    //{
    //    arr[i] = float(DATANUM + 1 - i);
    //}
    cout << "Initiation Finished." << endl;
    cout << "***********************************************" << endl;
}


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

void merge(vector<DTYPE>& arr, size_t low, size_t mid, size_t high)
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

void non_parallel_merge_sort(vector<DTYPE>& arr, size_t low, size_t high)
{
    if (low >= high)
    {
        return;
    }

    size_t mid = (low + high) / 2;
    non_parallel_merge_sort(arr, low, mid);
    non_parallel_merge_sort(arr, mid + 1, high);
    merge(arr, low, mid, high);
}

void parallel_merge_sort(vector<DTYPE>& arr, size_t low, size_t high)
{
    size_t itval = high - low;
    if (itval < 20) {
        if (itval == 3)
        {
            innerSortSIMD(arr, low, high);
            return;
        }
        insertSort(arr, low, high);
        return;
    }
#pragma omp parallel num_threads(NUM_THREADS)
    {
#pragma omp single nowait
        {
            size_t mid = (low + high) / 2;
            {
                parallel_merge_sort(arr, low, mid);
            }
            parallel_merge_sort(arr, mid + 1, high);
#pragma omp taskwait

            if (arr[mid] <= arr[mid + 1])
            {
                return;
            }
            merge(arr, low, mid, high);
        }
    }
}

size_t test_non_parallel(vector<DTYPE>& arr, size_t size)
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

size_t test_parallel(vector<DTYPE>& arr, size_t size)
{
    LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);//start  
    parallel_merge_sort(arr, /*tmp,*/ 0, size - 1);
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
        while (j >= low && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}


inline __m128 simd_sort_1V(__m128 input)
{
    {
        __m128 perm_neigh = _mm_shuffle_ps(input, input, _MM_SHUFFLE(2, 3, 0, 1));
        __m128 perm_neigh_min = _mm_min_ps(input, perm_neigh);
        __m128 perm_neigh_max = _mm_max_ps(input, perm_neigh);
        input = _mm_shuffle_ps(perm_neigh_min, perm_neigh_max, _MM_SHUFFLE(2, 0, 2, 0));
    }
    {
        __m128 perm_neigh = _mm_shuffle_ps(input, input, _MM_SHUFFLE(0, 1, 2, 3));
        __m128 perm_neigh_min = _mm_min_ps(input, perm_neigh);
        __m128 perm_neigh_max = _mm_max_ps(input, perm_neigh);
        input = _mm_shuffle_ps(perm_neigh_min, perm_neigh_max, _MM_SHUFFLE(1, 0, 3, 2));
    }
    {
        __m128 perm_neigh = _mm_shuffle_ps(input, input, _MM_SHUFFLE(2, 3, 0, 1));
        __m128 perm_neigh_min = _mm_min_ps(input, perm_neigh);
        __m128 perm_neigh_max = _mm_max_ps(input, perm_neigh);
        input = _mm_shuffle_ps(perm_neigh_min, perm_neigh_max, _MM_SHUFFLE(2, 0, 2, 0));
    }

    return input;
}

void innerSortSIMD(vector<DTYPE>& arr, size_t low, size_t high)
{
    __m128 floatVector = _mm_loadu_ps(&arr[low]);
    floatVector = simd_sort_1V(floatVector);
    _mm_storeu_ps(&arr[low], floatVector);
    //for (float value : getvector) {
    //    std::cout << value << " ";
    //}    
}

void compareSort()
{
    cout << "Array size: " << DATANUM << endl;
    vector<DTYPE> arr(DATANUM);

    vector_init(arr, DATANUM);
    cout << "Successfully assigned values to the array" << endl;
    size_t time_parl = test_parallel(arr, DATANUM);

    vector_init(arr, DATANUM);
    cout << "Successfully assigned values to the array" << endl;
    size_t time_nparl = test_non_parallel(arr, DATANUM);



    cout << "The Speedup Ratio is " << std::fixed << std::setprecision(2)
        << (float(time_nparl) / float(time_parl)) * 100 << "%" << endl;
}