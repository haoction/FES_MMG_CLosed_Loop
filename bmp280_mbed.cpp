#include "bmp280_mbed.h"

BMP280_MBED::BMP280_MBED(I2C* i2c, uint8_t address) 
    : _i2c(i2c), _address(address) {}

bool BMP280_MBED::begin() {
    // Read the chip ID register
    char cmd[1] = {BMP280_REGISTER_CHIPID};
    char chipid;

    // Write the register address
    if (_i2c->write(_address << 1, cmd, 1) != 0) {
        printf("Failed to write to BMP280!\r\n");
        return false;
    }

    // Read the chip ID
    if (_i2c->read(_address << 1, &chipid, 1) != 0) {
        printf("Failed to read from BMP280!\r\n");
        return false;
    }

    // Debug output
    printf("Chip ID: 0x%02X\r\n", chipid);

    // Check if the chip ID matches
    if (chipid != BMP280_CHIPID) {
        printf("Invalid chip ID! Expected 0x58, got 0x%02X\r\n", chipid);
        return false;
    }

    // Read calibration coefficients
    readCoefficients();

    // Configure the sensor
    setSampling();

    // Wait for the sensor to initialize
    ThisThread::sleep_for(100);  // Increased delay for initialization

    return true;
}

void BMP280_MBED::setSampling(sensor_mode mode,
                             sensor_sampling tempSampling,
                             sensor_sampling pressSampling,
                             sensor_filter filter,
                             standby_duration duration) {
    uint8_t control = (tempSampling << 5) | (pressSampling << 2) | mode;
    uint8_t config = (duration << 5) | (filter << 2);
    
    write8(BMP280_REGISTER_CONTROL, control);
    write8(BMP280_REGISTER_CONFIG, config);
}

float BMP280_MBED::readTemperature() {
    int32_t var1, var2;
    int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
    adc_T >>= 4;

    var1 = ((((adc_T >> 3) - ((int32_t)_calib.dig_T1 << 1))) * 
           ((int32_t)_calib.dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)_calib.dig_T1)) * 
            ((adc_T >> 4) - ((int32_t)_calib.dig_T1))) >> 12) * 
            ((int32_t)_calib.dig_T3)) >> 14;

    _t_fine = var1 + var2;
    return ((_t_fine * 5 + 128) >> 8) / 100.0;
}

float BMP280_MBED::readPressure() {
    int64_t var1, var2, p;
    readTemperature(); // Must be called first
    
    int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
    adc_P >>= 4;

    var1 = ((int64_t)_t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)_calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)_calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)_calib.dig_P3) >> 8) +
           ((var1 * (int64_t)_calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_calib.dig_P1) >> 33;

    if (var1 == 0) return 0;
    
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)_calib.dig_P8) * p) >> 19;
    
    p = ((p + var1 + var2) >> 8) + (((int64_t)_calib.dig_P7) << 4);
    return (float)p / 256;
}

void BMP280_MBED::write8(uint8_t reg, uint8_t value) {
    char cmd[2] = {reg, value};
    _i2c->write(_address << 1, cmd, 2);
}

uint8_t BMP280_MBED::read8(uint8_t reg) {
    char cmd[1] = {reg};
    char value[1];
    _i2c->write(_address << 1, cmd, 1);
    _i2c->read(_address << 1, value, 1);
    return value[0];
}

uint16_t BMP280_MBED::read16(uint8_t reg) {
    char cmd[1] = {reg};
    char value[2];
    _i2c->write(_address << 1, cmd, 1);
    _i2c->read(_address << 1, value, 2);
    return (value[0] << 8) | value[1];
}

uint16_t BMP280_MBED::read16_LE(uint8_t reg) {
    uint16_t temp = read16(reg);
    return (temp >> 8) | (temp << 8);
}

int16_t BMP280_MBED::readS16(uint8_t reg) {
    return (int16_t)read16(reg);
}

int16_t BMP280_MBED::readS16_LE(uint8_t reg) {
    return (int16_t)read16_LE(reg);
}

uint32_t BMP280_MBED::read24(uint8_t reg) {
    char cmd[1] = {reg};
    char value[3];
    _i2c->write(_address << 1, cmd, 1);
    _i2c->read(_address << 1, value, 3);
    return (value[0] << 16) | (value[1] << 8) | value[2];
}

void BMP280_MBED::readCoefficients() {
    _calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
    _calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
    _calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

    _calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
    _calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
    _calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
    _calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
    _calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
    _calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
    _calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
    _calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
    _calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);

    // Debug output
    printf("Calibration Data:\r\n");
    printf("dig_T1: %u\r\n", _calib.dig_T1);
    printf("dig_T2: %d\r\n", _calib.dig_T2);
    printf("dig_T3: %d\r\n", _calib.dig_T3);
    printf("dig_P1: %u\r\n", _calib.dig_P1);
    printf("dig_P2: %d\r\n", _calib.dig_P2);
    printf("dig_P3: %d\r\n", _calib.dig_P3);
    printf("dig_P4: %d\r\n", _calib.dig_P4);
    printf("dig_P5: %d\r\n", _calib.dig_P5);
    printf("dig_P6: %d\r\n", _calib.dig_P6);
    printf("dig_P7: %d\r\n", _calib.dig_P7);
    printf("dig_P8: %d\r\n", _calib.dig_P8);
    printf("dig_P9: %d\r\n", _calib.dig_P9);
}

void BMP280_MBED::reset() {
    write8(BMP280_REGISTER_SOFTRESET, 0xB6);
}