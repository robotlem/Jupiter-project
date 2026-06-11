//
// Created by robo on 29.05.26.
//

#ifndef FM03_ESP_DISPLAY_CAN_COMMUNICATION_H
#define FM03_ESP_DISPLAY_CAN_COMMUNICATION_H
#include <Arduino.h>
#include <CAN_IDs.h>

#define CAN_TX_PIN GPIO_NUM_17
#define CAN_RX_PIN GPIO_NUM_18

#ifndef SECOND_DISPLAY
constexpr uint8_t DISPLAY_ID = 0;
constexpr uint16_t HEARTBEAT_ID = CID_HEARTBEAT_ESP_DISPLAY_0;
#else
const uint8_t DISPLAY_ID = 1;
const uint16_t HEARTBEAT_ID = CID_HEARTBEAT_ESP_DISPLAY_1;
#endif

void sendHeartbeat();
void processIncomingCan();

#endif //FM03_ESP_DISPLAY_CAN_COMMUNICATION_H