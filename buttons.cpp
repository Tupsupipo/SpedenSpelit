#include "buttons.h"
#include "board.h"

const byte buttonAmount = sizeof(pins)/sizeof(pins[0]);
volatile byte buttonNumber = 255;                         //resetoidaan nappi BYTE viimeiseksi numeroksi. Selvempää logiikan kannalta, kuin int8_t "-1". Ei tarvita ylimääräistä ehtoa looppiin
volatile byte buttonPinInterrupted = 0;
volatile unsigned long viimeksi = 0;
volatile unsigned long debounceTime = 245;                // Isot napit pitkällä jousella. Debounce aika on siitä johtuen näin pitkä. Peli nopeus maksimissaan noin 350ms kieppeilläm, joten ei vielä ongelma


void initButtonsAndButtonInterrupts(void)
{
  // See requirements for this function from buttons.h

  for(byte pin=0; pin<buttonAmount; pin++) {         // Määritetään pelinappien pinnit. 
      pinMode(pins[pin] ,INPUT_PULLUP);             // Pinnit board.h:ssa määritellystä taulukosta. Alusta riippuvainen. MEGA2560:lla 50,51,52,53. UNO:lla 2,3,4,5.
  }
  // Haetaan alusta riippuvaiset rekisteriasetukset board.h tiedostosta.
  PCICR = PCICR_SETTING;   
  PCMSK0 = PCMSK0_SETTING;  
}

void debouncer(byte lastButtonPin, byte pinIndex) {
    unsigned long nyt = millis();

    if(nyt - viimeksi < debounceTime) return;  // debounce
      viimeksi = nyt;
      buttonPinInterrupted = lastButtonPin;
      buttonNumber = pinIndex;
    
}

ISR(PCINT0_vect) {

 // Katsotaan mitä nappia painettiin. Hylätään liian nopea painallus.
  for (byte pin=0; pin<buttonAmount; pin++) {
    if (digitalRead(pins[pin]) == LOW) {
        debouncer(pins[pin], pin);
    }
  }
}
