#include <Arduino.h>
#include <FastLED.h>
#include <SimpleFOC.h>
//#include "driver/twai.h"
#include "pins.h"

#define NUM_LEDS 16

BLDCMotor motor0 = BLDCMotor(7);
BLDCDriver3PWM driver0 = BLDCDriver3PWM(DRV0_IN1, DRV0_IN2, DRV0_IN3, DRV0_EN);
MagneticSensorI2C sensor0 = MagneticSensorI2C(0x36, 12, 0x0E, 4);

BLDCMotor motor1 = BLDCMotor(7);
BLDCDriver3PWM driver1 = BLDCDriver3PWM(DRV1_IN1, DRV1_IN2, DRV1_IN3, DRV1_EN);
MagneticSensorI2C sensor1 = MagneticSensorI2C(0x36, 12, 0x0E, 4);

TwoWire I2C_0 = TwoWire(0);
TwoWire I2C_1 = TwoWire(1);

CRGB leds0[NUM_LEDS];
CRGB leds1[NUM_LEDS];

void setup() {
// write your initialization code here
    Serial0.begin(115200);

    FastLED.addLeds<NEOPIXEL, LED0_OUT>(leds0, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, LED1_OUT>(leds1, NUM_LEDS);

    for (int i = 0; i < NUM_LEDS; i++) {
        leds0[i] = CRGB::Green;
        leds1[i] = CRGB::Blue;

    }
    FastLED.setBrightness(255);

    FastLED.show();


    // twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX, CAN_RX, TWAI_MODE_NORMAL);
    // twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();  // Baudrate anpassen
    // twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
    //     Serial0.println("TWAI install failed!");
    //     return;
    // }
    // if (twai_start() != ESP_OK) {
    //     Serial0.println("TWAI start failed!");
    //     return;
    // }
    // Serial0.println("CAN ready!");

    I2C_0.begin(I2C0_SDA, I2C0_SCL, 100000);
    //I2C_1.begin(I2C1_SDA, I2C1_SCL, 100000);

    for(uint8_t addr=1; addr<127; addr++) {
        I2C_0.beginTransmission(addr);
        uint8_t err = I2C_0.endTransmission();

        if(err == 0) {
            Serial.printf("Found I2C device: 0x%02X\n", addr);
        }
    }

    SimpleFOCDebug::enable(&Serial0);

    sensor0.init(&I2C_0);
    motor0.linkSensor(&sensor0);
    //sensor1.init(&I2C_1);
    //motor1.linkSensor(&sensor1);

    driver0.voltage_power_supply = 12;
    driver0.init();
    motor0.linkDriver(&driver0);
    // driver1.voltage_power_supply = 12;
    // driver1.init();
    // motor1.linkDriver(&driver1);

    motor0.controller = MotionControlType::torque;
    motor0.torque_controller = TorqueControlType::voltage;
    // motor1.controller = MotionControlType::torque;
    // motor1.torque_controller = TorqueControlType::voltage;

    motor0.monitor_downsample = 50; // disable intially
    motor0.monitor_variables = _MON_TARGET | _MON_VEL | _MON_ANGLE; // monitor target velocity and angle
    motor0.voltage_limit = 6;
    motor0.velocity_limit = 10;
    motor0.P_angle.P = 2;   // kleiner = weicher
    // motor1.monitor_downsample = 50; // disable intially
    // motor1.monitor_variables = _MON_TARGET | _MON_VEL | _MON_ANGLE; // monitor target velocity and angle
    // motor1.voltage_limit = 6;
    // motor1.velocity_limit = 10;
    // motor1.P_angle.P = 2;   // kleiner = weicher

    motor0.PID_velocity.P = 0.15;
    motor0.PID_velocity.I = 0.5;
    motor0.PID_velocity.D = 0.0;
    motor0.LPF_velocity.Tf = 0.05;
    // motor1.PID_velocity.P = 0.15;
    // motor1.PID_velocity.I = 0.5;
    // motor1.PID_velocity.D = 0.0;
    // motor1.LPF_velocity.Tf = 0.05;


    // initialise motor
    // if (!motor0.initFOC()) {
    //     Serial0.println("motor0 FOC init FAILED");
    //     while(1);
    // }
    // if (!motor1.initFOC()) {
    //     Serial0.println("motor1 FOC init FAILED");
    //     while(1);
    // }

    // align encoder and start FOC
    motor0.initFOC();
    motor0.target = motor0.shaft_angle;
    // motor1.initFOC();
    // motor1.target = motor1.shaft_angle;



}

void loop() {
// write your code here


    motor0.loopFOC();

    float angle = motor0.shaft_angle;
    float velocity = motor0.shaft_velocity;

    float torque = 0;

    // Beispiel: 24 Rastungen
    int teeth = 24;
    float step = _2PI / teeth;
    float nearest = round(angle / step) * step;
    float error = angle - nearest;
    torque = - error*error * 120.0;
    if (error < 0) torque = -torque;

    // leichte Dämpfung
    torque -= velocity * 0.02;

    // virtuelle Endanschläge
    float limit = 2 * _2PI;
    float wall_stiffness = 40.0;

    if (angle > limit) {
        torque = -(angle - limit) * wall_stiffness;
    }

    if (angle < -limit) {
        torque = -(angle + limit) * wall_stiffness;
    }



    torque = constrain(torque, -motor0.voltage_limit, motor0.voltage_limit);
    motor0.move(torque);

    //
    //
    // {
    //     motor1.loopFOC();
    //
    //     float angle = motor1.shaft_angle;
    //     float velocity = motor1.shaft_velocity;
    //
    //     float torque = 0;
    //
    //     // Beispiel: 24 Rastungen
    //     int teeth = 24;
    //     float step = _2PI / teeth;
    //     float nearest = round(angle / step) * step;
    //     float error = angle - nearest;
    //     torque = - error*error * 120.0;
    //     if (error < 0) torque = -torque;
    //
    //     // leichte Dämpfung
    //     torque -= velocity * 0.02;
    //
    //     // virtuelle Endanschläge
    //     float limit = 2 * _2PI;
    //     float wall_stiffness = 40.0;
    //
    //     if (angle > limit) {
    //         torque = -(angle - limit) * wall_stiffness;
    //     }
    //
    //     if (angle < -limit) {
    //         torque = -(angle + limit) * wall_stiffness;
    //     }
    //
    //
    //
    //     torque = constrain(torque, -motor1.voltage_limit, motor1.voltage_limit);
    //     motor1.move(torque);
    // }
}