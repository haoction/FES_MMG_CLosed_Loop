#include "mbed.h"
#include "bmp280_mbed.h"  // Use the new header
#include "data_analysis.h"
#include <cstdint>

// Define I2C pins - change according to your board
I2C i2c(PB_9, PB_8);  // Use actual I2C pins
BMP280_MBED bmp(&i2c); // Use the Mbed-ported class

// Data structures and buffers
#define DATA_POINTS 100
#define BUFFER_SIZE 100

int sensor_data[DATA_POINTS]; // Change it int
float average_buffer[BUFFER_SIZE];
int buffer_index = 0;
size_t data_count = 0;
int received_fatigue_level = 0;

struct FESParameters {
    int amplitude;
    int width;
    int frequency;
};

FESParameters pulseOptions;
Serial pc(USBTX, USBRX);  

// Function prototypes
void save_average_to_buffer(float average);
void setFESParameters(int fatigueLevel);
void process_sensor_data();

int main() {
    pc.baud(9600);  // Set baud rate using legacy method

    printf("BMP280 Sensor Test\n");
    
    if (!bmp.begin()) {  // Address is set in constructor
        printf("BMP280 not found!\n");
        return 1;
    }
    
    // Configure sensor settings using Mbed enum values
    bmp.setSampling(BMP280_MBED::MODE_NORMAL,
                    BMP280_MBED::SAMPLING_X2,
                    BMP280_MBED::SAMPLING_X16,
                    BMP280_MBED::FILTER_OFF,
                    BMP280_MBED::STANDBY_MS_1);

    // Wait for the sensor to stabilize
    ThisThread::sleep_for(200);

    while(1) {
        process_sensor_data(); // Replaced loop code with function call
    }
}



void process_sensor_data() {
    if (data_count < DATA_POINTS) {
        int pressure = bmp.readPressure() * 100;
        printf("Pressure: %d Pa\r\n", pressure);
        sensor_data[data_count] = pressure;
        data_count++;
        ThisThread::sleep_for(10);
    } else {
        int average = filter_and_calculate_average(sensor_data, DATA_POINTS);
        printf("Average: %d\r\n", average);
        
        save_average_to_buffer(average);
        if (average_buffer[0] != 0) {  // Prevent division by zero
            int normalized = average / average_buffer[0];
            int fatigue = calculate_fatigue_level(normalized);
            setFESParameters(fatigue);
        }
        data_count = 0;
    }
    ThisThread::sleep_for(10);
}

// Add these function definitions AFTER process_sensor_data()
// (or move them before process_sensor_data() and add prototypes)

void save_average_to_buffer(float average) {
    average_buffer[buffer_index] = average;
    buffer_index = (buffer_index + 1) % BUFFER_SIZE;
}

void setFESParameters(int fatigueLevel) {
    switch (fatigueLevel) {
        case 0:  pulseOptions = {10, 100, 20}; break;
        case 1:  pulseOptions = {15, 120, 25}; break;
        case 2:  pulseOptions = {20, 140, 30}; break;
        case 4:  pulseOptions = {30, 160, 35}; break;
        case 8:  pulseOptions = {40, 180, 40}; break;
        case 10: pulseOptions = {50, 200, 50}; break;
        default: pulseOptions = {0, 0, 0}; break;
    }
    
    printf("FES Params: %dmA, %dus, %dHz\r\n",
           pulseOptions.amplitude,
           pulseOptions.width,
           pulseOptions.frequency);
}

// Encapsulated sensor processing logic


// Existing buffer management and FES functions below...
// (Keep your existing save_average_to_buffer() and setFESParameters() implementations here)


        // Data process ifloat and non fucntionlized
        // if (data_count < DATA_POINTS) {
        //     float pressure = bmp.readPressure();
        //     printf("Pressure: %.2f Pa\r\n", pressure);  // Add \r for serial consistency
        //     sensor_data[data_count] = pressure;
        //     data_count++;
        //     ThisThread::sleep_for(10);
        // } else {
        //     float average = filter_and_calculate_average(sensor_data, DATA_POINTS);
        //     printf("Average: %.2f\r\n", average);
            
        //     save_average_to_buffer(average);
        //     if (average_buffer[0] != 0) {  // Prevent division by zero
        //         float normalized = average / average_buffer[0];
        //         int fatigue = calculate_fatigue_level(normalized);
        //         setFESParameters(fatigue);
        //     }
        //     data_count = 0;
        // }
        
        // ThisThread::sleep_for(10);



// #include "mbed.h"

// // Define I2C pins (PB_9 for SDA, PB_8 for SCL)
// I2C i2c(PB_9, PB_8);

// // Serial connection for debugging
// Serial pc(USBTX, USBRX);

// int main() {
//     pc.baud(9600);
//     pc.printf("Starting I2C scanner...\r\n");

//     char data[1];  // Buffer for reading
//     int found = 0;

//     for (int address = 0x08; address <= 0x77; address++) {
//         int result = i2c.write(address << 1, data, 1);
//         if (result == 0) {  // If device acknowledges
//             pc.printf("Device found at address: 0x%02X\r\n", address);
//             found++;
//         }
//         ThisThread::sleep_for(10);  // Small delay between scans
//     }

//     if (found == 0) {
//         pc.printf("No I2C devices found!\r\n");
//     } else {
//         pc.printf("Scan complete. Found %d devices.\r\n", found);
//     }

//     while (1) {
//         ThisThread::sleep_for(1);
//     }
// }