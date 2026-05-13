#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "Flightlib/vl53l8cx_driver/Core/Inc/vl53l8cx/vl53l8cx_api.h"

int main()
{
    VL53L8CX_Configuration dev;
    uint8_t status, resolution;

    /* Initialize the sensor */
    status = vl53l8cx_init(&dev);
    if (status != 0) {
        fprintf(stderr, "Failed to initialize VL53L8CX sensor\n");
        return -1;
    }
}



