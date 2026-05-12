#include <stdio.h>
#include <stdlib.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <unistd.h>

#define channel 0
#define speed 5000000   //5MHz

if (wiringPiSetup(channel, speed) < 0){
    printf("SPI setup Fehler!\n");
}

char buffer[2];
buffer[0] = 0x00;
buffer[1] = 0x00;

//Registeradresse = buffer[0]
//Daten = buffer[1]

int check = wiringPiSPIDataRW(channel, buffer, 2);

if (check == -1){
    printf("SPI Communication Fehler!\n");
}

