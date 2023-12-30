#include <omp.h>
#include <vector>
#include <chrono>
#include <limits>

#define DTYPE float

// 串行求最大值函数
DTYPE arrayMaxSerial(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE max_val = std::numeric_limits<DTYPE>::min(); //最大值初始值设为该类型变量的最小值
    for (size_t i = 0; i < len; i++)
    {
        if (arr[i] > max_val)
        {
            max_val = arr[i];
        }
    }
    return max_val;
}

// 并行求最大值函数
DTYPE arrayMaxParallel(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE max_val = std::numeric_limits<DTYPE>::min(); //最大值初始值设为该类型变量的最小值
    omp_set_num_threads(8);  // 设置并行线程的数量为8
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

// 串行求和函数
DTYPE arraySumSerial(std::vector<DTYPE>& arr, size_t len) {
    DTYPE sum = 0;
    for (int i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}

// 并行求和函数
DTYPE arraySumParallel(std::vector<DTYPE>& arr, size_t len) {
    DTYPE sum = 0;
    omp_set_num_threads(8);  // 设置并行线程的数量为8
#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}