#pragma once

#include <immintrin.h> 

#define MAX_THREADS 128
#define SUBDATANUM 10000
#define DATANUM (MAX_THREADS*SUBDATANUM)
#define NUM_THREADS 16

#define GET_SEC(start, end) ((end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec)*1e-6)
#define DTYPE float


float GetRandomFloat();

void vector_init(std::vector<DTYPE>& arr, size_t size);

bool checkSorted(std::vector<DTYPE>& arr, size_t size);

void merge(std::vector<DTYPE>& arr, size_t low, size_t mid, size_t high);

void non_parallel_merge_sort(std::vector<DTYPE>& arr, size_t low, size_t high);
void parallel_merge_sort(std::vector<DTYPE>& arr, size_t low, size_t high);

size_t test_non_parallel(std::vector<DTYPE>& arr, size_t size);
size_t test_parallel(std::vector<DTYPE>& arr, size_t size);

void insertSort(std::vector<DTYPE>& arr, size_t low, size_t high);

inline __m128 simd_sort_1V(__m128 input);
void innerSortSIMD(std::vector<DTYPE>& arr, size_t low, size_t high);

void compareSort();
