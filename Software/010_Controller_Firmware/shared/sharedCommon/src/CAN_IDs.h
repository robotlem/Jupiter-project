// Hier findet man alle CAN IDs aufgeschlüsselt
// Jede ID fängt mit CID an (CAN ID)
// Die CAN-IDs sind 11-Bit codes (max=2048 = 0x800). Die kleinere Zahl hat Priorität

#ifndef CAN_IDs_H
#define CAN_IDs_H
#include <Arduino.h>


// ##################################################################
// # Display actions                                                #
// ##################################################################
const uint16_t  CID_DISPLAY_SET_TEXT_HEADER     = 0x501;    // Expected: Display_ID; Text_ID; Text_Length
const uint16_t  CID_DISPLAY_SET_TEXT_PAYLOAD    = 0x500;    // Expected: UTF-8[0]; UTF-8[1]; ... UTF-8[7]
const uint16_t  CID_DISPLAY_SET_BOX_COLOR       = 0x502;    // Expected: Display_ID; Box_ID; R; G; B
const uint16_t  CID_DISPLAY_UPDATE              = 0x503;    // Expected: Display_ID (If Display_ID == 255: All displays update)





// ##################################################################
// # Heartbeats                                                     #
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

const uint16_t  CID_HEARTBEAT_ESP_KEYBOARD      = 0x707;
const uint16_t  CID_HEARTBEAT_ESP_HAPTIC_0      = 0x708;
const uint16_t  CID_HEARTBEAT_ESP_HAPTIC_1      = 0x709;
const uint16_t  CID_HEARTBEAT_ESP_HAPTIC_2      = 0x70A;



#endif // CAN_IDs_H
