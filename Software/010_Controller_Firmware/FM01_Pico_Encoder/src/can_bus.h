#ifndef CAN_BUS_H
#define CAN_BUS_H

#include <stdint.h>

struct CanBusMessage {
    uint32_t id;
    uint8_t length;
    uint8_t data[8];
};

void canBusBegin();
void canBusUpdate();
bool canBusReceive(CanBusMessage &message);
bool canBusTransmit(const CanBusMessage &message);
void sendHeartbeat();

#endif
