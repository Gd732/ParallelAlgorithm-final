#include <omp.h>
#include <vector>
#include <chrono>
#include <math.h>
#include <immintrin.h> 
#include <limits>

#define DTYPE float

DTYPE arrayMaxSerial(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE max_val = std::numeric_limits<DTYPE>::min();
    for (size_t i = 0; i < len; i++)
    {
        if (log(sqrt(arr[i])) > max_val)
        {
            max_val = log(sqrt(arr[i]));
        }
    }
    return max_val;
}

DTYPE arrayMaxSerialSIMD(std::vector<DTYPE>& arr, size_t len) {
    __m256 max_values = _mm256_set1_ps(std::numeric_limits<DTYPE>::min());

    for (size_t i = 0; i < len; i += 8) {
        __m256 arr_vector = _mm256_loadu_ps(&arr[i]);
        arr_vector = _mm256_log_ps(_mm256_sqrt_ps(arr_vector));
        max_values = _mm256_max_ps(max_values, arr_vector);
    }
    float result[8];
    _mm256_storeu_ps(result, max_values);

    DTYPE max_val = result[0];
    for (size_t i = 1; i < 4; ++i) {
        if (result[i] > max_val) {
            max_val = result[i];
        }
    }
    return max_val;
}

DTYPE arrayMaxParallel(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE max_val = std::numeric_limits<DTYPE>::min();
#pragma omp num_threads(8)
#pragma omp parallel
    for (size_t i = 0; i < len; i++)
    {
#pragma omp critical
        if (log(sqrt(arr[i])) > max_val)
        {
            max_val = log(sqrt(arr[i]));
        }
    }
    return max_val;
}

DTYPE arrayMaxParallelSIMD(std::vector<DTYPE>& arr, size_t len) {
    __m256 max_values = _mm256_set1_ps(std::numeric_limits<DTYPE>::min());

#pragma omp num_threads(8)
#pragma omp parallel
    for (size_t i = 0; i < len; i += 8) {
        __m256 arr_vector = _mm256_loadu_ps(&arr[i]);
        arr_vector = _mm256_log_ps(_mm256_sqrt_ps(arr_vector));
        max_values = _mm256_max_ps(max_values, arr_vector);
    }
    DTYPE result[8];
    _mm256_storeu_ps(result, max_values);

    DTYPE max_val = result[0];
    for (size_t i = 1; i < 4; ++i) {
        if (result[i] > max_val) {
            max_val = result[i];
        }
    }
    return max_val;
}

DTYPE arraySumSerial(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE sum = 0;
    for (int i = 0; i < len; i++) {
        sum += log(sqrt(arr[i]));
    }
    return sum;
}

DTYPE arraySumSerialSIMD(std::vector<DTYPE>& arr, size_t len) {
    __m256 sum_vector = _mm256_setzero_ps();
    for (size_t i = 0; i < len; i += 8)
    {
        __m256 arr_vector = _mm256_loadu_ps(&arr[i]);
        arr_vector = _mm256_log_ps(_mm256_sqrt_ps(arr_vector));
        sum_vector = _mm256_add_ps(sum_vector, arr_vector);
    }

    DTYPE sum_result[8];
    _mm256_storeu_ps(sum_result, sum_vector);

    DTYPE sum = 0.0f;
    for (size_t i = 0; i < 8; ++i) {
        sum += sum_result[i];
    }
    return sum;
}

DTYPE arraySumParallel(std::vector<DTYPE>& arr, size_t len) {
    DTYPE sum = 0;
#pragma omp num_threads(8);
#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < len; i++) {
        sum += log(sqrt(arr[i]));
    }
    return sum;
}

DTYPE arraySumParallelSIMD(std::vector<DTYPE>& arr, size_t len) {
    __m256 sum_vector = _mm256_setzero_ps();
#pragma omp num_threads(8);
#pragma omp parallel
    for (size_t i = 0; i < len; i += 8)
    {
        __m256 arr_vector = _mm256_loadu_ps(&arr[i]);
        arr_vector = _mm256_log_ps(_mm256_sqrt_ps(arr_vector));
        sum_vector = _mm256_add_ps(sum_vector, arr_vector);
    }

    DTYPE sum_result[8];
    _mm256_storeu_ps(sum_result, sum_vector);

    DTYPE sum = 0.0f;
    for (size_t i = 0; i < 8; ++i) {
        sum += sum_result[i];
    }
    return sum;
}

DTYPE arraySumKahanSerial(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE sum = 0.0;
    DTYPE compensation = 0.0;
    for (int i = 0; i < len; i++) {
        DTYPE y = log(sqrt(arr[i])) - compensation;
        DTYPE t = sum + y;
        compensation = (t - sum) - y;
        sum = t;
    }
    return sum;
}
DTYPE arraySumKahanSerialSIMD(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE sum = 0.0;
    DTYPE compensation = 0.0;

    __m256 sum_vector = _mm256_setzero_ps();
    __m256 compensation_vector = _mm256_setzero_ps();
    for (size_t i = 0; i < len; i += 8) {
        __m256 arr_vector = _mm256_loadu_ps(&arr[i]);
        arr_vector = _mm256_log_ps(_mm256_sqrt_ps(arr_vector));
        __m256 y_vector = _mm256_sub_ps(arr_vector, compensation_vector);
        __m256 t_vector = _mm256_add_ps(sum_vector, y_vector);
        compensation_vector = _mm256_sub_ps(_mm256_sub_ps(t_vector, sum_vector), y_vector);
        sum_vector = t_vector;
    }
    float sum_result[8];
    _mm256_storeu_ps(sum_result, sum_vector);
    // 计算最终求和
    for (int i = 0; i < 8; ++i) {
        sum += sum_result[i];
    }
    return sum;
}

DTYPE arraySumKahanParallelSIMD(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE sum = 0.0;
    DTYPE compensation = 0.0;

    __m256 sum_vector = _mm256_setzero_ps();
    __m256 compensation_vector = _mm256_setzero_ps();
#pragma omp num_threads(8);
#pragma omp parallel 
    for (size_t i = 0; i < len; i += 8) {
        __m256 arr_vector = _mm256_loadu_ps(&arr[i]);
        arr_vector = _mm256_log_ps(_mm256_sqrt_ps(arr_vector));
        __m256 y_vector = _mm256_sub_ps(arr_vector, compensation_vector);
        __m256 t_vector = _mm256_add_ps(sum_vector, y_vector);
        compensation_vector = _mm256_sub_ps(_mm256_sub_ps(t_vector, sum_vector), y_vector);
        sum_vector = t_vector;
    }
    float sum_result[8];
    _mm256_storeu_ps(sum_result, sum_vector);
    // 计算最终求和
    for (int i = 0; i < 8; ++i) {
        sum += sum_result[i];
    }
    return sum;
}

DTYPE arraySumKahanParallel(std::vector<DTYPE>& arr, size_t len)
{
    DTYPE sum = 0.0;
    DTYPE compensation = 0.0;

#pragma omp num_threads(8);
#pragma omp parallel for reduction(+:sum) reduction(+:compensation)
    for (int i = 0; i < len; i++) {
        DTYPE y = log(sqrt(arr[i])) - compensation;
        DTYPE t = sum + y;
        compensation = (t - sum) - y;
        sum = t;
    }
    return sum;
}

