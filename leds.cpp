#include "leds.h"
extern volatile byte buttonNumber;   // Käytetään LED-showssa, jotta voidaan lopettaa show, jos start nappia painetaan

// Varataan Analogi pinnit A2-A5 Ledeille.
const byte ledPins[4] = {A2, A3, A4, A5};

void initializeLeds()
{
  // Pinnit ulostuloiksi ja varmistetaan että ne ovat pois päältä
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); 
  }
}

void setLed(byte ledNumber)
{
  if (ledNumber > 3) return; // Varmistus
  
  // Sammutetaan kaikki muut ledit ensin (vaatimuksen mukaisesti)
  clearAllLeds(); 
  
  // Sytytetään vain haluttu ledi
  digitalWrite(ledPins[ledNumber], HIGH);
}

void clearAllLeds()
{
  // Käydään kaikki ledit läpi ja sammutetaan
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

void setAllLeds()
{
  // Käydään kaikki ledit läpi ja sytytetään
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
}

void show1()
{
  // Kirjoitetaan numerot 0-15 binääriesityksenä ledeillä
  for (byte i = 0; i <= 15; i++) {
    if (buttonNumber <= 4) {  // Jos start nappia painetaan, lopetetaan LED-show
      clearAllLeds(); // Sammutetaan ledit ennen poistumista
        return;  // Poistu heti LED-showsta
    }
    for (byte bit = 0; bit < 4; bit++) {
      // bitRead hakee luvusta "i" bitin kohdasta "bit"
      if (bitRead(i, bit)) {
        digitalWrite(ledPins[bit], HIGH);
      } else {
        digitalWrite(ledPins[bit], LOW);
      }
    }
    // Pieni viive, jotta binääriluvut erottaa toisistaan
    // 400 ms viive, mutta tarkistetaan nappi 1 ms välein. Muutin jotta päästään poistumaan showsta nopeasti, jos start nappia painetaan.
    for (int t = 0; t < 400; t++) {
      if (buttonNumber <= 4) {  // Jos start nappia painetaan, lopetetaan LED-show
        clearAllLeds();
        return;
      }
      delay(1);
    }
  }
  clearAllLeds(); // Sammutetaan lopuksi
}

void show2(int rounds)
{
  int viive = 300; // Aloitusnopeus
  
  for (int r = 0; r < rounds; r++) {
    clearAllLeds(); // Aloitetaan kierros pimeänä
    
    // Sytytetään ledit yksitellen päälle niin, että aiemmat jäävät palamaan
    for (int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], HIGH);
      delay(viive);
    }
    
    // Kiihtyvä tahti: pienennetään viivettä seuraavalle kierrokselle
    if (viive > 20) {
      viive = (viive * 8) / 10; // Lyhentää viivettä 20 % joka kierroksella
    }
  }
  clearAllLeds();
}

void show3(byte rounds)         // Lisäsin sammutusta varten. Näyttää kaikki ledit päälle ja pois päältä "rounds" kertaa.
{
  for (int r = 0; r < rounds; r++) {
    setAllLeds();
    delay(200); 
    clearAllLeds();
  }
}