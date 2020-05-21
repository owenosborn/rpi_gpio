
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringShift.h>

#define LEDG 22          
#define LEDR 23       
#define LEDB 24         


int main(void) {
 
    // setup GPIO, this uses actual BCM pin numbers 
    wiringPiSetupGpio();
    // GPIO for LEDs
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);


    printf("did it\n");

}
