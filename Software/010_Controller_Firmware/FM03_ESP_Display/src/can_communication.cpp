//
// Created by robo on 29.05.26.
//

#include "can_communication.h"
#include "ui.h"
#include "driver/twai.h"



void sendHeartbeat() {
    static twai_message_t msg = {
        .extd = 0,
        .rtr = 0,
        .identifier = HEARTBEAT_ID,
        .data_length_code = 0,
    };
    // twai_message_t msg;
    // msg.identifier = HEARTBEAT_ID;      // CAN-ID
    // msg.extd = 0;                // 0 = Standard Frame (11bit), 1 = Extended (29bit)
    // msg.rtr = 0;
    // msg.data_length_code = 0;    // Anzahl Datenbytes (0-8)
    twai_transmit(&msg, pdMS_TO_TICKS(1000));  // 1000ms Timeout
}


void processIncomingCan(){
    twai_message_t msg;
    if (twai_receive(&msg, pdMS_TO_TICKS(0)) == ESP_OK) {
        static char canRxBuffer[50];     // Buffer for receiving
        static int canRxBufferIndex = -1;   // -1 = Disabled
        static int rxLength = 0;            // Length for string receive
        static uint8_t textId = 0;          // Text id to update

        switch(msg.identifier) {
            case CID_DISPLAY_SET_TEXT_HEADER:   // Expected: Display_ID; Text_ID; Text_Length
            {
                if (msg.data[0] != DISPLAY_ID) break;
                canRxBufferIndex = 0;
                textId = msg.data[1];
                rxLength = msg.data[2];

                if (rxLength >= MAX_LABEL_LEN) rxLength = MAX_LABEL_LEN - 1;    // -1 for terminator \0

                break;
            }
            case CID_DISPLAY_SET_TEXT_PAYLOAD:  // Expected: UTF-8[0]; UTF-8[1]; ... UTF-8[7]
                if (canRxBufferIndex < 0) break;  // We are not the target display, return
                for ( int i = 0; i < msg.data_length_code; i++) {
                    canRxBuffer[canRxBufferIndex++] = msg.data[i];
                }
                break;
            case CID_DISPLAY_SET_BOX_COLOR:     // Expected: Display_ID; Box_ID; R; G; B

                break;
            case CID_DISPLAY_UPDATE:            // Expected: Display_ID (If Display_ID == 255: All displays
                if (msg.data[0] == DISPLAY_ID || msg.data[0] == 255) {
                    update_screen();
                }
                break;
            default:
                // No relevant information for our display
                break;
        }
        if (canRxBufferIndex > 0 && canRxBufferIndex >= rxLength) {
            Serial0.println("Message complete");
            canRxBuffer[rxLength] = '\0';       // add String Terminator
            update_label(textId, canRxBuffer);
            canRxBufferIndex = -1;
        }
        Serial0.printf("ID: 0x%03X  DLC: %d  Data: ", msg.identifier, msg.data_length_code);
        for (int i = 0; i < msg.data_length_code; i++) {
            Serial0.printf("%02X ", msg.data[i]);
        }
        Serial0.println();
    }
}