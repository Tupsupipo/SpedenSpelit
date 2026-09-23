#ifndef BOARD_H
#define BOARD_H
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Määritetään onko käytössä arduino UNO vaiko MEGA. Määritetään rekisterit ja valitaan käytettävät pinnit sen mukaan

#if defined(ARDUINO_AVR_MEGA2560)
#define BOARD_NAME "MEGA"
const int startButtonPin = 10;  // Käynnistysnappi MEGA:lle. PCINT4. Sijaitsee PCIE0 ryhmässä, joten voidaan käyttää alla olevaa PCINT0_vect keskeytystä.
const int pins[] = {50, 51, 52, 53, 10}; // Käytettävät pinnit MEGA:lle. PCINT0-PCINT3 == pinnit 50-53. PCINT4 == pinni 10.
//  Näppäin asetukset MEGA:lle.
const uint8_t PCICR_SETTING = B00000001;    // (1 << PCIE0)      Sama molemmissa == sama keskeytyspalvelin
const uint8_t PCMSK0_SETTING = B00011111;   // PCINT0-PCINT3 == pinnit 50-53.     PCMSK2 MEGASSA sattuu analogiportteihin, joten käytetään mielummin PCMSK0
const uint8_t PCMSK1_SETTING = B00000000;   // O, koska ei käytetä MEGALLA

#elif defined(ARDUINO_AVR_UNO) 
#define BOARD_NAME "UNO"
const int startButtonPin = A0; // Käynnistysnappi UNO:lle. PCINT8 Sijaitsee PCIE1 ryhmässä, joten pitää käyttää PCINT1_vect keskeytystä.
const int pins[] = {10, 11, 12, 13};
// Näppäin asetukset UNO:lle. 
const uint8_t PCICR_SETTING = B0000011;     // (1 << PCIE0)
const uint8_t PCMSK0_SETTING = B00111100;   // PCINT2-PCINT5 == pinnit 10-13
const uint8_t PCMSK1_SETTING = B00000001;  // PCINT8 == pinni A0. startButtonPin UNO:lla.

#else
#error "Laitetta ei tunnistettu!"
#define BOARD_NAME "ERROR_404"

#endif
#endif