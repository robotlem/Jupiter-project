#include <Arduino.h>
#include "config.h"
#include <pio_encoder.h>
#include <RotaryEncoder.h>
extern "C"{
    #include "can2040.h"
}

static const uint32_t SYS_CLK_HZ = clock_get_hz(clk_sys);


PioEncoder pio_encoders[] ={        // 7 encoders
    PioEncoder(PIN_ENC0, pio1),
    PioEncoder(PIN_ENC1, pio1),
    PioEncoder(PIN_ENC2, pio1),
    PioEncoder(PIN_ENC3, pio1)
};

RotaryEncoder rotary_encoders[] ={
    RotaryEncoder(PIN_ENC4, PIN_ENC4+1, RotaryEncoder::LatchMode::TWO03),
    RotaryEncoder(PIN_ENC5, PIN_ENC5+1, RotaryEncoder::LatchMode::TWO03),
    RotaryEncoder(PIN_ENC6, PIN_ENC6+1, RotaryEncoder::LatchMode::TWO03),
    RotaryEncoder(PIN_ENC7, PIN_ENC7+1, RotaryEncoder::LatchMode::TWO03),
    RotaryEncoder(PIN_ENC8, PIN_ENC8+1, RotaryEncoder::LatchMode::TWO03),
    RotaryEncoder(PIN_ENC9, PIN_ENC9+1, RotaryEncoder::LatchMode::TWO03)
};

int lastPositions[]    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int currentPositions[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void ISR_rotEnc4() {rotary_encoders[0].tick();}
void ISR_rotEnc5() {rotary_encoders[1].tick();}
void ISR_rotEnc6() {rotary_encoders[2].tick();}
void ISR_rotEnc7() {rotary_encoders[3].tick();}
void ISR_rotEnc8() {rotary_encoders[4].tick();}
void ISR_rotEnc9() {rotary_encoders[5].tick();}

uint16_t updatePositions() {
    uint16_t changed = 0;
    for (int i = 0; i < 4; i ++) {
        lastPositions[i] = currentPositions[i];
        currentPositions[i] = pio_encoders[i].getCount();
        if (currentPositions[i] != lastPositions[i]) {
            changed |= 0b00000001 << i;
        }
    }

    for (int i = 4; i < 10; i ++) {
        lastPositions[i] = currentPositions[i];
        currentPositions[i] = rotary_encoders[i-4].getPosition();
        if (currentPositions[i] != lastPositions[i]) {
            changed |= 1 << i;
        }
    }

    return changed;

}

// --- Globaler Empfangspuffer (ISR-safe) ---
volatile bool     msg_received = false;
volatile uint32_t rx_id;
volatile uint8_t  rx_len;
volatile uint8_t  rx_data[8];

static struct can2040 cbus;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    if (notify == CAN2040_NOTIFY_RX) {
        rx_id  = msg->id;
        rx_len = msg->dlc;
        memcpy((void*)rx_data, msg->data, msg->dlc);
        msg_received = true;
    }
}

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = SYS_CLK_HZ, bitrate = CAN_BITRATE;
    uint32_t gpio_rx = CAN_RX_PIN, gpio_tx = CAN_TX_PIN;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
    irq_set_priority(PIO0_IRQ_0, 1);
    irq_set_enabled(PIO0_IRQ_0, 1);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Hello World");

    // Init encoders

    /* PIO Encoders */
    for (int i = 0; i < 4; i++) {
        pio_encoders[i].begin();
        //pio_encoders[i].flip();
    }
    /* SW Encoders */
    pinMode(PIN_ENC4, INPUT_PULLUP);
    pinMode(PIN_ENC4+1, INPUT_PULLUP);
    pinMode(PIN_ENC5, INPUT_PULLUP);
    pinMode(PIN_ENC5+1, INPUT_PULLUP);
    pinMode(PIN_ENC6, INPUT_PULLUP);
    pinMode(PIN_ENC6+1, INPUT_PULLUP);
    pinMode(PIN_ENC7, INPUT_PULLUP);
    pinMode(PIN_ENC7+1, INPUT_PULLUP);
    pinMode(PIN_ENC8, INPUT_PULLUP);
    pinMode(PIN_ENC8+1, INPUT_PULLUP);
    pinMode(PIN_ENC9, INPUT_PULLUP);
    pinMode(PIN_ENC9+1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC4),   ISR_rotEnc4, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC4+1), ISR_rotEnc4, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC5),   ISR_rotEnc5, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC5+1), ISR_rotEnc5, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC6),   ISR_rotEnc6, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC6+1), ISR_rotEnc6, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC7),   ISR_rotEnc7, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC7+1), ISR_rotEnc7, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC8),   ISR_rotEnc8, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC8+1), ISR_rotEnc8, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC9),   ISR_rotEnc9, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC9+1), ISR_rotEnc9, CHANGE);


    // CAN Setup
    canbus_setup();
    Serial.println("Canbus_setup finished");

}

void loop() {
    static unsigned long lastHeartbeat = 0;

    if (updatePositions() != 0) {
        Serial.print("Positions: ");
        for (int i = 0; i < 10; i++) {
            Serial.print(currentPositions[i]);
            Serial.print(", ");
        }
        Serial.println("");
    }

    if (lastHeartbeat + 2000 < millis() ) {
        Serial.println("Heartbeat");
        //lastHeartbeat = millis();
    }


    // --- Empfangene CAN-Nachrichten ausgeben ---
    if (msg_received) {
        msg_received = false;
        Serial.print("CAN RX  ID: 0x");
        Serial.print(rx_id, HEX);
        Serial.print("  DLC: ");
        Serial.print(rx_len);
        Serial.print("  Data: ");
        for (int i = 0; i < rx_len; i++) {
            if (rx_data[i] < 0x10) Serial.print("0");
            Serial.print(rx_data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    // --- Heartbeat senden alle 2s ---
    if (millis() - lastHeartbeat >= 2000) {
        lastHeartbeat = millis();

        struct can2040_msg tx_msg = {};
        tx_msg.id  = 0x123;
        tx_msg.dlc = 4;
        tx_msg.data[0] = 0xDE;
        tx_msg.data[1] = 0xAD;
        tx_msg.data[2] = 0xBE;
        tx_msg.data[3] = 0xEF;

        int ret = can2040_transmit(&cbus, &tx_msg);
        Serial.print("CAN TX: ");
        Serial.println(ret == 0 ? "OK" : "FEHLER");
    }





}
