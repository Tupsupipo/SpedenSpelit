#include "leds.h"

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
    for (byte bit = 0; bit < 4; bit++) {
      // bitRead hakee luvusta "i" bitin kohdasta "bit"
      if (bitRead(i, bit)) {
        digitalWrite(ledPins[bit], HIGH);
      } else {
        digitalWrite(ledPins[bit], LOW);
      }
    }
    // Pieni viive, jotta binääriluvut erottaa toisistaan
    delay(400); 
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