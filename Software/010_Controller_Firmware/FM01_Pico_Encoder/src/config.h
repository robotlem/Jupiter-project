//
// Created by robo on 19.05.26.
//

#ifndef ZA_TEST_ENCODER_RPI_CONFIG_H
#define ZA_TEST_ENCODER_RPI_CONFIG_H

#include "Arduino.h"

// --- CAN CONFIGURATION ---    // CAN Läuft auf PIO 0
const int CAN_TX_PIN = 16;           // Beliebige freie GPIOs wählbar
const int CAN_RX_PIN = 17;
const uint32_t CAN_BUS_BITRATE = 125000;

// --- Encoder Configuration ---
const uint8_t PIN_ENC0 = 6;     // PIO 1
const uint8_t PIN_ENC1 = 8;     // PIO 1
const uint8_t PIN_ENC2 = 10;    // PIO 1
const uint8_t PIN_ENC3 = 12;    // PIO 1

const uint8_t PIN_ENC4 = 14;    // PIO 2

// --- SPI Configuration ---
const uint8_t PIN_SPI_SCK  = 18;
const uint8_t PIN_SPI_MOSI = 19;
const uint8_t PIN_SPI_MISO = 20;
const uint8_t PIN_SPI_CS   = 21;

// --- MCP23S17 Configuration ---
const uint8_t PIN_MCP0_INT = 5;
const uint8_t PIN_MCP1_INT = 4;

// --- SK6812 Mini-E Configuration ---
const uint8_t LED_PIN = 2;

// How to get the Index of the local LED Array from LED_ID?
// 1. Check for range: if (LED_ID-LED_OFFSET > LED_MAX_ID || LED_ID-LED_OFFSET < 0) return;
// 2. Get local Array Index: LED_MAPPING(LED_ID - LED_OFFSET)
// Check Result: 255 = invalid

#ifdef SECOND_ENCODER
const uint8_t NUM_LEDS = 32;
const uint8_t LED_OFFSET = 0;
const uint8_t LED_MAX_ID = 52;
const uint8_t LED_MAPPING[] = {
// 0    1    2    3    4      5    6    7    8    9
  255, 255, 255, 255, 255,   255,   0,   1,   2,   3, //  0 -   9
    4,   5, 255, 255, 255,   255, 255, 255, 255, 255, // 10 -  19
   255, 255, 255, 255, 255,  255, 255,  22,  23,  24, // 20 -  29
    25,  26,  27,  28,  29,   30,  31,   6,   7,   8, // 30 -  39
     9,  10,  11,  12,  13,   14,  15,  16,  17,  18, // 40 -  49
    19,  20,  21
};
#else
const uint8_t NUM_LEDS = 27;
const uint8_t LED_OFFSET = 0;
const uint8_t LED_MAX_ID = 26;
const uint8_t LED_MAPPING[] = {
      0,   1,   2,   3,   4,     5, 255, 255, 255, 255, //  0 -   9
    255, 255,   6,   7,   8,     9,  10,  11,  12,  13, // 10 -  19
     14,  15,  16,  17,  18,    19,  20, 255, 255, 255  // 20 -  29
};
#endif

#endif //ZA_TEST_ENCODER_RPI_CONFIG_H
