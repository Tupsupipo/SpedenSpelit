#ifndef BOARD_H
#define BOARD_H
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Määritetään onko käytössä arduino UNO vaiko MEGA. Määritetään rekisterit ja valitaan käytettävät pinnit sen mukaan

#if defined(ARDUINO_AVR_MEGA2560)
#define BOARD_NAME "MEGA"
const int pins[] = {50, 51, 52, 53};
//  Näppäin asetukset MEGA:lle.
const uint8_t PCICR_SETTING = B00000001;    // (1 << PCIE0)      Sama molemmissa == sama keskeytyspalvelin
const uint8_t PCMSK0_SETTING = B00001111;   // PCINT0-PCINT3 == pinnit 50-53.     PCMSK2 MEGASSA sattuu analogiportteihin, joten käytetään mielummin PCMSK0

#elif defined(ARDUINO_AVR_UNO) 
#define BOARD_NAME "UNO"
const int pins[] = {10, 11, 12, 13};
// Näppäin asetukset UNO:lle. 
const uint8_t PCICR_SETTING = B0000001;     // (1 << PCIE0)
const uint8_t PCMSK0_SETTING = B00111100;   // PCINT2-PCINT5 == pinnit 10-13


#else
#error "Laitetta ei tunnistettu!"
#define BOARD_NAME "ERROR_404"

#endif
#endif