#pragma once
#define DTYPE float
#include <vector>


DTYPE arrayMaxSerial(std::vector<DTYPE>& arr, size_t len);
DTYPE arrayMaxParallel(std::vector<DTYPE>& arr, size_t len);
DTYPE arraySumSerial(std::vector<DTYPE>& arr, size_t len);
DTYPE arraySumParallel(std::vector<DTYPE>& arr, size_t len);
DTYPE arraySumKahanParallel(std::vector<DTYPE>& arr, size_t len);
DTYPE arraySumKahanSerial(std::vector<DTYPE>& arr, size_t len);

