#ifndef FM01_PICO_ENCODER_LED_CONTROLLER_H
#define FM01_PICO_ENCODER_LED_CONTROLLER_H

#include <Arduino.h>
#include <FastLED.h>

#include "can_bus.h"
#include "config.h"

class LedController {
public:
    LedController(CRGB leds[], uint8_t ledCount);

    void begin();
    void update();
    bool handleCanMessage(const CanBusMessage &message);

private:
    static constexpr unsigned long BLINK_RENDER_INTERVAL_MS = 20;

    CRGB *leds = nullptr;
    uint8_t ledCount = 0;
    CRGB pendingColors[NUM_LEDS] = {};
    CRGB activeColors[NUM_LEDS] = {};
    CRGB renderedColors[NUM_LEDS] = {};
    uint8_t pendingBlinkModes[NUM_LEDS] = {};
    uint8_t activeBlinkModes[NUM_LEDS] = {};
    unsigned long blinkStartMs = 0;
    unsigned long warningBlinkStartMs = 0;
    unsigned long nextBlinkRenderMs = 0;
    unsigned long lastHostHeartbeatMs = 0;
    bool hostOffline = false;

    void setIndividual(const CanBusMessage &message);
    void setIndividualBlinkMode(const CanBusMessage &message);
    void setLed(uint16_t ledId, uint8_t colorIntens);
    void setLedBlinkMode(uint16_t ledId, uint8_t blinkMode);
    void commitPendingState();
    void handleHostHeartbeat();
    void updateHostHeartbeatState(unsigned long now);
    bool renderOutputState(unsigned long now);
    CRGB renderLed(uint8_t localIndex, unsigned long now) const;
    CRGB renderWarningLed(uint8_t localIndex, unsigned long now) const;
    static uint8_t blinkScale(uint8_t blinkMode, unsigned long elapsedMs);
    static unsigned long blinkInterval(uint8_t blinkMode);
    static unsigned long fadeInterval(uint8_t blinkMode);
    static CRGB decodeColor(uint8_t colorIntens);
    static CRGB scaleColor(const CRGB &color, uint8_t scale);
    static bool mapLedId(uint16_t ledId, uint8_t &localIndex);
};

#endif //FM01_PICO_ENCODER_LED_CONTROLLER_H
