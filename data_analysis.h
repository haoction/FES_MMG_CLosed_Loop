#ifndef DATA_ANALYSIS_H
#define DATA_ANALYSIS_H

#include <cstddef> // For size_t

int filter_and_calculate_average(const int* sensor_data, size_t data_size);
int calculate_fatigue_level(int average);

#endif