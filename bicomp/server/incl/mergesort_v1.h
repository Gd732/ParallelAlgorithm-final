#pragma once

#define MAX_THREADS 128
#define SUBDATANUM 100000
#define DATANUM (SUBDATANUM * MAX_THREADS)  
#define SORT_DATANUM DATANUM/2 // Half of the original data
#define NUM_THREADS 64 
#define MAX_LEVEL 6

#define GET_SEC(start, end) ((end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec)*1e-6)
#define DTYPE float


float GetRandomFloat();

void vector_init(std::vector<DTYPE>& arr, size_t size);

//bool checkSorted(T* arr, size_t size);
bool checkSorted(std::vector<DTYPE>& arr, size_t size);

void merge(std::vector<DTYPE>& arr, /*std::vector<DTYPE>& tmp,*/ size_t low, size_t mid, size_t high);
//void non_parallel_merge_sort(T arr[], size_t low, size_t high);
void non_parallel_merge_sort(std::vector<DTYPE>& arr, /*std::vector<DTYPE>& tmp,*/ size_t low, size_t high);
void parallel_merge_sort(std::vector<DTYPE>& arr, /*std::vector<DTYPE>& tmp,*/ size_t low, size_t high, size_t level);

size_t test_non_parallel(std::vector<DTYPE>& arr, /*std::vector<DTYPE>& tmp,*/ size_t size);
size_t test_parallel(std::vector<DTYPE>& arr, /*std::vector<DTYPE>& tmp,*/ size_t size);

void insertSort(std::vector<DTYPE>& arr, size_t low, size_t high);

void compareSort();
