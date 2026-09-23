#include "display.h"
#include "board.h"
#include "buttons.h"
#include "leds.h"
#include "SpedenSpelit.h"

// Use these 2 volatile variables for communicating between
// loop() function and interrupt handlers
volatile bool gameRunning = false;            // Peli alkaa kun käynnistysnappia painettu.
extern volatile byte buttonNumber;            // for buttons interrupt handler
volatile bool newTimerInterrupt = false;      // for timer interrupt handler muutetaan kun tulee timer keskeytys
bool DEBUG = true;                            // Debug muuttuja. Helpottanee asioiden tutkimista ja ylimääräisen roskan siivoamista lopuksi
extern volatile int buttonPinInterrupted;     // Pinni joka muutti tilaansa
volatile static int interruptCount = 0;       // Lasketaan keskeytyksiä
volatile static int interruptCountTotal = 0;  // Lasketaan keskeytyksien kokonaismäärä. 99 kipattava? vaiko pisteet alas ja uutta sataa lasiin?
volatile int buttonsPressed[99];              // Taulukko nappien painalluksille.
volatile int randomizedNumbers[99];           // Taulukko satunnaisille numeroille, jotka määrittävät syttyvän LEDin ja pelaajan painettavan napin.
volatile int buttonPressIndex = 0;            // Lasketaan pelaajan painallusten indeksiä taulukkoon vertailua varten. 
volatile int randomizedLedIndex = 0;          // Lasketaan satunnaister numeroiden indeksiä taulukkoon vertailua varten.


void setup()
{
  /*
  Initialize here all modules
  */
 Serial.begin(9600);
 // Näitä varmaan siirrellää sopivampiin paikkoihin. Testiä
 Serial.print("Alustaksi maaritetty: ");
 Serial.println(BOARD_NAME);
    randomSeed(analogRead(A0));                            // käytetään analogRead(A0) randomin siemenlukuna. Tyhjän analogisen pinni A0 lukeminen antaa satunnaisen arvon, joka on hyvä siemenluku random() funktiolle.
    initButtonsAndButtonInterrupts();
    initializeLeds();
        
}

void loop()
{
  if(buttonNumber>=0)
  {
     // start the game if buttonNumber == 4
    if (buttonNumber == 4) { 
        startTheGame();                                   // Käynnistetään peli. Peli alkaa, kun start button painetaan.
        if (DEBUG == true){  Serial.println("Käynnistetään peliä.");}
        buttonNumber = -1;                                // resetoidaan buttonNumber
    }
     // check the game if 0<=buttonNumber<4
    if (buttonNumber < 4) {
        buttonsPressed[buttonPressIndex] = buttonNumber;  // tallennetaan pelaajan painallus taulukkoon vertailua varten
        checkGame(buttonPressIndex);                      // tarkistetaan painallus
        buttonPressIndex++;                               // kasvatetaan indeksiä taulukkoon
        if (DEBUG == true){  Serial.print("Nappia painettu: ");Serial.println(buttonNumber);}
        buttonNumber = -1;                                // resetoidaan buttonNumber
      
    }
  }

  if (gameRunning == false) {
      if (DEBUG == true){  Serial.println("Peli ei vielä käynnissä. Näytetään valoshow");}
      //odotellessa ledishow
      show1();  

  } 
  

  if(newTimerInterrupt && gameRunning)
  {
    interruptHandler();

    // new random number must be generated
    int randomized = random(4);                                     // Muuttuja satunnaisten lukujen generointiin. Random() funktio palauttaa satunnaisen luvun 0,1,2 tai 3. Tämä luku on seuraava syttyvä LED ja nappi jota pelaajan pitäisi painaa.
    int prevRandom = -1;                                            // Edellinen satunnainen luku. Tämän hetkinen indexi -1
    if (randomizedLedIndex > 0) {                                   // Tarkistetaan, että sama LED ei syty uudestaan. Jos syttyy, generoidaan uusi satunnainen luku.
      prevRandom = randomizedNumbers[randomizedLedIndex - 1];       // Haetaan edellinen satunnainen luku taulukosta
      while (randomized == prevRandom) {                            // Jos sama luku, generoidaan uusi satunnainen luku, kunnes saadaan eri luku.
        randomized = random(4);
      }
    }

    // and corresponding let must be activated
    setLed(randomized);                                           // sytytetään satunnainen LED. Pelaajan pitäisi painaa vastaavaa nappia. TODO: Lisää taulukkoon. Älä sytytä samaa LEDiä kuin edellisellä kierroksella. Painetaanko vastaavaa nappia. Tallennetaan nekin taulukkoon ja verrataan painojärjestystä.
    randomizedNumbers[randomizedLedIndex] = randomized;           // tallennetaan satunnainen numero taulukkoon vertailua varten

    if (DEBUG == true){  Serial.print("Random number: ");Serial.println(randomized);}
    Serial.print("randomizedNumbers: ");
    for (int i = 0; i <= randomizedLedIndex; i++) {
      Serial.print(randomizedNumbers[i]);
      Serial.print(" ");
    }
    Serial.println();
    randomizedLedIndex++;                                 // kasvatetaan indeksiä taulukkoon
    
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


void checkGame(byte buttonIndex)
{
//  for (int i = 0; i < buttonIndex; i++) {
    if (randomizedNumbers[buttonIndex] == buttonsPressed[buttonIndex]) {
      if (DEBUG == true){ Serial.print("Pelaaja painoi oikeaa nappia: "); Serial.println(buttonsPressed[buttonIndex]); Serial.print("INDEX: "); Serial.println(buttonIndex); }
      return;
    } 
    else {
      if (DEBUG == true){ Serial.print("Väärää nappi: "); Serial.print(buttonsPressed[buttonIndex]); Serial.print("INDEX: "); Serial.println(buttonIndex); }
      gameOver();  // Väärää nappi. Peli loppuu.
    }
}


void initializeGame()
{
	// see requirements for the function from SpedenSpelit.h
  buttonPressIndex = 0;            // Nollataan pelaajan painallusten indeksi pelin alussa.
  randomizedLedIndex = 0;          // Nollataan satunnaisten numeroiden indeksi pelin alussa.
  interruptCount = 0;              // Nollataan keskeytyslaskuri pelin alussa
  interruptCountTotal = 0;         // Nollataan keskeytyslaskuri pelin alussa         
    if (DEBUG == true){  Serial.println("initializeGame() initialized");}

  
}

void startTheGame()
{
   // see requirements for the function from SpedenSpelit.h
    if (DEBUG == true){  Serial.println("startTheGame() initialized");}
    gameRunning = true; 
    initializeTimer();
    initializeGame();

}

void gameOver()
{
  // Mieti miten peli lopetetaan
  gameRunning = false;

}

void interruptHandler() {
  if (gameRunning) {
      newTimerInterrupt = false;
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
}