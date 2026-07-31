# FM01 Pico Encoder

Firmware für den Raspberry Pi Pico 2 mit RP2350. Das Projekt liest fünf
Quadratur-Encoder über PIO ein, kommuniziert über einen mit `can2040`
implementierten CAN-Bus und steuert eine Kette kompatibler NeoPixel-LEDs.

## Projektaufbau

- `src/main.cpp`: Initialisierung und Anwendungslogik
- `src/config.h`: Pinbelegung und grundlegende Konfiguration
- `src/can_bus.h/.cpp`: CAN-Schnittstelle, Empfangspuffer und PIO0-IRQ
- `src/can2040.c/.h`: Low-Level-CAN-Implementierung
- `lib/rp2040-encoder-library`: angepasste PIO-Encoder-Library für RP2350

## Build

Das Projekt verwendet PlatformIO:

```bash
pio run -e pico2
```

Die erzeugte Firmware liegt unter:

```text
.pio/build/pico2/firmware.uf2
```

## Pinbelegung

| Funktion | GPIO |
|---|---:|
| Encoder 0 A/B | 6 / 7 |
| Encoder 1 A/B | 8 / 9 |
| Encoder 2 A/B | 10 / 11 |
| Encoder 3 A/B | 12 / 13 |
| Encoder 4 A/B | 14 / 15 |
| CAN TX | 16 |
| CAN RX | 17 |
| NeoPixel-Daten | 2 |
| SPI SCK | 18 |
| SPI MOSI | 19 |
| SPI MISO | 20 |
| SPI CS | 21 |
| MCP0 Interrupt | 5 |
| MCP1 Interrupt | 4 |

Die beiden Signale eines Encoders müssen auf aufeinanderfolgenden GPIOs
liegen. Der in `PioEncoder` angegebene Pin ist jeweils der A-Pin.

## PIO-Belegung

Der RP2350 besitzt drei PIO-Blöcke mit jeweils vier State Machines.

| PIO | State Machine | Verwendung |
|---|---|---|
| PIO0 | SM0 | CAN Synchronisation |
| PIO0 | SM1 | CAN Empfang |
| PIO0 | SM2 | CAN Bitmustervergleich |
| PIO0 | SM3 | CAN Senden |
| PIO1 | SM0 | Encoder 0 |
| PIO1 | SM1 | Encoder 1 |
| PIO1 | SM2 | Encoder 2 |
| PIO1 | SM3 | Encoder 3 |
| PIO2 | SM0 | Encoder 4 |
| PIO2 | SM1 | NeoPixel/FastLED |
| PIO2 | SM2 | frei |
| PIO2 | SM3 | frei |

### Warum FastLED auf PIO2/SM1 liegt

Die Encoder werden zuerst initialisiert. Dadurch sind alle State Machines von
PIO1 und SM0 von PIO2 belegt. Anschließend reserviert das CAN-Modul alle vier
State Machines von PIO0. FastLED sucht danach in der Reihenfolge PIO0, PIO1,
PIO2 nach einer freien State Machine und erhält PIO2/SM1.

FastLED benötigt für die gesamte LED-Kette nur eine State Machine und einen
DMA-Kanal. Die Anzahl der LEDs erhöht die Anzahl benötigter State Machines
nicht.

### PIO-Programmspeicher

Jeder PIO-Block besitzt 32 Instruktionsplätze:

- Das Encoderprogramm benötigt 24 Instruktionen und wird pro PIO nur einmal
  geladen.
- Das FastLED-NeoPixel-Programm benötigt auf PIO2 zusätzlich 4 Instruktionen.
- PIO2 verwendet damit 28 von 32 Instruktionsplätzen.

Auf PIO2 bleiben zwei State Machines frei, aber nur vier freie
Instruktionsplätze. Ein weiteres PIO-Programm muss daher sowohl in die freien
State Machines als auch in den verbleibenden Programmspeicher passen.

## CAN-Schnittstelle

Das CAN-Modul stellt drei Funktionen bereit:

```cpp
void canBusBegin();
bool canBusReceive(CanBusMessage &message);
bool canBusTransmit(const CanBusMessage &message);
```

`canBusReceive()` liefert die zuletzt im IRQ empfangene Nachricht. Wenn vor dem
Auslesen mehrere Nachrichten eintreffen, wird die vorherige Nachricht
überschrieben; es gibt aktuell keine Software-Warteschlange.

## MCP23S17

Die Klasse `Mcp23s17` konfiguriert einen SPI-Portexpander mit 16 Eingängen,
internen Pull-ups und Interrupt bei Eingangsänderung:

```cpp
#include "mcp23s17.h"

Mcp23s17 mcp0;
Mcp23s17 mcp1;

void setup() {
    mcp0.init(0, PIN_MCP0_INT);
    mcp1.init(1, PIN_MCP1_INT);
}

void loop() {
    const uint16_t inputs0 = mcp0.update();
    const uint16_t inputs1 = mcp1.update();
}
```

Die Adresse entspricht dem Zustand der Hardware-Adresspins `A2:A0` und muss
zwischen `0` und `7` liegen. Alle MCP23S17 teilen sich den in `config.h`
definierten Chip-Select-Pin; die Auswahl erfolgt über die aktivierte
Hardware-Adressdekodierung.

Der Rückgabewert von `update()` enthält `GPIOA` in Bit 0 bis 7 und `GPIOB` in
Bit 8 bis 15. Wegen der Pull-ups ist ein offener Eingang `1` und ein gegen
Masse geschalteter Eingang `0`.
