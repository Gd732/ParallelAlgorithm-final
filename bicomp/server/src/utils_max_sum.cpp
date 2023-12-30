#include <omp.h>
#include <vector>
#include <chrono>
#include <limits>

#define DTYPE float

DTYPE arrayMaxSerial(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE max_val = std::numeric_limits<DTYPE>::min();
    for (size_t i = 0; i < len; i++)
    {
        if (arr[i] > max_val)
        {
            max_val = arr[i];
        }
    }
    return max_val;
}

DTYPE arrayMaxParallel(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE max_val = std::numeric_limits<DTYPE>::min();
    omp_set_num_threads(8);
#pragma omp parallel for
    for (size_t i = 0; i < len; i++)
    {
#pragma omp critical
        if (arr[i] > max_val)
        {
            max_val = arr[i];
        }
    }
    return max_val;
}

DTYPE arraySumSerial(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE sum = 0;
    for (int i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}

DTYPE arraySumParallel(std::vector<DTYPE>& arr, size_t len) {
    DTYPE sum = 0;
    omp_set_num_threads(8);
#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}