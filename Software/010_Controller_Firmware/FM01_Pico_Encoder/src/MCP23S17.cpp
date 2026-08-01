//
// Created by robo on 11.06.26.
//

#include "MCP23S17.h"

#include <SPI.h>

#include "can_bus.h"
#include "config.h"

namespace {

constexpr uint32_t SPI_FREQUENCY = 8000000;
const SPISettings MCP_SPI_SETTINGS(SPI_FREQUENCY, MSBFIRST, SPI_MODE0);

constexpr uint8_t OPCODE_BASE = 0x40;
constexpr uint8_t REGISTER_IODIRA = 0x00;
constexpr uint8_t REGISTER_IOCON = 0x0A;
constexpr uint8_t REGISTER_GPIOA = 0x12;

constexpr uint8_t IOCON_MIRROR = 1u << 6;
constexpr uint8_t IOCON_HAEN = 1u << 3;
constexpr uint8_t IOCON_INTPOL = 1u << 1;
constexpr uint8_t IOCON_CONFIGURATION = IOCON_MIRROR | IOCON_HAEN | IOCON_INTPOL;

bool spiInitialized = false;
bool hardwareAddressingEnabled = false;

void selectChip() {
    digitalWrite(PIN_SPI_CS, LOW);
}

void deselectChip() {
    digitalWrite(PIN_SPI_CS, HIGH);
}

void initializeSpi() {
    if (spiInitialized) {
        return;
    }

    pinMode(PIN_SPI_CS, OUTPUT);
    deselectChip();

    SPI.setMISO(PIN_SPI_MISO);
    SPI.setMOSI(PIN_SPI_MOSI);
    SPI.setSCK(PIN_SPI_SCK);
    SPI.begin();

    spiInitialized = true;
}

void writeRegister(uint8_t opcode, uint8_t reg, uint8_t value) {
    SPI.beginTransaction(MCP_SPI_SETTINGS);
    selectChip();
    SPI.transfer(opcode);
    SPI.transfer(reg);
    SPI.transfer(value);
    deselectChip();
    SPI.endTransaction();
}

void enableHardwareAddressing() {
    if (hardwareAddressingEnabled) {
        return;
    }

    // Directly after reset all devices respond to address 0 until HAEN is set.
    writeRegister(OPCODE_BASE, REGISTER_IOCON, IOCON_CONFIGURATION);
    hardwareAddressingEnabled = true;
}

}  // namespace

MCP23S17::MCP23S17() {
    pinState.word = 0xFFFF;
}

void MCP23S17::init(uint8_t intPin, uint8_t address, uint16_t opcEvent) {
    this->intPin = intPin;
    this->address = address < 8 ? address : 0;
    this->opcEvent = opcEvent;

    initializeSpi();
    enableHardwareAddressing();

    pinMode(intPin, INPUT_PULLDOWN);
    writeConfiguration();

    initialized = true;
    if (digitalRead(intPin) != LOW) {
        readPinState();
    }
}

void MCP23S17::setButtonIds(const uint8_t ids[]) {
    for (int i = 0; i < 16; i++) {
        this->ids[i] = ids[i];
    }
}

bool MCP23S17::update() {
    if (!initialized) {
        return false;
    }

    const bool pinsUpdated = digitalRead(intPin) != LOW;
    if (pinsUpdated) {
        readPinState();
    }

    return sendButtonEvents() || pinsUpdated;
}

uint8_t MCP23S17::writeOpcode() const {
    return OPCODE_BASE | (address << 1);
}

uint8_t MCP23S17::readOpcode() const {
    return writeOpcode() | 0x01;
}

void MCP23S17::readPinState() {
    SPI.beginTransaction(MCP_SPI_SETTINGS);
    selectChip();
    SPI.transfer(readOpcode());
    SPI.transfer(REGISTER_GPIOA);
    pinState.byte[0] = SPI.transfer(0x00);
    pinState.byte[1] = SPI.transfer(0x00);
    deselectChip();
    SPI.endTransaction();
}

void MCP23S17::writeConfiguration() {
    static constexpr uint8_t configuration[] = {
        0xFF, 0xFF,  // IODIRA/B: all inputs
        0x00, 0x00,  // IPOLA/B: no inversion
        0xFF, 0xFF,  // GPINTENA/B: interrupts enabled
        0xFF, 0xFF,  // DEFVALA/B: default high
        0x00, 0x00,  // INTCONA/B: interrupt on change
        IOCON_CONFIGURATION, IOCON_CONFIGURATION,
        0xFF, 0xFF,  // GPPUA/B: pull-ups enabled
    };

    SPI.beginTransaction(MCP_SPI_SETTINGS);
    selectChip();
    SPI.transfer(writeOpcode());
    SPI.transfer(REGISTER_IODIRA);
    for (uint8_t value : configuration) {
        SPI.transfer(value);
    }
    deselectChip();
    SPI.endTransaction();
}

bool MCP23S17::sendButtonEvents() {
    bool eventAccepted = false;

    for (uint8_t i = 0; i < 16; i++) {
        const uint16_t bitMask = 1u << i;
        if ((pinState.word & bitMask) == (lastPinState & bitMask)) {
            continue;
        }

        CanBusMessage canMessage = {};
        canMessage.id = opcEvent;
        canMessage.length = 1;
        if ((pinState.word & bitMask) != 0) {
            canMessage.data[0] = ids[i];
        } else {
            canMessage.data[0] = ids[i] | 0x80;
        }

        if (!canBusTransmit(canMessage)) {
            break;
        }

        lastPinState = (lastPinState & ~bitMask) | (pinState.word & bitMask);
        eventAccepted = true;
    }

    return eventAccepted;
}
