#ifndef DATA_ANALYSIS_H
#define DATA_ANALYSIS_H

#include "mbed.h"
#include <cstdint>

// Forward declaration for BMP280 sensor class
class BMP280_MBED;

// Configuration constants
constexpr size_t MAX_DATA_SIZE = 1000;
constexpr size_t DATA_POINTS = 100;
constexpr size_t BUFFER_SIZE = 100;

// FES parameters structure
struct FESParameters {
    int amplitude;
    int width;
    int frequency;
};

// Shared buffer declarations
extern int sensor_data[DATA_POINTS];
extern float average_buffer[BUFFER_SIZE];
extern int buffer_index;
extern size_t data_count;
extern FESParameters pulseOptions;

// Core functionality declarations
int filter_and_calculate_average(const int* sensor_data, size_t data_size);
int calculate_fatigue_level(float normalized_average);
void save_average_to_buffer(float average);
void set_fes_parameters(int fatigue_level);
void process_sensor_data(BMP280_MBED& sensor);

#endif
