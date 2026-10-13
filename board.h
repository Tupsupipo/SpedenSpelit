#ifndef BOARD_H
#define BOARD_H
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Määritetään onko käytössä arduino UNO vaiko MEGA. Määritetään rekisterit ja valitaan käytettävät pinnit sen mukaan

#if defined(ARDUINO_AVR_MEGA2560)
#define BOARD_NAME "MEGA"

const byte pins[] = {50, 51, 52, 53, 10}; // Käytettävät pinnit MEGA:lle. PCINT0-PCINT3 == pinnit 50-53. PCINT4 == pinni 10.
//  Näppäin asetukset MEGA:lle.
const byte PCICR_SETTING = B00000001;    // (1 << PCIE0)      Sama molemmissa == sama keskeytyspalvelin       Pelinapeille
const byte PCMSK0_SETTING = B00011111;   // PCINT0-PCINT3 == pinnit 50-53.     PCMSK2 MEGASSA sattuu analogiportteihin, joten käytetään mielummin PCMSK0


#elif defined(ARDUINO_AVR_UNO) 
#define BOARD_NAME "UNO"

const byte pins[] = {10, 11, 12, 13, 9}; // Käytettävät pinnit UNO:lle. Pelinapeille PCINT2-PCINT5 == pinnit 10-13. Käynnistysnappi PCINT1 == pinni 9.
// Näppäin asetukset UNO:lle. 
const byte PCICR_SETTING = B00000011;     // (1 << PCIE0)(1 << PCIE1)  PCIE0 = PCINT0-7, PCIE1 = PCINT8-14
const byte PCMSK0_SETTING = B00111110;   // PCINT1-PCINT5 == pinnit 9-13


#else
#error "Laitetta ei tunnistettu!"
#define BOARD_NAME "ERROR_404"

#endif
#endif