#include <Arduino.h>
#include "config.h"
#include <pio_encoder.h>
extern "C"{
    #include "can2040.h"
}

static const uint32_t SYSTEM_CLOCK_HZ = clock_get_hz(clk_sys);


PioEncoder pio_encoders[] = {
    PioEncoder(PIN_ENC0, pio1),
    PioEncoder(PIN_ENC1, pio1),
    PioEncoder(PIN_ENC2, pio1),
    PioEncoder(PIN_ENC3, pio1),
    PioEncoder(PIN_ENC4, pio2),
};

constexpr size_t ENCODER_COUNT = sizeof(pio_encoders) / sizeof(pio_encoders[0]);

int lastPositions[ENCODER_COUNT] = {};
int currentPositions[ENCODER_COUNT] = {};

uint16_t updatePositions() {        // Updates encoder Positions; returns changed Encoder (Bit Shifted)
    uint16_t changed = 0;
    for (size_t i = 0; i < ENCODER_COUNT; i++) {
        lastPositions[i] = currentPositions[i];
        currentPositions[i] = pio_encoders[i].getCount();
        if (currentPositions[i] != lastPositions[i]) {
            changed |= 1u << i;
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
    uint32_t sys_clock = SYSTEM_CLOCK_HZ, bitrate = CAN_BITRATE;
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
    for (size_t i = 0; i < ENCODER_COUNT; i++) {
        pio_encoders[i].begin();
        //pio_encoders[i].flip();
    }

    // CAN Setup
    canbus_setup();
    Serial.println("Canbus_setup finished");

}

void loop() {
    static unsigned long lastHeartbeat = 0;

    if (updatePositions() != 0) {
        Serial.print("Positions: ");
        for (size_t i = 0; i < ENCODER_COUNT; i++) {
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
