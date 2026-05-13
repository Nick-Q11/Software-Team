#include "Flightlib/vl53l8cx_driver/Core/Inc/vl53l8cx/vl53l8cx_platform.h"


int32_t VL53L8CX_Init(VL53L8CX_Platform *p)
{
        if (!bcm2835_init())
        return -1;

    if (!bcm2835_spi_begin())
        return -1;

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);
    bcm2835_spi_setClockDivider(VL53L8CX_SPI_SPEED);
    bcm2835_spi_chipSelect(BCM53L8CX_SPI_CS);
    bcm2835_spi_setChipSelectPolarity(VL53L8CX_SPI_CS, LOW);

    p->defined = 1;
    p->speed = VL53L8CX_SPI_SPEED;

    return 0;
}
// Write one byte
int32_t VL53L8CX_WrByte(VL53L8CX_Platform *p, uint16_t reg, uint8_t data)
{
    return VL53L8CX_WrMulti(p, reg, &data, 1); // Return 0 for success
}

// Read one byte
int32_t VL53L8CX_RdByte(VL53L8CX_Platform *p, uint16_t reg, uint8_t *data)
{
    return VL53L8CX_RdMulti(p, reg, data, 1);
}

// Write multiple bytes
int32_t VL53L8CX_WrMulti(VL53L8CX_Platform *p, uint16_t reg, uint8_t *pdata, uint32_t count)
{
    if(p->defined != 1){
        VL53L8CX_Init(p); // Initialize if not already done
    }

    uint8_t buffer[count + 2];

    if (p->fd < 0){
        fprintf(stderr, "SPI device not initialized\n");
        return -1; // Return -1 for failure
    }

    buffer[0] = (reg >> 8) & 0xFF; // High byte of register address
    buffer[1] = reg & 0xFF;        // Low byte of register

    memcpy(&buffer[2], pdata, count); // Copy data to buffer for write operation
 

    if (wiringPiSPIDataRW(p->channel, buffer, count + 2) < 0) {
        perror("Failed to read/write SPI data");
        return -1; // Return -1 for failure
    }

    return 0;
}

// Read multiple bytes
int32_t VL53L8CX_RdMulti(VL53L8CX_Platform *p, uint16_t reg, uint8_t *pdata, uint32_t count)
{
    if(p->defined != 1){
        VL53L8CX_Init(p); // Initialize if not already done
    }

    if (p->fd < 0){
        fprintf(stderr, "SPI device not initialized\n");
        return -1; // Return -1 for failure
    }

    uint8_t buffer[count + 2];

    buffer[0] = (reg >> 8) & 0xFF; // High byte of register address
    buffer[1] = reg & 0xFF;        // Low byte of register

    memset(&buffer[2], 0x00, count); // Clear buffer for read operation
 

    if(wiringPiSPIDataRW(p->channel, buffer, count + 2) < 0) {
        perror("Failed to read/write SPI data");
        return -1; // Return -1 for failure
    }
        memcpy(pdata, &buffer[2], count); // Copy data to buffer for write operation

    return 0;
}

// Swap buffer (big-endian <-> little-endian, 4 bytes at a time)
void VL53L8CX_SwapBuffer(uint8_t *pbuffer, int size)
{
    for (int i = 0; i < size; i += 4) {
        uint8_t tmp;
        tmp = pbuffer[i];     pbuffer[i]   = pbuffer[i+3]; pbuffer[i+3] = tmp;
        tmp = pbuffer[i+1];   pbuffer[i+1] = pbuffer[i+2]; pbuffer[i+2] = tmp;
    }
}

// Delay in milliseconds
int32_t VL53L8CX_WaitMs(VL53L8CX_Platform *p, int wait_ms)
{
    (void)p;  // unused
    usleep(wait_ms*1000); // usleep takes microseconds
    return 0;
}