#ifndef FM01_PICO_ENCODER_ENCODER_H
#define FM01_PICO_ENCODER_ENCODER_H

#include <Arduino.h>
#include <pio_encoder.h>

class EncoderManager {
public:
    EncoderManager(PioEncoder encoders[], size_t encoderCount, uint16_t canId);

    void begin();
    void update();

private:
    static constexpr size_t MAX_ENCODERS = 8;
    static constexpr unsigned long UPDATE_INTERVAL_MS = 50;

    PioEncoder *encoders = nullptr;
    size_t encoderCount = 0;
    uint16_t canId = 0;
    int lastSentPositions[MAX_ENCODERS] = {};
    unsigned long nextUpdateMs = 0;
    bool initialized = false;

    static int8_t constrainedDelta(int delta);
};

#endif //FM01_PICO_ENCODER_ENCODER_H
