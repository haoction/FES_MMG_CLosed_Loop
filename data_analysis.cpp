#include "data_analysis.h"
#include <cmath>
#include <array>
// Define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// Use a fixed maximum size to avoid VLAs
constexpr size_t MAX_DATA_SIZE = 1000;
std::array<float, MAX_DATA_SIZE> filtered_data_buffer;





int filter_and_calculate_average(const int* sensor_data, size_t data_size) {
    constexpr float cutoff_frequency = 70.0f;
    constexpr float sampling_rate = 1000.0f;
    constexpr float rc = 1.0f / (2.0f * M_PI * cutoff_frequency);
    constexpr float alpha = 1.0f / (1.0f + rc * sampling_rate); // Corrected filter coefficient

    if(data_size > MAX_DATA_SIZE) return NAN;

    float previous_value = sensor_data[0];
    filtered_data_buffer[0] = previous_value;

    // Apply IIR low-pass filter
    for(size_t i = 1; i < data_size; ++i) {
        filtered_data_buffer[i] = alpha * previous_value + 
                                 (1.0f - alpha) * sensor_data[i];
        previous_value = filtered_data_buffer[i];
    }

    // Calculate average
    int sum = 0;
    for(size_t i = 0; i < data_size; ++i) {
        sum += filtered_data_buffer[i];
    }
    return sum / data_size;
}

int calculate_fatigue_level(int average) {
    // Corrected range checks
    if (average >= 9) {
        return 0;
    } else if (average >= 8) {
        return 1;
    } else if (average >= 7) {
        return 2;
    } else if (average >= 6) {
        return 3;
    } else if (average >= 5) {
        return 4;
    } else if (average >= 4) {
        return 5;
    } else {
        return 6; // Error case
    }
}
