#include "can_bus.h"

#include <Arduino.h>
#include <hardware/clocks.h>
#include <hardware/irq.h>
#include <hardware/pio.h>
#include <hardware/sync.h>
#include <CAN_IDs.h>

#include "config.h"

extern "C" {
#include "can2040.h"
}

namespace {

constexpr uint32_t CAN_PIO_INDEX = 0;

can2040 canBus;
volatile bool messageReceived = false;
volatile CanBusMessage receivedMessage = {};

void can2040Callback(can2040 *, uint32_t notify, can2040_msg *message) {
    if (notify != CAN2040_NOTIFY_RX) {
        return;
    }

    receivedMessage.id = message->id;
    receivedMessage.length = min<uint32_t>(message->dlc, sizeof(receivedMessage.data));
    for (uint8_t i = 0; i < receivedMessage.length; i++) {
        receivedMessage.data[i] = message->data[i];
    }
    messageReceived = true;
}

void pio0IrqHandler() {
    can2040_pio_irq_handler(&canBus);
}

}  // namespace

void canBusBegin() {
    // can2040 controls all four PIO0 state machines directly.
    for (uint sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
        pio_sm_claim(pio0, sm);
    }

    can2040_setup(&canBus, CAN_PIO_INDEX);
    can2040_callback_config(&canBus, can2040Callback);

    irq_set_exclusive_handler(PIO0_IRQ_0, pio0IrqHandler);
    irq_set_priority(PIO0_IRQ_0, 1);
    irq_set_enabled(PIO0_IRQ_0, true);

    can2040_start(
        &canBus,
        clock_get_hz(clk_sys),
        CAN_BITRATE,
        CAN_RX_PIN,
        CAN_TX_PIN
    );
}

bool canBusReceive(CanBusMessage &message) {
    const uint32_t interruptState = save_and_disable_interrupts();
    if (!messageReceived) {
        restore_interrupts(interruptState);
        return false;
    }

    message.id = receivedMessage.id;
    message.length = receivedMessage.length;
    for (uint8_t i = 0; i < message.length; i++) {
        message.data[i] = receivedMessage.data[i];
    }
    messageReceived = false;
    restore_interrupts(interruptState);
    return true;
}

bool canBusTransmit(const CanBusMessage &message) {
    can2040_msg canMessage = {};
    canMessage.id = message.id;
    canMessage.dlc = min<uint8_t>(message.length, sizeof(canMessage.data));
    memcpy(canMessage.data, message.data, canMessage.dlc);

    return can2040_transmit(&canBus, &canMessage) == 0;
}

void sendHeartbeat() {
    static long nextHeartbeat = 0;
    if ( millis() > nextHeartbeat) {
        nextHeartbeat = millis() + HEARTBEAT_INTERVALL;
        struct CanBusMessage heartbeat;
#ifndef SECOND_ENCODER
        heartbeat.id = CID_HEARTBEAT_RPI_ENCODER_0;
#else
        heartbeat.id = CID_HEARTBEAT_RPI_ENCODER_1;
#endif
        heartbeat.length = 0;

        canBusTransmit(heartbeat);
    }
}
