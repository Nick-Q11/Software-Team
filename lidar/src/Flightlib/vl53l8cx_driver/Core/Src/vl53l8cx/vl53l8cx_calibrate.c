#include "vl53l8cx_calibrate.h"


#define _8x8 VL53L8CX_RESOLUTION_8X8
#define _4x4 VL53L8CX_RESOLUTION_4X4
#define CONTINUOUS VL53L8CX_RANGING_MODE_CONTINUOUS
#define AUTONOMOUS VL53L8CX_RANGING_MODE_AUTONOMOUS
#define CLOSEST VL53L8CX_TARGET_ORDER_CLOSEST
#define STRONGEST VL53L8CX_TARGET_ORDER_STRONGEST
#define FREQUENCE 10
#define INTEGRATION_TIME 20


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

    calib->resolution = _8x8;
    calib->ranging_frequency = FREQUENCE;
    calib->integration_time = INTEGRATION_TIME;
    calib->data_is_ready = (uint8_t*)malloc(sizeof(uint8_t));

    status = vl53l8cx_init(&calib->conf);
    failure(status, "Failed to initialize VL53L8CX sensor");

    status = vl53l8cx_set_resolution(&calib->conf, calib->resolution);
    failure(status, "Failed to set resolution");
    
    status = vl53l8cx_set_ranging_frequency_hz(&calib->conf, calib->ranging_frequency);
    failure(status, "Failed to set ranging frequency");
    
    status = vl53l8cx_set_integration_time_ms(&calib->conf, calib->integration_time);
    failure(status, "Failed to set integration time");

    status = vl53l8cx_set_ranging_mode(&calib->conf, CONTINUOUS);
    failure(status, "Failed to set ranging mode");

    status = vl53l8cx_set_target_order(&calib->conf, STRONGEST);
    failure(status, "Failed to set target order");

    status = vl53l8cx_start_ranging(&calib->conf);
    failure(status, "Failed to start ranging");

    do{
        sleep_ms(100);
        status = vl53l8cx_check_data_ready(&calib->conf, calib->data_is_ready);
        i--;
    }while(status == 0 || i < 1);
    if(i < 1){
        failure(status, "Data not ready after waiting");
    }

    calib->calibrated = 1;
    return 0;
}

int printInfoSingle(VL53L8CX_calibrate *calib)
{
    int status = 0;
    if(calib->calibrated != 1){
        calibrate(calib);
    }
    int y = 0;
    status = vl53l8cx_get_ranging_data(&calib->conf, &calib->results);
    failure(status, "Failed to get ranging data");
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            y = i*8 + j;
            printf("%d:%d mm, %d s, %d t\t|",
            y,
            calib->results.distance_mm[y],
            calib->results.signal_per_spad[y],
            calib->results.target_status[y]);
        }
        printf("\n");
    }
    return 0;
}

int printInfoMultiple(VL53L8CX_calibrate *calib, int times)
{
    for(int i = 0; i < times; i++){
        printInfoSingle(calib);
        printf("\n");
        sleep_ms(1000);
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
