#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#define CHANNEL 0
#define SPEED 3000000 //3MHz
#define MODE 3 //Phase = 1, Polarity = 1


int main()
{
	volatile int setup = 0;
	volatile int connection = 0; 
	unsigned char data[4];
	unsigned short reg = 0x0055;
	unsigned short read = reg | 0x8000;
	unsigned short write = 0xa5a5;
	setup = wiringPiSPISetupMode(CHANNEL, SPEED, MODE);
	if (setup < 0)
	{
		perror("SPI Setup fehlgeschlagen");
		setup = 0;
		return -1;
	}
	
	 
	data[0] = (read >> 8) & 0xFF;
	data[1] = read & 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	connection = wiringPiSPIDataRW(CHANNEL, data, 4);
	if (connection < 0)
	{
		perror("SPI Connection fehlgeschlagen");
		connection = 0;
		return -1;
	}
	printf("Empfangene Daten: Daten 2: 0x%02x \nDaten 3: 0x%02x\n", data[2], data[3]);
	sleep(5);
	
	data[0] = (write >> 8) & 0xFF;
	data[1] = write & 0xFF;
	data[2] = 0x1C; //gesendete Daten
	data[3] = 0x00;
	connection = wiringPiSPIDataRW(CHANNEL, data, 4);
	if (connection < 0)
	{
		perror("SPI Connection fehlgeschlagen");
		return -1;
	}
	
	
	printf("Empfangene Daten: Daten 2: 0x%02x \nDaten 3: 0x%02x\n", data[2], data[3]);
	
	return 0;
}



