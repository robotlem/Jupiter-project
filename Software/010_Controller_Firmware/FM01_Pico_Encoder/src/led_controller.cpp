#include "led_controller.h"

#include <CAN_IDs.h>

#include "config.h"

LedController::LedController(CRGB leds[], uint8_t ledCount) {
    this->leds = leds;
    this->ledCount = ledCount <= NUM_LEDS ? ledCount : NUM_LEDS;
}

void LedController::begin() {
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, ledCount);
    FastLED.setBrightness(255);
    fill_solid(pendingColors, ledCount, CRGB::Black);
    fill_solid(activeColors, ledCount, CRGB::Black);
    fill_solid(renderedColors, ledCount, CRGB::Black);
    fill_solid(leds, ledCount, CRGB::Black);
    blinkStartMs = millis();
    nextBlinkRenderMs = blinkStartMs;
    FastLED.show();
}

void LedController::update() {
    const unsigned long now = millis();
    if (static_cast<long>(now - nextBlinkRenderMs) < 0) {
        return;
    }

    nextBlinkRenderMs = now + BLINK_RENDER_INTERVAL_MS;
    if (renderActiveState(now)) {
        FastLED.show();
    }
}

bool LedController::handleCanMessage(const CanBusMessage &message) {
    switch (message.id) {
        case CID_LED_SET_INDIVIDUAL:
            setIndividual(message);
            return true;
        case CID_LED_SET_INDI_BLINKMODE:
            setIndividualBlinkMode(message);
            return true;
        case CID_LED_SYNC_BLINKMODE:
            blinkStartMs = millis();
            nextBlinkRenderMs = blinkStartMs;
            if (renderActiveState(blinkStartMs)) {
                FastLED.show();
            }
            return true;
        case CID_LED_UPDATE:
            commitPendingState();
            return true;
        default:
            return false;
    }
}

void LedController::setIndividual(const CanBusMessage &message) {
    if (message.length < 2) {
        return;
    }

    const uint8_t startLedId = message.data[0];
    for (uint8_t i = 1; i < message.length; i++) {
        setLed(static_cast<uint16_t>(startLedId) + i - 1, message.data[i]);
    }
}

void LedController::setIndividualBlinkMode(const CanBusMessage &message) {
    if (message.length < 2) {
        return;
    }

    const uint8_t startLedId = message.data[0];
    for (uint8_t i = 1; i < message.length; i++) {
        setLedBlinkMode(static_cast<uint16_t>(startLedId) + i - 1, message.data[i]);
    }
}

void LedController::setLed(uint16_t ledId, uint8_t colorIntens) {
    uint8_t localIndex = 0;
    if (!mapLedId(ledId, localIndex) || localIndex >= ledCount) {
        return;
    }

    pendingColors[localIndex] = decodeColor(colorIntens);
}

void LedController::setLedBlinkMode(uint16_t ledId, uint8_t blinkMode) {
    uint8_t localIndex = 0;
    if (!mapLedId(ledId, localIndex) || localIndex >= ledCount) {
        return;
    }

    pendingBlinkModes[localIndex] = blinkMode;
}

void LedController::commitPendingState() {
    if (leds == nullptr) {
        return;
    }

    for (uint8_t i = 0; i < ledCount; i++) {
        activeColors[i] = pendingColors[i];
        activeBlinkModes[i] = pendingBlinkModes[i];
    }

    renderActiveState(millis());
    FastLED.show();
}

bool LedController::renderActiveState(unsigned long now) {
    if (leds == nullptr) {
        return false;
    }

    bool changed = false;
    for (uint8_t i = 0; i < ledCount; i++) {
        const CRGB rendered = renderLed(i, now);
        if (rendered != renderedColors[i]) {
            renderedColors[i] = rendered;
            leds[i] = rendered;
            changed = true;
        }
    }

    return changed;
}

CRGB LedController::renderLed(uint8_t localIndex, unsigned long now) const {
    const uint8_t scale = blinkScale(activeBlinkModes[localIndex], now - blinkStartMs);
    return scaleColor(activeColors[localIndex], scale);
}

uint8_t LedController::blinkScale(uint8_t blinkMode, unsigned long elapsedMs) {
    if ((blinkMode & 0x03) == LED_BLINKMODE_STATIC) {
        return 255;
    }

    const unsigned long interval = blinkInterval(blinkMode);
    const unsigned long cycleTime = interval * 2;
    const unsigned long cyclePosition = elapsedMs % cycleTime;
    const bool onPhase = cyclePosition < interval;

    if (onPhase) {
        if ((blinkMode & LED_BLINKMODE_FADE_IN) == 0) {
            return 255;
        }

        const unsigned long fadePosition = min(cyclePosition, fadeInterval(blinkMode));
        return static_cast<uint8_t>((fadePosition * 255) / fadeInterval(blinkMode));
    }

    if ((blinkMode & LED_BLINKMODE_FADE_OUT) == 0) {
        return 0;
    }

    const unsigned long offPosition = cyclePosition - interval;
    const unsigned long fadePosition = min(offPosition, fadeInterval(blinkMode));
    return static_cast<uint8_t>(255 - ((fadePosition * 255) / fadeInterval(blinkMode)));
}

unsigned long LedController::blinkInterval(uint8_t blinkMode) {
    switch (blinkMode & 0x03) {
        case LED_BLINKMODE_SLOW:
            return BLINKMODE_INTERVAL_SLOW;
        case LED_BLINKMODE_MID:
            return BLINKMODE_INTERVAL_MID;
        case LED_BLINKMODE_FAST:
            return BLINKMODE_INTERVAL_FAST;
        default:
            return BLINKMODE_INTERVAL_SLOW;
    }
}

unsigned long LedController::fadeInterval(uint8_t blinkMode) {
    switch (blinkMode & 0x03) {
        case LED_BLINKMODE_SLOW:
            return BLINKMODE_INTERVAL_FADE_SLOW;
        case LED_BLINKMODE_MID:
            return BLINKMODE_INTERVAL_FADE_MID;
        case LED_BLINKMODE_FAST:
            return BLINKMODE_INTERVAL_FADE_FAST;
        default:
            return BLINKMODE_INTERVAL_FADE_SLOW;
    }
}

CRGB LedController::decodeColor(uint8_t colorIntens) {
    const uint8_t intensity = colorIntens >> 5;
    const uint8_t colorId = colorIntens & 0x1F;
    const RgbColor baseColor = led_colors[colorId];

    return CRGB(
        static_cast<uint8_t>((static_cast<uint16_t>(baseColor.r) * intensity + 3) / 7),
        static_cast<uint8_t>((static_cast<uint16_t>(baseColor.g) * intensity + 3) / 7),
        static_cast<uint8_t>((static_cast<uint16_t>(baseColor.b) * intensity + 3) / 7)
    );
}

CRGB LedController::scaleColor(const CRGB &color, uint8_t scale) {
    return CRGB(
        static_cast<uint8_t>((static_cast<uint16_t>(color.r) * scale + 127) / 255),
        static_cast<uint8_t>((static_cast<uint16_t>(color.g) * scale + 127) / 255),
        static_cast<uint8_t>((static_cast<uint16_t>(color.b) * scale + 127) / 255)
    );
}

bool LedController::mapLedId(uint16_t ledId, uint8_t &localIndex) {
    if (ledId < LED_OFFSET || ledId > LED_MAX_ID) {
        return false;
    }

    const uint8_t mappingIndex = ledId - LED_OFFSET;
    localIndex = LED_MAPPING[mappingIndex];
    return localIndex != 255;
}
