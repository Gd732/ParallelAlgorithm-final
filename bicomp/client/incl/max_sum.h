#pragma once
#define DTYPE float

DTYPE arrayMaxSerial(std::vector<DTYPE>& arr, size_t len);
DTYPE arrayMaxParallel(std::vector<DTYPE>& arr, size_t len);
DTYPE arraySumSerial(std::vector<DTYPE>& arr, size_t len);
DTYPE arraySumParallel(std::vector<DTYPE>& arr, size_t len);


