// Hier findet man alle CAN IDs aufgeschlüsselt
// Jede ID fängt mit CID an (CAN ID)
// Die CAN-IDs sind 11-Bit codes (max=2048 = 0x800). Die kleinere Zahl hat Priorität

#ifndef CAN_IDs_H
#define CAN_IDs_H
#include <Arduino.h>

// ##################################################################
// # Configuration                                                  #
// ##################################################################
const uint32_t CAN_BITRATE = 500000;

// ##################################################################
// # Encoder actions                           (Controller -> Host) #
// ##################################################################
// Expected arguments: Local Encoder Nr, Relative Position (Delta since last sent)
const uint16_t  CID_ENCODER_EVENT_GRP0           = 0x200;    // GRP0: RP Pico 2 of Encoder 0 PCB
const uint16_t  CID_ENCODER_EVENT_GRP1           = 0x201;    // GRP1: RP Pico 2 of Encoder 1 PCB
const uint16_t  CID_ENCODER_EVENT_GRP2           = 0x202;    // GRP2: ESP Haptic Encoder 0
const uint16_t  CID_ENCODER_EVENT_GRP3           = 0x203;    // GRP3: ESP Haptic Encoder 1
const uint16_t  CID_ENCODER_EVENT_GRP4           = 0x204;    // GRP4: ESP Haptic Encoder 3 (Intensity Wheel)
const uint16_t  CID_ENCODER_EVENT_GRP5           = 0x205;
const uint16_t  CID_ENCODER_EVENT_GRP6           = 0x206;
const uint16_t  CID_ENCODER_EVENT_GRP7           = 0x207;
const uint16_t  CID_ENCODER_EVENT_GRP8           = 0x208;
const uint16_t  CID_ENCODER_EVENT_GRP9           = 0x209;
const uint16_t  CID_ENCODER_EVENT_GRP10          = 0x20A;
const uint16_t  CID_ENCODER_EVENT_GRP11          = 0x20B;
const uint16_t  CID_ENCODER_EVENT_GRP12          = 0x20C;
const uint16_t  CID_ENCODER_EVENT_GRP13          = 0x20D;
const uint16_t  CID_ENCODER_EVENT_GRP14          = 0x20E;
const uint16_t  CID_ENCODER_EVENT_GRP15          = 0x20F;


// ##################################################################
// # Button actions                            (Controller -> Host) #
// ##################################################################
// Expected arguments: Local Button Nr (MSB Set = pushed)
const uint16_t  CID_BUTTON_EVENT_GRP0           = 0x220;    // GRP0: RP Pico 2 of Encoder 0 PCB
const uint16_t  CID_BUTTON_EVENT_GRP1           = 0x221;    // GRP1: RP Pico 2 of Encoder 1 PCB
const uint16_t  CID_BUTTON_EVENT_GRP2           = 0x222;    // GRP2: RP Pico of programming keyboard
const uint16_t  CID_BUTTON_EVENT_GRP3           = 0x223;
const uint16_t  CID_BUTTON_EVENT_GRP4           = 0x224;
const uint16_t  CID_BUTTON_EVENT_GRP5           = 0x225;
const uint16_t  CID_BUTTON_EVENT_GRP6           = 0x226;
const uint16_t  CID_BUTTON_EVENT_GRP7           = 0x227;
const uint16_t  CID_BUTTON_EVENT_GRP8           = 0x228;
const uint16_t  CID_BUTTON_EVENT_GRP9           = 0x229;
const uint16_t  CID_BUTTON_EVENT_GRP10          = 0x22A;
const uint16_t  CID_BUTTON_EVENT_GRP11          = 0x22B;
const uint16_t  CID_BUTTON_EVENT_GRP12          = 0x22C;
const uint16_t  CID_BUTTON_EVENT_GRP13          = 0x22D;
const uint16_t  CID_BUTTON_EVENT_GRP14          = 0x22E;
const uint16_t  CID_BUTTON_EVENT_GRP15          = 0x22F;


