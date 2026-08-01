#include <Arduino.h>
#include <CAN_IDs.h>
#include <FastLED.h>
#include <pio_encoder.h>

#include "can_bus.h"
#include "config.h"
#include "encoder.h"
#include "led_controller.h"
#include "MCP23S17.h"

CRGB leds[NUM_LEDS];

PioEncoder pio_encoders[] = {
    PioEncoder(PIN_ENC0, pio1),
    PioEncoder(PIN_ENC1, pio1),
    PioEncoder(PIN_ENC2, pio1),
    PioEncoder(PIN_ENC3, pio1),
    PioEncoder(PIN_ENC4, pio2),
};

MCP23S17 mcp23s17[2];

constexpr size_t ENCODER_COUNT = sizeof(pio_encoders) / sizeof(pio_encoders[0]);
EncoderManager encoderManager(pio_encoders, ENCODER_COUNT, CID_ENCODER_EVENT);
LedController ledController(leds, NUM_LEDS);

void setupLeds() {
    ledController.begin();
}

void setupMCP23S17() {
    uint8_t ids[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    mcp23s17[0].setButtonIds(ids);
    mcp23s17[0].init(PIN_MCP0_INT, 0x00, CID_BUTTON_EVENT);

    uint8_t ids2[] = {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
    mcp23s17[1].setButtonIds(ids2);
    mcp23s17[1].init(PIN_MCP1_INT, 0x01, CID_BUTTON_EVENT);
}

void printCanMessage(const CanBusMessage &message) {
    Serial.print("CAN RX  ID: 0x");
    Serial.print(message.id, HEX);
    Serial.print("  DLC: ");
    Serial.print(message.length);
    Serial.print("  Data: ");

    for (uint8_t i = 0; i < message.length; i++) {
        if (message.data[i] < 0x10) {
            Serial.print('0');
        }
        Serial.print(message.data[i], HEX);
        Serial.print(' ');
    }
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("FM01 Pico Encoder");

    encoderManager.begin();
    Serial.println("Encoder initialisiert");

    canBusBegin();
    Serial.println("CAN initialisiert");

    setupLeds();
    Serial.println("LEDs initialisiert");

    setupMCP23S17();
    Serial.println("MCP23S17 initialisiert");

    Serial.println("Setup abgeschlossen");
}

void loop() {
    canBusUpdate();
    sendHeartbeat();

    encoderManager.update();
    ledController.update();

    CanBusMessage receivedMessage = {};
    while (canBusReceive(receivedMessage)) {
        if (!ledController.handleCanMessage(receivedMessage)) {
            printCanMessage(receivedMessage);
        }
    }

    for (int j = 0; j < 2; j++) {           // Button update
        mcp23s17[j].update();
    }




}
