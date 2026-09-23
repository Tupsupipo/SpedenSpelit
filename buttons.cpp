#include "buttons.h"
#include "board.h"

volatile byte buttonNumber = -1;  
extern volatile bool DEBUG;
volatile int buttonPinInterrupted = 0;
volatile unsigned long viimeksi = 0;
volatile unsigned long debounceTime = 300;
const int buttonAmount = sizeof(pins)/sizeof(pins[0]);

void initButtonsAndButtonInterrupts(void)
{
  // See requirements for this function from buttons.h
  // MEGA2560 toimii hieman erilailla, kuin UNO. Molempien pitäisi toimia samoilla rekisteriasetuksilla, jos määritellään pinnit molemmille omiksi.

  pinMode(startButtonPin, INPUT_PULLUP);  // Start button pin. 
  if (DEBUG == true){  Serial.print("startButton in PIN : ");Serial.print(startButtonPin);Serial.println(" intialized"); }

  for(int pin=0; pin<buttonAmount; pin++) {         // Määritetään pelinappien pinnit. 
      pinMode(pins[pin] ,INPUT_PULLUP);             // Pinnit board.h:ssa määritellystä taulukosta. Alusta riippuvainen. MEGA2560:lla 50,51,52,53. UNO:lla 2,3,4,5.
      if (DEBUG == true){  Serial.print("Button in PIN : ");Serial.print(pins[pin]);Serial.println(" intialized"); }
  }
  // Haetaan alusta riippuvaiset rekisteriasetukset board.h tiedostosta.
  PCICR = PCICR_SETTING;    //  (1 << PCIE0)
  PCMSK0 = PCMSK0_SETTING;  
  PCMSK1 = PCMSK1_SETTING;

}


ISR(PCINT0_vect) {
  unsigned long nyt = millis();
 // Katsotaan mitä nappia painettiin. Hylätään liian nopea painallus.
  for (int pin=0; pin<buttonAmount; pin++) {
    if (digitalRead(pins[pin]) == LOW) {
      if(nyt - viimeksi < debounceTime) return;  // debounce
        viimeksi = nyt;
        buttonPinInterrupted = pins[pin];
        buttonNumber = pin;
    }
  }
}

ISR(PCINT2_vect) {
   /* Korvattu PCINT0lla, joka pitäisi toimia myös UNOlla.
     Here you implement logic for handling
	 interrupts from 2,3,4,5 pins for Game push buttons
	 and for pin 6 for start Game push button.
   */
}

ISR(PCINT1_vect) {
  unsigned long nyt = millis();
   // Käynnistysnappi UNO:lle. PCINT8 Sijaitsee PCIE1 ryhmässä, joten pitää käyttää PCINT1_vect keskeytystä.
  if (digitalRead(startButtonPin) == LOW) {
    if (DEBUG == true){  Serial.println("Käynnistysnappi painettu");}
      if(nyt - viimeksi < debounceTime) return;  // debounce
        viimeksi = nyt;
        buttonPinInterrupted = startButtonPin;
        buttonNumber = 4;  // Käynnistysnappi on index 4, koska pelinappien indeksit ovat 0-3.
  }
}