// ##################################################################
// # Display actions                           (Host -> Controller) #
// ##################################################################
const uint16_t  CID_DISPLAY_SET_TEXT_HEADER     = 0x501;    // Expected: Display_ID (begins at 0x00); Text_ID; Text_Length
const uint16_t  CID_DISPLAY_SET_TEXT_PAYLOAD    = 0x500;    // Expected: UTF-8[0]; UTF-8[1]; ... UTF-8[7]
const uint16_t  CID_DISPLAY_SET_BOX_COLOR       = 0x502;    // Expected: Display_ID; Box_ID; R; G; B
const uint16_t  CID_DISPLAY_UPDATE              = 0x503;    // Expected: Display_ID (If Display_ID == 255: All displays update)

// ##################################################################
// # LED actions                               (Host -> Controller) #
// ##################################################################
const uint16_t  CID_LED_UPDATE                  = 0x550;
const uint16_t  CID_LED_SET_INDIVIDUAL          = 0x551;    // Expected: LED_ID, LED_COLOR_INTENS
const uint16_t  CID_LED_SET_GROUP               = 0x552;    // Expected: GROUP_ID, LED_COLOR_INTENS
const uint16_t  CID_LED_SET_INDI_BLINKMODE      = 0x553;    // Expected: LED_ID, LED_BLINKMODE
const uint16_t  CID_LED_SET_GROUP_BLINKMODE     = 0x554;    // Expected: GROUP_ID, LED_BLINKMODE

// LED_COLOR_INTENS
// Sends COLOR_ID (5 lower Bit -> 32 Variations) and INTENSITY_ID (3 upper Bit -> 8 Variations) in one byte
const uint8_t   INTENS_ID_0                     = 0b00000000;   // OFF
const uint8_t   INTENS_ID_1                     = 0b00100000;   // Low Light
const uint8_t   INTENS_ID_2                     = 0b01000000;
const uint8_t   INTENS_ID_3                     = 0b01100000;
const uint8_t   INTENS_ID_4                     = 0b10000000;
const uint8_t   INTENS_ID_5                     = 0b10100000;
const uint8_t   INTENS_ID_6                     = 0b11000000;
const uint8_t   INTENS_ID_7                     = 0b11100000;   // Full On

const uint8_t   COLOR_ID_RED                    = 0;
const uint8_t   COLOR_ID_RED_ORANGE             = 1;
const uint8_t   COLOR_ID_ORANGE                 = 2;
const uint8_t   COLOR_ID_GOLD_ORANGE            = 3;
const uint8_t   COLOR_ID_GOLD_YELLOW            = 4;
const uint8_t   COLOR_ID_YELLOW                 = 5;
const uint8_t   COLOR_ID_YELLOW_GREEN           = 6;
const uint8_t   COLOR_ID_LIME_GREEN             = 7;
const uint8_t   COLOR_ID_APPLE_GREEN            = 8;
const uint8_t   COLOR_ID_SPRING_GREEN           = 9;
const uint8_t   COLOR_ID_GREEN                  = 10;
const uint8_t   COLOR_ID_GREEN_TURQUOISE        = 11;
const uint8_t   COLOR_ID_EMERALD_GREEN          = 12;
const uint8_t   COLOR_ID_AQUA_GREEN             = 13;
const uint8_t   COLOR_ID_TURQUOISE              = 14;
const uint8_t   COLOR_ID_CYAN                   = 15;
const uint8_t   COLOR_ID_LIGHT_BLUE             = 16;
const uint8_t   COLOR_ID_AZURE_BLUE             = 17;
const uint8_t   COLOR_ID_SKY_BLUE               = 18;
const uint8_t   COLOR_ID_DEEP_BLUE              = 19;
const uint8_t   COLOR_ID_BLUE                   = 20;
const uint8_t   COLOR_ID_BLUE_VIOLET            = 21;
const uint8_t   COLOR_ID_INDIGO                 = 22;
const uint8_t   COLOR_ID_VIOLET                 = 23;
const uint8_t   COLOR_ID_PURPLE_VIOLET          = 24;
const uint8_t   COLOR_ID_MAGENTA                = 25;
const uint8_t   COLOR_ID_PINK_MAGENTA           = 26;
const uint8_t   COLOR_ID_PINK                   = 27;
const uint8_t   COLOR_ID_RASPBERRY_RED          = 28;
const uint8_t   COLOR_ID_CARMINE_RED            = 29;
const uint8_t   COLOR_ID_WARM_WHITE             = 30;
const uint8_t   COLOR_ID_COOL_WHITE             = 31;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RgbColor;

