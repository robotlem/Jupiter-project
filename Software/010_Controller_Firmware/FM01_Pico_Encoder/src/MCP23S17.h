//
// Created by robo on 11.06.26.
//

#ifndef FM01_PICO_ENCODER_MCP23S17_H
#define FM01_PICO_ENCODER_MCP23S17_H

#include <Arduino.h>

class MCP23S17 {
public:
    union {
        uint8_t byte[2];
        uint16_t word;
    } pinState;

    MCP23S17();
    void init(uint8_t intPin, uint8_t address, uint16_t opcEvent);
    void setButtonIds(const uint8_t ids[]);
    bool update();

private:
    uint8_t intPin = 5;
    uint8_t address = 0;
    uint8_t ids[16] = {};
    uint16_t opcEvent = 0;
    bool initialized = false;

    uint8_t writeOpcode() const;
    uint8_t readOpcode() const;
    void writeConfiguration();
};


#endif //FM01_PICO_ENCODER_MCP23S17_H
