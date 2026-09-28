#include "display.h"                // Merkinnät
                                    // |Datasheet|Simu|
const byte serialInputPin = 2;       // |SER      |DS  | Serial kommunikointipinni
const byte latchClockPin = 3;        // |STCP     |RCK |
const byte shiftClockPin = 4;        // |SHCP     |SCK |
const byte resetPin = 5;             // |SCLR'    |MR  | Reset. Voisiko myös pitää suoraan +5V? Taitaa olla turha tässä kohtaa? Lopussa lopputulos, alussa uudet numerot
// outEnable tönäsin suoraan maihin, kun taitaa olla turha. Haittaako??

void initializeDisplay(void)
{
    pinMode(serialInputPin, OUTPUT);
    pinMode(latchClockPin, OUTPUT); 
    pinMode(shiftClockPin, OUTPUT);
    pinMode(resetPin, OUTPUT);

    digitalWrite(resetPin, HIGH);
    
// See requirements for this function from display.h
}


void writeByte(uint8_t bits,bool last)
{
// See requirements for this function from display.h

}


void writeHighAndLowNumber(uint8_t tens,uint8_t ones)
{
// See requirements for this function from display.h
}

void showResult(byte number)
{
// See requirements for this function from display.h
}