static const RgbColor led_colors[32] = {
    {255,   0,   0}, //  0 Rot
    {255,  51,   0}, //  1 Rotorange
    {255, 102,   0}, //  2 Orange
    {255, 153,   0}, //  3 Goldorange
    {255, 204,   0}, //  4 Goldgelb
    {255, 255,   0}, //  5 Gelb
    {204, 255,   0}, //  6 Gelbgrün
    {153, 255,   0}, //  7 Limettengrün
    {102, 255,   0}, //  8 Apfelgrün
    { 51, 255,   0}, //  9 Frühlingsgrün
    {  0, 255,   0}, // 10 Grün
    {  0, 255,  51}, // 11 Grün-Türkis
    {  0, 255, 102}, // 12 Smaragdgrün
    {  0, 255, 153}, // 13 Aquagrün
    {  0, 255, 204}, // 14 Türkis
    {  0, 255, 255}, // 15 Cyan
    {  0, 204, 255}, // 16 Hellblau
    {  0, 153, 255}, // 17 Azurblau
    {  0, 102, 255}, // 18 Himmelblau
    {  0,  51, 255}, // 19 Tiefblau
    {  0,   0, 255}, // 20 Blau
    { 51,   0, 255}, // 21 Blauviolett
    {102,   0, 255}, // 22 Indigo
    {153,   0, 255}, // 23 Violett
    {204,   0, 255}, // 24 Purpurviolett
    {255,   0, 255}, // 25 Magenta
    {255,   0, 204}, // 26 Pink-Magenta
    {255,   0, 153}, // 27 Pink
    {255,   0, 102}, // 28 Himbeerrot
    {255,   0,  51}, // 29 Karminrot
    {255, 180, 100}, // 30 Warmweiß
    {180, 220, 255}, // 31 Kaltweiß
};

// LED_BLINKMODES
// Use one of the Modes Static, Slow, Mid, Fast and enable Fade In/Out with bit setting.
const uint8_t   LED_BLINKMODE_STATIC            = 0b00000000;
const uint8_t   LED_BLINKMODE_SLOW              = 0b00000001;
const uint8_t   LED_BLINKMODE_MID               = 0b00000010;
const uint8_t   LED_BLINKMODE_FAST              = 0b00000011;
const uint8_t   LED_BLINKMODE_FADE_IN           = 0b00000100;
const uint8_t   LED_BLINKMODE_FADE_OUT          = 0b00001000;

// ##################################################################
// # Heartbeats                                (Host <> Controller) #
// ##################################################################
const uint32_t  HEARTBEAT_INTERVALL             = 3000;     // The period in which the heartbeat is sent
const uint32_t  HEARTBEAT_TIMEOUT               = 5000;     // Timeout when a controller is considerd offline

const uint16_t  CID_HEARTBEAT_MAC               = 0x700;

const uint16_t  CID_HEARTBEAT_RPI_ENCODER_0     = 0x701;
const uint16_t  CID_HEARTBEAT_RPI_ENCODER_1     = 0x702;
const uint16_t  CID_HEARTBEAT_ESP_FADER_0       = 0x703;
const uint16_t  CID_HEARTBEAT_ESP_FADER_1       = 0x704;
const uint16_t  CID_HEARTBEAT_ESP_DISPLAY_0     = 0x705;
const uint16_t  CID_HEARTBEAT_ESP_DISPLAY_1     = 0x706;

const uint16_t  CID_HEARTBEAT_RPI_KEYBOARD      = 0x707;
const uint16_t  CID_HEARTBEAT_ESP_HAPTIC_0      = 0x708;
const uint16_t  CID_HEARTBEAT_ESP_HAPTIC_1      = 0x709;
const uint16_t  CID_HEARTBEAT_ESP_HAPTIC_2      = 0x70A;



#endif // CAN_IDs_H
