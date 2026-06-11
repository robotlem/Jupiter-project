//
// Created by robo on 19.05.26.
//

#ifndef ZA_TEST_ENCODER_RPI_CONFIG_H
#define ZA_TEST_ENCODER_RPI_CONFIG_H

#include "Arduino.h"

// --- CAN CONFIGURATION ---
const uint32_t CAN_BITRATE = 125000; // 125 kbit/s (anpassen falls nötig)
const int CAN_TX_PIN = 16;           // Beliebige freie GPIOs wählbar
const int CAN_RX_PIN = 17;

// --- Encoder Configuration
const uint8_t PIN_ENC0 = 9;     // PIO 1
const uint8_t PIN_ENC1 = 8;     // PIO 1
const uint8_t PIN_ENC2 = 10;    // PIO 1
const uint8_t PIN_ENC3 = 12;    // PIO 1

const uint8_t PIN_ENC4 = 4;     // SW
const uint8_t PIN_ENC5 = 2;     // SW
const uint8_t PIN_ENC6 = 0;     // SW
const uint8_t PIN_ENC7 = 14;    // SW
const uint8_t PIN_ENC8 = 18;    // SW
const uint8_t PIN_ENC9 = 20;    // SW


#endif //ZA_TEST_ENCODER_RPI_CONFIG_H