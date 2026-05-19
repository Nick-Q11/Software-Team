#include "vl53l8cx_calibrate.h"

void sleep_ms(int ms);
void powerON(void);
int failure(int status, const char* message);
int printInfo(VL53L8CX_calibrate *calib);
int calibrate(VL53L8CX_calibrate *calib);

int calibrate(VL53L8CX_calibrate *calib)
{

    int status;
    int i = 20; // Max wait time of 2 seconds for data ready
    powerON();

    status = vl53l8cx_init(&calib->conf);
    failure(status, "Failed to initialize VL53L8CX sensor");

    status = vl53l8cx_set_resolution(&calib->conf, calib->resolution);
    failure(status, "Failed to set resolution");
    
    status = vl53l8cx_set_ranging_frequency_hz(&calib->conf, calib->ranging_frequency);
    failure(status, "Failed to set ranging frequency");
    
    status = vl53l8cx_set_integration_time_ms(&calib->conf, calib->integration_time);
    failure(status, "Failed to set integration time");

    status = vl53l8cx_start_ranging(&calib->conf);
    failure(status, "Failed to start ranging");

    do{
        sleep_ms(100);
        i--;
    }while((vl53l8cx_check_data_ready(&calib->conf, calib->data_is_ready) == 0 || i < 1));
    if(i < 1){
        failure(-1, "Data not ready after waiting");
    }

    calib->calibrated = 1;
    return 0;
}

int printInfo(VL53L8CX_calibrate *calib)
{
    if(calib->calibrated != 1){
        calibrate(calib);
    }
    
    
    return 0;
}

void powerON(void)
{
    pinMode(LPN, OUTPUT);
    pinMode(PWREN, OUTPUT);
    digitalWrite(LPN, LOW);
    digitalWrite(PWREN, LOW);
    sleep_ms(100);
    digitalWrite(PWREN, HIGH);
    sleep_ms(50);
    digitalWrite(LPN, HIGH);
    sleep_ms(250);
}

int failure(int status, const char* message)
{
    if (status != 0)
    {
        fprintf(stderr, "%s: %s\n", message, strerror(errno));
        return -1;
    }
    return 0;
}

void sleep_ms(int ms)
{
    usleep(ms * 1000); // usleep takes microseconds
}
