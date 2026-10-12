#include "display.h"                // Merkinnät
                                    // |Datasheet|Simu|
const byte serialInputPin = 2;      // |SER      |DS  | Serial kommunikointipinni
const byte latchClockPin = 3;       // |STCP     |RCK |
const byte shiftClockPin = 4;       // |SHCP     |SCK |


// Minkä segmenttien halutaan palavan milläkin numerolla. Common cathode toteutus.
byte numbers[10] = {
  B00111111, // 0
  B00000110, // 1
  B01011011, // 2
  B01001111, // 3
  B01100110, // 4
  B01101101, // 5
  B01111101, // 6
  B00000111, // 7
  B01111111, // 8
  B01101111  // 9
};

void initializeDisplay(void)
{
    pinMode(serialInputPin, OUTPUT);
    pinMode(latchClockPin, OUTPUT); 
    pinMode(shiftClockPin, OUTPUT);  
    showResult(0);  // Näytetään 00 pelin alussa
}


void writeByte(uint8_t bits,bool last)
{
    digitalWrite(latchClockPin, LOW);           // Salpa alas ennen lähetystä

    for (int8_t i = 7; i >= 0; i--) {           // lähetetään tavu bitti kerrallaan
        digitalWrite(shiftClockPin, LOW);

        if (bits&(1<<i)) {                      // Tarkistetaan onko bitti 1 vai 0. 1==HIGH, 0==LOW
            digitalWrite(serialInputPin, HIGH);
        }    
        else {
            digitalWrite(serialInputPin, LOW);  // Varmistetaan, että bitti jää low tilaan
        }
       digitalWrite(shiftClockPin, HIGH); 
    }

    if (last) {         // Viimeiset tavun jälkeen päivitetään näytöt \/
        digitalWrite(latchClockPin, HIGH);      // Salpa ylös
    }

    digitalWrite(latchClockPin, LOW);           // Salpa alas 
}


void writeHighAndLowNumber(uint8_t tens,uint8_t ones)   // Invertoin bitit tässä kohtaa, koska vaihdoin common anode näyttöön
{
    writeByte(~numbers[ones], false);
    writeByte(~numbers[tens], true);
}

void showResult(byte number)
{
    byte tens = number / 10;            // jaetaan kymmenellä pyöristyy kymmeniksi
    byte ones = number % 10;            // jakojäännöksellä ykköset
    writeHighAndLowNumber(tens,ones);
}

void clearDisplay()                     // sammutetaan näyttö lepotilaa varten.
{
    writeByte(0xFF, false);
    writeByte(0xFF, true);
}

