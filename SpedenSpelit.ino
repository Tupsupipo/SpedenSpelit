#include "display.h"
#include "board.h"
#include "buttons.h"
#include "leds.h"
#include "SpedenSpelit.h"


// Use these 2 volatile variables for communicating between
// loop() function and interrupt handlers
volatile bool gameRunning = false;              // Peli alkaa kun käynnistysnappia painettu.
extern volatile byte buttonNumber;              // for buttons interrupt handler
volatile bool newTimerInterrupt = false;        // for timer interrupt handler muutetaan kun tulee timer keskeytys
extern volatile byte buttonPinInterrupted;      // Pinni joka muutti tilaansa
volatile static byte interruptCount = 0;        // Lasketaan keskeytyksiä
volatile static byte interruptCountTotal = 0;   // Lasketaan keskeytyksien kokonaismäärä. 99 kipattava? vaiko pisteet alas ja uutta sataa lasiin?
byte buttonsPressed[99];                        // Taulukko nappien painalluksille.
byte randomizedNumbers[99];                     // Taulukko satunnaisille numeroille, jotka määrittävät syttyvän LEDin ja pelaajan painettavan napin.
byte buttonPressIndex = 0;                      // Lasketaan pelaajan painallusten indeksiä taulukkoon vertailua varten. 
byte randomizedLedIndex = 0;                    // Lasketaan satunnaister numeroiden indeksiä taulukkoon vertailua varten.
unsigned long startButtonPressTime = 0;         // Start napin painalluksen aika. Virran säästöä varten. 
bool startButtonDown = false;                   // Katsoo onko start nappi painettuna. Virran säästöä varten.

extern uint32_t pcintCounter;

void setup()
{
  /*
  Initialize here all modules
  */
  Serial.begin(115200);
  // Näitä varmaan siirrellää sopivampiin paikkoihin. Testiä
  Serial.print("Alustaksi maaritetty: ");
  Serial.println(BOARD_NAME);
  randomSeed(analogRead(A0));                            // käytetään analogRead(A0) randomin siemenlukuna. Tyhjän analogisen pinni A0 lukeminen antaa satunnaisen arvon, joka on hyvä siemenluku random() funktiolle.

  initButtonsAndButtonInterrupts();
  initializeLeds();
  initializeDisplay(); 
}

