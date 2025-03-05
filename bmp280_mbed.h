#ifndef BMP280_MBED_H
#define BMP280_MBED_H

#include "mbed.h"

#define BMP280_ADDRESS       0x76 //The default I2C address for the sensor.
#define BMP280_CHIPID        0x58 //Default chip ID

// Registers available on the sensor.
enum {
    BMP280_REGISTER_DIG_T1       = 0x88,
    BMP280_REGISTER_DIG_T2       = 0x8A,
    BMP280_REGISTER_DIG_T3       = 0x8C,
    BMP280_REGISTER_DIG_P1       = 0x8E,
    BMP280_REGISTER_DIG_P2       = 0x90,
    BMP280_REGISTER_DIG_P3       = 0x92,
    BMP280_REGISTER_DIG_P4       = 0x94,
    BMP280_REGISTER_DIG_P5       = 0x96,
    BMP280_REGISTER_DIG_P6       = 0x98,
    BMP280_REGISTER_DIG_P7       = 0x9A,
    BMP280_REGISTER_DIG_P8       = 0x9C,
    BMP280_REGISTER_DIG_P9       = 0x9E,
    BMP280_REGISTER_CHIPID       = 0xD0,
    BMP280_REGISTER_VERSION      = 0xD1,
    BMP280_REGISTER_SOFTRESET    = 0xE0,
    BMP280_REGISTER_STATUS       = 0xF3,
    BMP280_REGISTER_CONTROL      = 0xF4,
    BMP280_REGISTER_CONFIG       = 0xF5,
    BMP280_REGISTER_PRESSUREDATA = 0xF7,
    BMP280_REGISTER_TEMPDATA     = 0xFA
};


//Driver for the Adafruit BMP280 barometric pressure sensor.
class BMP280_MBED {
public:
//Oversampling rate for the sensor.
    enum sensor_sampling {
        SAMPLING_NONE = 0,
        SAMPLING_X1   = 1,
        SAMPLING_X2   = 2,
        SAMPLING_X4   = 3,
        SAMPLING_X8   = 4,
        SAMPLING_X16  = 5
    };
//Operating mode for the sensor.
    enum sensor_mode {
        MODE_SLEEP  = 0,
        MODE_FORCED = 1,
        MODE_NORMAL = 3
    };
//Filtering level for sensor data.
    enum sensor_filter {
        FILTER_OFF = 0,
        FILTER_X2  = 1,
        FILTER_X4  = 2,
        FILTER_X8  = 3,
        FILTER_X16 = 4
    };
//Standby duration in ms
    enum standby_duration {
        STANDBY_MS_1   = 0,
        STANDBY_MS_63  = 1,
        STANDBY_MS_125 = 2,
        STANDBY_MS_250 = 3,
        STANDBY_MS_500 = 4,
        STANDBY_MS_1000= 5,
        STANDBY_MS_2000= 6,
        STANDBY_MS_4000= 7
    };

    BMP280_MBED(I2C* i2c, uint8_t address = BMP280_ADDRESS);
    bool begin();
    float readTemperature();
    float readPressure();
    void setSampling(sensor_mode mode = MODE_NORMAL,
                    sensor_sampling tempSampling = SAMPLING_X16,
                    sensor_sampling pressSampling = SAMPLING_X16,
                    sensor_filter filter = FILTER_OFF,
                    standby_duration duration = STANDBY_MS_1);

//Struct to hold calibration data.
private:
    struct calibration_data {
        uint16_t dig_T1;
        int16_t dig_T2;
        int16_t dig_T3;
        uint16_t dig_P1;
        int16_t dig_P2;
        int16_t dig_P3;
        int16_t dig_P4;
        int16_t dig_P5;
        int16_t dig_P6;
        int16_t dig_P7;
        int16_t dig_P8;
        int16_t dig_P9;
    };

    I2C* _i2c;
    uint8_t _address;
    calibration_data _calib;
    int32_t _t_fine;

    void write8(uint8_t reg, uint8_t value);
    uint8_t read8(uint8_t reg);
    uint16_t read16(uint8_t reg);
    uint16_t read16_LE(uint8_t reg);
    int16_t readS16(uint8_t reg);
    int16_t readS16_LE(uint8_t reg);
    uint32_t read24(uint8_t reg);
    void readCoefficients();
    void reset();
};

#endif