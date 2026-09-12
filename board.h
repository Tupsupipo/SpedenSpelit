#ifndef BOARD_H
#define BOARD_H
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Määritetään onko käytössä arduino UNO vaiko MEGA ja valitaan käytettävät pinnit sen mukaan

#if defined(ARDUINO_AVR_MEGA2560)
#define BOARD_NAME "MEGA"
const int pins[] = {50, 51, 52, 53};
//  Mieti mitä näille. Samanlainen systeemi taidetaan tarvia .cpp PCICR = PCICR_SETTING? .h CONST PCICR_SETTING = B00000001;  ??
//  PCICR = B00000001;    // PCIE0
//  PCMSK0 = B00001111;   // PCINT0-PCINT3 == pinnit 50-53 

#elif defined(ARDUINO_AVR_UNO) // TODO: kattele UNOa ja päivitä tiedot
#define BOARD_NAME "UNO"
const int pins[] = {2, 3, 4, 5};

#else
#error "Laitetta ei tunnistettu!"
#define BOARD_NAME "ERROR_404"

#endif
#endif