#include "vl53l8cx/vl53l8cx_platform.h"

#define MAX_BYTES 4094

int32_t VL53L8CX_InitSPI(VL53L8CX_Platform *p)
{
    p->defined = 1;
    p->speed = 3000000;
    p->channel = 0;
    p->mode = 3;
    p->fd = wiringPiSPISetupMode(p->channel, p->speed, p->mode);
    if (p->fd < 0) {
        fprintf(stderr, "Failed to initialize SPI device: %s\n", strerror(errno));
        return -1; // Return -1 for failure
    }
    

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
	uint32_t count2 = 0;
	uint16_t reg2 = 0;
	uint8_t *pdata2;
    if(p->defined != 1){
        VL53L8CX_InitSPI(p); // Initialize if not already done
    }

    if(count > MAX_BYTES){
    	count2 = (count - MAX_BYTES);
    	reg2 = (reg + MAX_BYTES);
    	pdata2 = (pdata + MAX_BYTES);
    	count = MAX_BYTES;
    }

    if (p->fd < 0){
        fprintf(stderr, "SPI device not initialized\n");
        return -1; // Return -1 for failure
    }
    uint8_t buffer[count + 2];
    buffer[0] = ((reg >> 8) & 0xFF) | (1 << 7); // High byte of register address
    buffer[1] = reg & 0xFF;        				// Low byte of register

    memcpy(&buffer[2], pdata, count); // Copy data to buffer for write operation
 

    if (wiringPiSPIDataRW(p->channel, buffer, count + 2) < 0) {
        perror("Failed to read/write SPI data");
        return -1; // Return -1 for failure
    }
    if(count2 != 0){
    	VL53L8CX_WrMulti(p, reg2, pdata2, count2);
    }
    return 0;
}

// Read multiple bytes
int32_t VL53L8CX_RdMulti(VL53L8CX_Platform *p, uint16_t reg, uint8_t *pdata, uint32_t count)
{
	uint32_t count2 = 0;
	uint16_t reg2 = 0;
	uint8_t *pdata2;

	if(p->defined != 1){
        VL53L8CX_InitSPI(p); // Initialize if not already done
    }

    if(count > MAX_BYTES){
        	count2 = (count - MAX_BYTES);
        	reg2 = (reg + MAX_BYTES);
        	pdata2 = (pdata + MAX_BYTES);
        	count = MAX_BYTES;
        }

    if (p->fd < 0){
        fprintf(stderr, "SPI device not initialized\n");
        return -1; // Return -1 for failure
    }

    uint8_t buffer[count + 2];

    buffer[0] = ((reg >> 8) & 0xFF) & (~(1 << 7)); // High byte of register address
    buffer[1] = reg & 0xFF;        // Low byte of register

    memset(&buffer[2], 0x00, count); // Clear buffer for read operation
 

    if(wiringPiSPIDataRW(p->channel, buffer, count + 2) < 0) {
        perror("Failed to read/write SPI data");
        return -1; // Return -1 for failure
    }
    memcpy(pdata, &buffer[2], count); // Copy data to buffer for write operation

    if(count2 != 0){
    	VL53L8CX_WrMulti(p, reg2, pdata2, count2);
    }

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
