#include <omp.h>
#include <vector>
#include <chrono>
#include <math.h>
#include <limits>

#define DTYPE float

DTYPE arrayMaxSerial(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE max_val = std::numeric_limits<DTYPE>::min();
    for (size_t i = 0; i < len; i++)
    {
        if (pow((sin(sqrt(arr[i]))), 2) > max_val)
        {
            max_val = pow((sin(sqrt(arr[i]))), 2);
        }
    }
    return max_val;
}

DTYPE arrayMaxParallel(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE max_val = std::numeric_limits<DTYPE>::min();
    // omp_set_num_threads(8);
#pragma omp num_threads(8)
#pragma omp parallel for
    for (size_t i = 0; i < len; i++)
    {
#pragma omp critical
        if (pow((sin(sqrt(arr[i]))), 2) > max_val)
        {
            max_val = pow((sin(sqrt(arr[i]))), 2);
        }
    }
    return max_val;
}

DTYPE arraySumSerial(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE sum = 0;
    for (int i = 0; i < len; i++) {
        sum += pow((sin(sqrt(arr[i]))), 2);
    }
    return sum;
}

DTYPE arraySumParallel(std::vector<DTYPE>& arr, size_t len) {
    DTYPE sum = 0;
    // omp_set_num_threads(8);
#pragma omp num_threads(8)
#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < len; i++) {
        sum += pow((sin(sqrt(arr[i]))), 2);
    }
    return sum;
}


DTYPE arraySumKahanParallel(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE sum = 0.0;
    DTYPE compensation = 0.0;

#pragma omp num_threads(8)
#pragma omp parallel for reduction(+:sum) reduction(+:compensation)
    for (int i = 0; i < len; i++) {
        DTYPE y = pow((sin(sqrt(arr[i]))), 2) - compensation;
        DTYPE t = sum + y;
        compensation = (t - sum) - y;
        sum = t;
    }
    return sum;
}

DTYPE arraySumKahanSerial(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE sum = 0.0;
    DTYPE compensation = 0.0;
    for (int i = 0; i < len; i++) {
        DTYPE y = pow((sin(sqrt(arr[i]))), 2) - compensation;
        DTYPE t = sum + y;
        compensation = (t - sum) - y;
        sum = t;
    }
    return sum;
}