//
// Created by robo on 31.05.26.
//

#ifndef PM02_ESP_HAPTIC_ENCODER_PINS_H
#define PM02_ESP_HAPTIC_ENCODER_PINS_H

#include <Arduino.h>
const uint8_t DRV0_IN1        = 16;
const uint8_t DRV0_IN2        = 15;
const uint8_t DRV0_IN3        = 7;
const uint8_t DRV0_EN         = 6;
const uint8_t DRV0_SLEEP      = 4;
const uint8_t DRV0_FAULT      = 5;

const uint8_t DRV1_IN1        = 14;
const uint8_t DRV1_IN2        = 13;
const uint8_t DRV1_IN3        = 12;
const uint8_t DRV1_EN         = 11;
const uint8_t DRV1_SLEEP      = 9;
const uint8_t DRV1_FAULT      = 10;

const uint8_t I2C0_SDA        = 40;
const uint8_t I2C0_SCL        = 39;
const uint8_t I2C1_SDA        = 41;
const uint8_t I2C1_SCL        = 42;

const uint8_t LED0_OUT        = 47;
const uint8_t LED1_OUT        = 21;

const gpio_num_t CAN_RX       = GPIO_NUM_18;
const gpio_num_t CAN_TX       = GPIO_NUM_17;

#endif //PM02_ESP_HAPTIC_ENCODER_PINS_H