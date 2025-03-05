#include "mbed.h"
#include "data_analysis.h"
#include "bmp280_mbed.h"

I2C i2c(PB_9, PB_8);
BMP280_MBED bmp(&i2c);
Serial pc(USBTX, USBRX);

int main() {
    pc.baud(9600);
    printf("FES Fatigue Monitoring System\n");
    
    if (!bmp.begin()) {
        printf("Sensor initialization failed!\n");
        return 1;
    }

    bmp.setSampling(BMP280_MBED::MODE_NORMAL,
                    BMP280_MBED::SAMPLING_X2,
                    BMP280_MBED::SAMPLING_X16,
                    BMP280_MBED::FILTER_OFF,
                    BMP280_MBED::STANDBY_MS_1);

    ThisThread::sleep_for(200);

    while(true) {
        process_sensor_data(bmp);
    }
}
