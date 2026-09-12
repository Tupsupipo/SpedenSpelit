#include "display.h"
#include "board.h"
#include "buttons.h"
#include "leds.h"
#include "SpedenSpelit.h"

// Use these 2 volatile variables for communicating between
// loop() function and interrupt handlers
volatile int buttonNumber = -1;               // for buttons interrupt handler
volatile bool newTimerInterrupt = false;      // for timer interrupt handler muutetaan kun tulee timer keskeytys
bool DEBUG = true;                            // Debug muuttuja. Helpottanee asioiden tutkimista ja ylimääräisen roskan siivoamista lopuksi
extern volatile int buttonPinInterrupted;             // Pinni joka muutti tilaansa
volatile static int interruptCount = 0;       // Lasketaan keskeytyksiä
volatile static int interruptCountTotal = 0;  // Lasketaan keskeytyksien kokonaismäärä. 99 kipattava? vaiko pisteet alas ja uutta sataa lasiin?


void setup()
{
  /*
    Initialize here all modules
  */
    Serial.begin(9600);
    // Näitä varmaan siirrellää sopivampiin paikkoihin. Testiä
    Serial.print("Alustaksi maaritetty: ");
    Serial.println(BOARD_NAME);
    initButtonsAndButtonInterrupts();
    startTheGame();

}

void loop()
{
  if(buttonNumber>=0)
  {
     // start the game if buttonNumber == 4
     // check the game if 0<=buttonNumber<4
  }

  if(newTimerInterrupt == true)
  {
    interruptHandler();

    // new random number must be generated
    randomSeed(analogRead(A0));               // käytetään analogRead(A0) randomin siemenlukuna. Tyhjän analogisen pinni A0 lukeminen antaa satunnaisen arvon, joka on hyvä siemenluku random() funktiolle.
    int randomized = random(4);               // random() funktio palauttaa satunnaisen luvun 0,1,2 tai 3. Tämä luku on seuraava syttyvä LED ja nappi jota pelaajan pitäisi painaa.
    if (DEBUG == true){  Serial.print("Random number: ");Serial.println(randomized);}
    
    // and corresponding let must be activated
    setLed(randomized);                       // sytytetään satunnainen LED. Pelaajan pitäisi painaa vastaavaa nappia.
    if (DEBUG == true){  Serial.print("Sytytetään LED: ");Serial.println(randomized);}

  }
    if (buttonPinInterrupted !=0) {
      Serial.print("PCINT0 vaylasta keskeytys pinnista HIGH to LOW = ");
      Serial.println(buttonPinInterrupted);
      buttonPinInterrupted = 0;
  }


}

void initializeTimer(void)
{
	// see requirements for the function from SpedenSpelit.h
  cli();                      // Sammutetaan keskeytykset ongelmien välttämiseksi. Rekisteri voi korruptoitua, jos keskeytys sattuisi asetuksia vaihtaessa.
  TCCR1A = 0;                 // Nollataan rekisteri, jos vanhoja arvoja tallessa
  TCCR1B = 0;                 // Nollataan rekisteri, jos vanhoja arvoja tallessa
  TCCR1B |= (1 << WGM12);     // Timer nollaantuu automaattisesti ORC1A arvossa. Clear Timer on Compare Match
  TCCR1B |= (1 << CS12);      // Prescaler 256. 16 000 000 / 256 = 62 500
  OCR1A = 62500;              // laskuri laskee 1 sekunnin aikana 62500, joka tuottaa keskeytyksen yllä olevien asetusten johdosta 
  TIMSK1 |= (1 << OCIE1A);    // Keskeytys tulee ORC1A:lta
  sei();                      // Keskeytykset takaisin päälle
  if (DEBUG == true){  Serial.println("Timer initialized");}

}
ISR(TIMER1_COMPA_vect)
{
  /*
  Communicate to loop() that it's time to make new random number.
  Increase timer interrupt rate after 10 interrupts.
  */
  newTimerInterrupt = true;   // ilmoitetaan loopille ja interruptHandelrille uudesta keskeytyksestä

}


void checkGame(byte nbrOfButtonPush)
{
	// see requirements for the function from SpedenSpelit.h
}


void initializeGame()
{
	// see requirements for the function from SpedenSpelit.h
    if (DEBUG == true){  Serial.println("initializeGame() initialized");}

  
}

void startTheGame()
{
   // see requirements for the function from SpedenSpelit.h
    if (DEBUG == true){  Serial.println("startTheGame() initialized");}
    initializeTimer();
    initializeGame();

}

void stopTheGame()
{

}

void interruptHandler() {
    //if (DEBUG == true){  Serial.print("newTimerInterrupt = ");Serial.println(newTimerInterrupt);}  
    newTimerInterrupt = false;
    //if (DEBUG == true){  Serial.print("newTimerInterrupt = ");Serial.println(newTimerInterrupt);}  
    interruptCount++;
    interruptCountTotal++;

    if (DEBUG == true){
      Serial.print("Keskeytys: ");
      Serial.println(interruptCount);
      Serial.print("Keskeytys total: ");
      Serial.println(interruptCountTotal);
    }
    if (interruptCount >= 10 && interruptCountTotal <= 99) {   // Kun 10, nopeutetaan
      OCR1A = OCR1A * 0.9;
      interruptCount = 0;
      if  (DEBUG == true){  Serial.print("SpeedyGonzales: "); Serial.println(OCR1A);}
    }
  }