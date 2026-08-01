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
constexpr uint8_t CAN_RX_QUEUE_SIZE = 16;
constexpr uint8_t CAN_TX_QUEUE_SIZE = 32;

can2040 canBus;

CanBusMessage rxQueue[CAN_RX_QUEUE_SIZE];
volatile uint8_t rxHead = 0;
volatile uint8_t rxTail = 0;
volatile uint8_t rxCount = 0;
volatile uint32_t rxDropped = 0;

CanBusMessage txQueue[CAN_TX_QUEUE_SIZE];
uint8_t txHead = 0;
uint8_t txTail = 0;
uint8_t txCount = 0;
uint32_t txDropped = 0;

uint8_t nextQueueIndex(uint8_t index, uint8_t size) {
    index++;
    return index >= size ? 0 : index;
}

CanBusMessage normalizedMessage(const CanBusMessage &message) {
    CanBusMessage normalized = message;
    normalized.length = min<uint8_t>(message.length, sizeof(normalized.data));
    return normalized;
}

void enqueueRxFromIrq(const CanBusMessage &message) {
    if (rxCount >= CAN_RX_QUEUE_SIZE) {
        rxDropped++;
        return;
    }

    rxQueue[rxHead] = message;
    rxHead = nextQueueIndex(rxHead, CAN_RX_QUEUE_SIZE);
    rxCount++;
}

bool enqueueTx(const CanBusMessage &message) {
    if (txCount >= CAN_TX_QUEUE_SIZE) {
        txDropped++;
        return false;
    }

    txQueue[txHead] = normalizedMessage(message);
    txHead = nextQueueIndex(txHead, CAN_TX_QUEUE_SIZE);
    txCount++;
    return true;
}

bool transmitNow(const CanBusMessage &message) {
    can2040_msg canMessage = {};
    canMessage.id = message.id;
    canMessage.dlc = min<uint8_t>(message.length, sizeof(canMessage.data));
    memcpy(canMessage.data, message.data, canMessage.dlc);

    return can2040_transmit(&canBus, &canMessage) == 0;
}

void drainTxQueue() {
    while (txCount > 0) {
        if (!transmitNow(txQueue[txTail])) {
            return;
        }

        txTail = nextQueueIndex(txTail, CAN_TX_QUEUE_SIZE);
        txCount--;
    }
}

void can2040Callback(can2040 *, uint32_t notify, can2040_msg *message) {
    if (notify != CAN2040_NOTIFY_RX) {
        return;
    }

    CanBusMessage receivedMessage = {};
    receivedMessage.id = message->id;
    receivedMessage.length = min<uint32_t>(message->dlc, sizeof(receivedMessage.data));
    for (uint8_t i = 0; i < receivedMessage.length; i++) {
        receivedMessage.data[i] = message->data[i];
    }
    enqueueRxFromIrq(receivedMessage);
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

void canBusUpdate() {
    drainTxQueue();
}

bool canBusReceive(CanBusMessage &message) {
    canBusUpdate();

    const uint32_t interruptState = save_and_disable_interrupts();
    if (rxCount == 0) {
        restore_interrupts(interruptState);
        return false;
    }

    message = rxQueue[rxTail];
    rxTail = nextQueueIndex(rxTail, CAN_RX_QUEUE_SIZE);
    rxCount--;
    restore_interrupts(interruptState);
    return true;
}

bool canBusTransmit(const CanBusMessage &message) {
    canBusUpdate();

    if (txCount == 0 && transmitNow(message)) {
        return true;
    }

    const bool queued = enqueueTx(message);
    canBusUpdate();
    return queued;
}

void sendHeartbeat() {
    canBusUpdate();

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