void loop()
{
  if(buttonNumber != 255)
  {
    if (buttonNumber == 4 && !startButtonDown) {
        startButtonDown = true;
        startButtonPressTime = millis(); 
    }
    if (startButtonDown && digitalRead(10) == HIGH) {    // Pinni 10 on HIGH, eli nappi vapautettiin
    unsigned long startButtonDownTime = millis() - startButtonPressTime;

    // start the game if buttonNumber == 4
    if (startButtonDownTime < 3000) {                   // Jos nappia painetaan alle 3000 ms, käynnistetään peli
    
      startTheGame(); 
      buttonNumber = 255;                 
    }
    startButtonDown = false;                           // Vapautetaan lukko
  }

  // Jos nappia on pidetty pohjassa yhtäjaksoisesti 3 sekuntia (3000 ms)
  if (startButtonDown && (millis() - startButtonPressTime >= 3000)) {
    startButtonDown = false; 
    buttonNumber = 255;
    startSleep(); // Mennään nukkumaan
  }

     // check the game if 0<=buttonNumber<4
    if (buttonNumber < 4 && gameRunning) {
        buttonsPressed[buttonPressIndex] = buttonNumber;  // tallennetaan pelaajan painallus taulukkoon vertailua varten
        checkGame(buttonPressIndex);                      // tarkistetaan painallus
        buttonNumber = 255;                               // resetoidaan buttonNumber BYTE viimeiseksi numeroksi
    }
  }

  if (gameRunning == false) {
    //odotellessa ledishow
    show1(); 
    delay(500); 
  }

  if(newTimerInterrupt && gameRunning)
  {
    interruptHandler();                                             // Kutsutaan keskeytyksien käsittelijää, jossa hoidetaan keskeytysten laskenta ja pelinouden kiihdytys

    // new random number must be generated
    byte randomized = random(4);                                    // Muuttuja satunnaisten lukujen generointiin. Random() funktio palauttaa satunnaisen luvun 0,1,2 tai 3. Tämä luku on seuraava syttyvä LED ja nappi jota pelaajan pitäisi painaa.
    int8_t prevRandom = -1;                                         // Edellinen satunnainen luku. Tämän hetkinen indexi -1
    if (randomizedLedIndex > 0) {                                   // Tarkistetaan, että sama LED ei syty uudestaan. Jos syttyy, generoidaan uusi satunnainen luku.
      prevRandom = randomizedNumbers[randomizedLedIndex - 1];       // Haetaan edellinen satunnainen luku taulukosta
      while (randomized == prevRandom) {                            // Jos sama luku, generoidaan uusi satunnainen luku, kunnes saadaan eri luku.
        randomized = random(4);
      }
    }

    // and corresponding let must be activated
    setLed(randomized);                                           // sytytetään satunnainen LED. Pelaajan pitäisi painaa vastaavaa nappia.
    randomizedNumbers[randomizedLedIndex] = randomized;           // tallennetaan satunnainen numero taulukkoon vertailua varten 
    randomizedLedIndex++;                                         // kasvatetaan indeksiä taulukkoon
    
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

  if (randomizedNumbers[buttonIndex] == buttonsPressed[buttonIndex]) {
      buttonPressIndex++;                               // kasvatetaan indeksiä taulukkoon
      showResult(buttonPressIndex);
    return;
  } 
  else {
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
}

void startTheGame()
{
   // see requirements for the function from SpedenSpelit.h
    gameRunning = true; 
    showResult(00);  // Näytetään 00 pelin alussa
    initializeTimer();
    initializeGame();

}

void gameOver()
{
  // Mieti miten peli lopetetaan
  gameRunning = false;
  show2(5);  // Näytetään valoshow pelin lopuksi
}

void interruptHandler() {
  if (gameRunning) {
      newTimerInterrupt = false;
      interruptCount++;
      interruptCountTotal++;

    if (interruptCount >= 10 && interruptCountTotal <= 99) {   // Kun 10, nopeutetaan
      OCR1A = OCR1A * 0.9;
      interruptCount = 0;
    }
    if (interruptCountTotal >= 99) {  // Kun 99, peli loppuu
      gameOver();
    }
  }
}

void startSleep() {
  while (digitalRead(10) == LOW) {  // Odotetaan, että start nappi vapautetaan ennen nukkumaanmenoa
    delay(10);
  }
  
  delay(500); //lisä debouncea, ettei start nappi herätä laitetta heti uudestaan. Alkuperäinen debounce on kulunut jo aikoja sitten.

  cli();                          // Sammutetaan keskeytykset ongelmien välttämiseksi.
  byte TIMSK1_backup = TIMSK1;     // Tallennetaan keskeytykset talteen, jotta voidaan palauttaa ne herätyksen jälkeen
  byte TCCR1B_backup = TCCR1B;      
  TIMSK1 = 0;                     // Sammutetaan Timer1 keskeytykset
  TCCR1B = 0;
  TIFR1 = _BV(OCF1A) | _BV(OCF1B) | _BV(TOV1); // Tyhjennetään Timer1-keskeytysjono
  sei();
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  Serial.flush();
  gameOver(); 
  clearDisplay();

  sleep_enable();
  sleep_mode();

  sleep_disable();

  cli();                          // Sammutetaan keskeytykset ongelmien välttämiseksi.
  TCCR1B = TCCR1B_backup;         // Palautetaan keskeytysten tila ennalleen
  TIMSK1 = TIMSK1_backup;         
  sei();                          // Keskeytykset takaisin päälle

  show2(3);
  delay(300);
  clearAllLeds();

  buttonNumber = 255;  // resetoidaan buttonNumber BYTE viimeiseksi numeroksi. Varmistetaan, että peli ei käynnisty vahingossa.
  startButtonDown = false;
  startButtonPressTime = millis();  
}
  