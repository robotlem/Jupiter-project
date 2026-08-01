#include "encoder.h"

#include <stdint.h>

#include "can_bus.h"

EncoderManager::EncoderManager(PioEncoder encoders[], size_t encoderCount, uint16_t canId) {
    this->encoders = encoders;
    this->encoderCount = encoderCount <= MAX_ENCODERS ? encoderCount : MAX_ENCODERS;
    this->canId = canId;
}

void EncoderManager::begin() {
    if (encoders == nullptr) {
        return;
    }

    for (size_t i = 0; i < encoderCount; i++) {
        encoders[i].begin();
        encoders[i].flip();
        lastSentPositions[i] = encoders[i].getCount();
    }

    nextUpdateMs = millis() + UPDATE_INTERVAL_MS;
    initialized = true;
}

void EncoderManager::update() {
    if (!initialized || encoders == nullptr) {
        return;
    }

    const unsigned long now = millis();
    if (static_cast<long>(now - nextUpdateMs) < 0) {
        return;
    }
    nextUpdateMs = now + UPDATE_INTERVAL_MS;

    for (size_t i = 0; i < encoderCount; i++) {
        const int currentPosition = encoders[i].getCount();
        const int pendingDelta = currentPosition - lastSentPositions[i];
        if (pendingDelta == 0) {
            continue;
        }

        const int8_t delta = constrainedDelta(pendingDelta);
        CanBusMessage canMessage = {};
        canMessage.id = canId;
        canMessage.length = 2;
        canMessage.data[0] = static_cast<uint8_t>(i);
        canMessage.data[1] = static_cast<uint8_t>(delta);

        if (canBusTransmit(canMessage)) {
            lastSentPositions[i] += delta;
        }
    }
}

int8_t EncoderManager::constrainedDelta(int delta) {
    if (delta > INT8_MAX) {
        return INT8_MAX;
    }
    if (delta < INT8_MIN) {
        return INT8_MIN;
    }
    return static_cast<int8_t>(delta);
}
