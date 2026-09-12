#include "buttons.h"
#include "board.h"

extern volatile bool DEBUG;
volatile int buttonPinInterrupted = 0;
volatile unsigned long viimeksi = 0;
volatile unsigned long debounceTime = 300;
const int buttonAmount = sizeof(pins)/sizeof(pins[0]);

void initButtonsAndButtonInterrupts(void)
{
  // See requirements for this function from buttons.h
  // MEGA2560 toimii hieman erilailla, kuin UNO. Molempien pitäisi toimia samoilla !=rekisteriasetuksilla, jos määritellään pinnit molemmille omiksi. (Ilmeisesti ei)
  // Ehkä jonkinlainen define systeemi kumpi käytössä(done, automatic). Pitää miettiä. Startup nappi voidaan varmaan tehdä external keskeytyksenä, jonka voi toteuttaa 
  // "attachInterrupt(digitalPinToInterrupt(pin), ISR, mode)" funktiolla, joka hoitaa määrittelyn itse. Silloin ei ole väliä alustalla ja murhe jää vain pelinappuloille.


  for(int pin=0; pin<buttonAmount; pin++) {         // Megassa ajattelin mennä näillä. Lisänä varmaan vielä erikseen määriteltävä käynnistynappi. Ei 5 pinniä lähekkäin..
      pinMode(pins[pin] ,INPUT_PULLUP);             // Arduino pinnit 50-53 INPUT_PULLUP tilaan
      if (DEBUG == true){  Serial.print("Button in PIN : ");Serial.print(pins[pin]);Serial.println(" intialized"); }
  }
  // taitaa kuitenkin olla eri rekisterit, kuin UNOlla. Korvataan nämä muuttujalla, joka määritellään board.h tiedostossa laudan mukaan kuten napitkin. TODO selaa uudestaan datalehtiä.. 
  PCICR = B00000001;    // PCIE0
  PCMSK0 = B00001111;   // PCINT0-PCINT3 == pinnit 50-53 

}



ISR(PCINT0_vect) {
  unsigned long nyt = millis();
 // Megassa hieman ärsyttävästi hajallaan pinnit.. Tehdään oma käynnistysnapille, vain nämä vierekkäin tässä ryhmässä. Käynnistysnappi voidaan toteuttaa myös external keskeytyksenä, jolloin se ei ole riippuvainen alustasta. 
  for (int pin=0; pin<buttonAmount; pin++) {
    //int pin = pins[pin];
    if (digitalRead(pins[pin]) == LOW) {
      if(nyt - viimeksi < debounceTime) return;  // debounce
        viimeksi = nyt;
        buttonPinInterrupted = pins[pin];
    }
  }
}

ISR(PCINT2_vect) {
   /* Korvattu PCINT0lla, joka pitäisi toimia myös UNOlla. _NOT_
     Here you implement logic for handling
	 interrupts from 2,3,4,5 pins for Game push buttons
	 and for pin 6 for start Game push button.
   */
}