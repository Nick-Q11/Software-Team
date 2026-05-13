#ifndef VL53L8CX_PLATFORM_H_
#define VL53L8CX_PLATFORM_H_


#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#ifndef VL53L8CX_NB_TARGET_PER_ZONE
#define VL53L8CX_NB_TARGET_PER_ZONE    1U
#endif
#define CHANNEL 0
#define SPEED 3000000 //3 MHz
#define MODE 3 //SPI_MODE3: CPOL=1, CPHA=1

typedef struct
{
    //I2C_HandleTypeDef *hi2c;
    uint8_t address;
} VL53L8CX_Platform;


// Functions called by VL53L8CX API

int32_t VL53L8CX_WrByte(VL53L8CX_Platform *p, uint16_t reg, uint8_t data);
int32_t VL53L8CX_RdByte(VL53L8CX_Platform *p, uint16_t reg, uint8_t *data);
int32_t OwnSPI_RdWrByte(VL53L8CX_Platform *p, uint16_t reg, uint8_t *data, uint32_t len, uint8_t read);
int32_t VL53L8CX_WrMulti(VL53L8CX_Platform *p, uint16_t reg, uint8_t *pdata, uint32_t count);
int32_t VL53L8CX_RdMulti(VL53L8CX_Platform *p, uint16_t reg, uint8_t *pdata, uint32_t count);
int32_t OwnSPI_RdWrMulti(VL53L8CX_Platform *p, uint16_t reg, uint8_t *data, uint32_t len, uint8_t read);
void VL53L8CX_SwapBuffer(uint8_t *pbuffer, int size);
int32_t VL53L8CX_WaitMs(VL53L8CX_Platform *p, int32_t wait_ms);

#endif