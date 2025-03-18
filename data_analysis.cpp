#include "data_analysis.h"
#include "bmp280_mbed.h"
#include <cmath>
#include <array>

// Define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Shared buffers
std::array<float, MAX_DATA_SIZE> filtered_data_buffer;
int sensor_data[DATA_POINTS];
float average_buffer[BUFFER_SIZE];
int buffer_index = 0;
size_t data_count = 0;
FESParameters pulseOptions;

int filter_and_calculate_average(const int* sensor_data, size_t data_size) {
    constexpr float cutoff_frequency = 70.0f;
    constexpr float sampling_rate = 1000.0f;
    constexpr float rc = 1.0f / (2.0f * M_PI * cutoff_frequency);
    constexpr float alpha = 1.0f / (1.0f + rc * sampling_rate);

    if(data_size > MAX_DATA_SIZE) return 0;

    float previous_value = sensor_data[0];
    filtered_data_buffer[0] = previous_value;

    for(size_t i = 1; i < data_size; ++i) {
        filtered_data_buffer[i] = alpha * previous_value + 
                                 (1.0f - alpha) * sensor_data[i];
        previous_value = filtered_data_buffer[i];
    }

    int sum = 0;
    for(size_t i = 0; i < data_size; ++i) {
        sum += filtered_data_buffer[i];
    }
    return sum / data_size;
}

int calculate_fatigue_level(float normalized_average) {
    if (normalized_average >= 0.9f) return 0;
    if (normalized_average >= 0.8f) return 1;
    if (normalized_average >= 0.7f) return 2;
    if (normalized_average >= 0.6f) return 4;
    if (normalized_average >= 0.5f) return 8;
    return 10;
}

void save_average_to_buffer(float average) {
    average_buffer[buffer_index] = average;
    buffer_index = (buffer_index + 1) % BUFFER_SIZE;
}

void set_fes_parameters(int fatigue_level) {
    switch (fatigue_level) {
        case 0:  pulseOptions = {10, 100, 25}; break;
        case 1:  pulseOptions = {10, 120, 25}; break;
        case 2:  pulseOptions = {10, 140, 25}; break;
        case 4:  pulseOptions = {10, 160, 25}; break;
        case 8:  pulseOptions = {10, 180, 25}; break;
        case 10: pulseOptions = {10, 200, 25}; break;
        default: pulseOptions = {0, 0, 0}; break;
    }
    
    printf("FES Params: %dmA, %dus, %dHz\r\n",
           pulseOptions.amplitude,
           pulseOptions.width,
           pulseOptions.frequency);
}

void process_sensor_data(BMP280_MBED& sensor) {
    if (data_count < DATA_POINTS) {
        int16_t pressureArray[2];  // Array to store pressure data
        sensor.readPressure(pressureArray);  // Call the modified function

        // Reconstruct the pressure value from two int16_t values
        int32_t pressure = (static_cast<int32_t>(pressureArray[0]) << 16) | 
                            static_cast<uint16_t>(pressureArray[1]);

        printf("Pressure: %d Pa\r\n", pressure);
        
        sensor_data[data_count] = pressure;
        data_count++;
        ThisThread::sleep_for(10);
    } else {
        int average = filter_and_calculate_average(sensor_data, DATA_POINTS);
        printf("Filtered Average: %d\r\n", average);
        
        save_average_to_buffer(static_cast<float>(average));
        if (average_buffer[0] != 0) {
            float normalized = static_cast<float>(average) / average_buffer[0];
            int fatigue = calculate_fatigue_level(normalized);
            set_fes_parameters(fatigue);
        }
        data_count = 0;
    }
    ThisThread::sleep_for(10);
}
