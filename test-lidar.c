#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wiringPi.h>
#include "Flightlib/vl53l8cx_driver/Core/Inc/vl53l8cx/vl53l8cx_api.h"

#define LPN 13
#define PWREN 15

void powerON(void);
volatile VL53L8CX_Configuration dev;

int main()
{ 
    VL53L8CX_ResultsData results;

    uint8_t status;
    powerON();

    status = vl53l8cx_init(&dev);
    if(status != VL53L8CX_STATUS_OK)
    {
        fprintf(stderr, "Failed to initialize VL53L8CX sensor\n");
        return -1;
    }
    status = vl53l8cx_set_resolution(&dev, VL53L8CX_RESOLUTION_8X8);
    if(status != VL53L8CX_STATUS_OK)
    {
        fprintf(stderr, "Failed to set resolution\n");
        return -1;
    }
    status = vl53l8cx_set_ranging_frequency_hz(&dev, 10);
    if(status != VL53L8CX_STATUS_OK)
    {
        fprintf(stderr, "Failed to set ranging frequency\n");
        return -1;
    }
    status = vl53l8cx_set_integration_time_ms(&dev, 20);
    if(status != VL53L8CX_STATUS_OK)
    {
        fprintf(stderr, "Failed to set integration time\n");
        return -1;
    }
    int y = 0;
    do
    {
        status = vl53l8cx_get_ranging_data(&dev, &results);
        if (status != VL53L8CX_STATUS_OK){
            fprintf(stderr, "Failed to get ranging data");
            return -1;
        }
        VL53L8CX_WaitMs(&dev, 1000);
        
        for(int i = 0; i < 8; i++)    {
            for(int j = 0; j < 8; j++){
                y = i*8 + j;
                printf("%d: %dmm,%ds,%da,%dr || ",
                y,
                results.distance_mm[y],
                results.signal_per_spad[y],
                results.ambient_per_spad[y],
                results.reflectance[y]);
            }
            printf("\n");
        }
        y = 0;
    }  while(results.distance_mm[0] < 50 && results.distance_mm[0] > 0);
   
    return 0;

}

void powerON(void){
    wiringPiSetupPhys();
    pinMode(LPN, OUTPUT);
    digitalWrite(LPN, LOW);
    pinMode(PWREN, OUTPUT);
    digitalWrite(PWREN, LOW);
    VL53L8CX_WaitMs(&dev, 100);
    digitalWrite(PWREN, HIGH);
    VL53L8CX_WaitMs(&dev, 50);
    digitalWrite(LPN, HIGH);
    VL53L8CX_WaitMs(&dev, 250);
}



