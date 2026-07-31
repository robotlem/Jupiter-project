# Jupiter Controller Firmware

Firmware-Sammlung fuer ein modulares Lichtpult, das ETC Eos auf einem Mac mini
bedient. Die Bedienhardware wie Taster, Displays, Fader und Drehencoder ist als
verteiltes System ueber einen gemeinsamen CAN-Bus angebunden. Der Mac mini
haengt ueber einen USB-CAN-Adapter ebenfalls am Bus und uebernimmt spaeter die
Uebersetzung zwischen Hardwareereignissen und Eos.

Der aktuelle Stand ist ein PlatformIO-Workspace mit mehreren separaten
Firmware-Projekten. Die Firmware fuer die einzelnen Module liegt jeweils in
einem eigenen Unterordner; gemeinsame CAN-IDs liegen in `shared/sharedCommon`.

## Projektstruktur

| Ordner | Status | Zweck |
| --- | --- | --- |
| `FM01_Pico_Encoder` | aktiv | Raspberry Pi Pico 2 / RP2350 fuer fuenf Quadratur-Encoder, zwei MCP23S17-Tasterexpander, SK6812/NeoPixel-Test und CAN ueber `can2040`. |
| `FM02_ESP_Fader` | Prototyp | ESP32-S3-Firmware fuer Motorfader mit analoger Positionsmessung, Moving Average und PI/PID-artiger Motorregelung ueber DRV8833. Noch ohne CAN-Anbindung. |
| `FM03_ESP_Display` | aktiv/prototypisch | ESP32-S3-Displaymodul mit 800x480 RGB-Panel, LovyanGFX, LVGL und CAN-Textupdates ueber ESP-IDF-TWAI. |
| `PM02_ESP_Haptic_Encoder` | Prototyp | ESP32-S3-Firmware fuer haptische Encoder mit SimpleFOC, AS5600-Magnetsensor, BLDC-Treiber und LED-Test. CAN ist vorbereitet, aber auskommentiert. |
| `shared/sharedCommon` | aktiv | PlatformIO-Library mit zentralen CAN-Konstanten in `CAN_IDs.h`. |
| `ZZZZ_legacy` | Altstand | Aeltere Tests, Proof-of-Concepts und historische Kopien. Nicht als aktuelle Quelle behandeln. |

## Toolchain

Alle aktiven Projekte verwenden PlatformIO mit Arduino-Framework.

```bash
pio run -d FM01_Pico_Encoder -e encoder_1
pio run -d FM01_Pico_Encoder -e encoder_2
pio run -d FM02_ESP_Fader
pio run -d FM03_ESP_Display -e display_1
pio run -d FM03_ESP_Display -e display_2
pio run -d PM02_ESP_Haptic_Encoder
```

Upload und serieller Monitor laufen entsprechend ueber PlatformIO:

```bash
pio run -d FM01_Pico_Encoder -e encoder_1 -t upload
pio device monitor -d FM01_Pico_Encoder -b 115200
```

Die ESP32-S3-Projekte aktivieren USB-CDC im Build, daher erscheint die serielle
Konsole typischerweise direkt ueber den USB-Port.

## CAN-Bus

Die zentrale Protokolldatei ist:

```text
shared/sharedCommon/src/CAN_IDs.h
```

Aktuelle Busparameter:

| Parameter | Wert |
| --- | --- |
| CAN-Frameformat | Standard-ID, 11 Bit |
| Bitrate | 500 kbit/s |
| Heartbeat-Intervall | 3000 ms |
| Heartbeat-Timeout | 5000 ms |

Die kleinere CAN-ID hat auf dem Bus hoehere Prioritaet.

### Button-Events

| CAN-ID | Bedeutung |
| --- | --- |
| `0x200` bis `0x20F` | `CID_BUTTON_EVENT_GRP0` bis `CID_BUTTON_EVENT_GRP15` |

Laut Kommentar wird als Payload eine Buttonnummer erwartet; bei gedruecktem
Button ist das MSB gesetzt. In `FM01_Pico_Encoder` werden die beiden MCP23S17
bereits gelesen, Button-Events werden aber im aktuellen Code noch nicht auf den
CAN-Bus gesendet.

### Display-Kommandos

| CAN-ID | Name | Payload |
| --- | --- | --- |
| `0x501` | `CID_DISPLAY_SET_TEXT_HEADER` | `Display_ID`, `Text_ID`, `Text_Length` |
| `0x500` | `CID_DISPLAY_SET_TEXT_PAYLOAD` | bis zu 8 Bytes UTF-8-Text |
| `0x502` | `CID_DISPLAY_SET_BOX_COLOR` | `Display_ID`, `Box_ID`, `R`, `G`, `B` |
| `0x503` | `CID_DISPLAY_UPDATE` | `Display_ID`; `255` aktualisiert alle Displays |

`FM03_ESP_Display` implementiert Text-Header, Payload und Update. Die
Textdaten werden in einem lokalen Buffer gesammelt und erst mit
`CID_DISPLAY_UPDATE` sichtbar gemacht. `CID_DISPLAY_SET_BOX_COLOR` ist im Code
als Case vorhanden, aber noch nicht implementiert.

Die Display-UI ist in fuenf Sektionen mit jeweils 16 Label-IDs organisiert.
Eine `Text_ID` besteht praktisch aus `section * 16 + labelIndex`, gueltig von
`0x00` bis `0x4F`.

### Heartbeats

| CAN-ID | Modul |
| --- | --- |
| `0x700` | Mac / Host |
| `0x701` | RP Pico Encoder 0 |
| `0x702` | RP Pico Encoder 1 |
| `0x703` | ESP Fader 0 |
| `0x704` | ESP Fader 1 |
| `0x705` | ESP Display 0 |
| `0x706` | ESP Display 1 |
| `0x707` | ESP Keyboard |
| `0x708` bis `0x70A` | ESP Haptic 0 bis 2 |

## Modulstatus

### FM01 Pico Encoder

Das Pico-Modul ist aktuell der am weitesten integrierte Firmwarestand:

- Zielplattform: Raspberry Pi Pico 2 (`rpipico2`) mit Earle Philhower Arduino-Core.
- Zwei Build-Varianten: `encoder_1` und `encoder_2`; `encoder_2` setzt `SECOND_ENCODER`.
- Fuenf Quadratur-Encoder werden ueber PIO gelesen.
- CAN laeuft ueber `can2040` auf PIO0 mit 500 kbit/s.
- Heartbeat sendet je nach Build-Variante `0x701` oder `0x702`.
- Zwei MCP23S17 an SPI sind initialisiert, mit Hardware-Adressierung `0x00` und `0x01`.
- 32 SK6812/NeoPixel-LEDs werden aktuell mit Testfarben gesetzt.

Bekannte Einschraenkungen:

- Encoderpositionsaenderungen werden bisher nur seriell ausgegeben, nicht als CAN-Event gesendet.
- MCP23S17-Zustandsaenderungen werden seriell ausgegeben, aber noch nicht in `CID_BUTTON_EVENT_*` Frames umgesetzt.
- `src/config.h` enthaelt noch `CAN_BUS_BITRATE = 125000`; verwendet wird aber `CAN_BITRATE = 500000` aus `sharedCommon`.
- Der CAN-Empfang puffert nur die zuletzt empfangene Nachricht. Wenn mehrere Frames vor dem Auslesen eintreffen, kann eine aeltere Nachricht ueberschrieben werden.

Weitere Details stehen in `FM01_Pico_Encoder/README.md`. Dort ist der Build-
Abschnitt allerdings aelter als die aktuelle `platformio.ini`; die aktuellen
Environments heissen `encoder_1` und `encoder_2`.

### FM02 ESP Fader

Der Fader-Code ist ein lokaler Motorregelungs-Prototyp:

- Zielplattform: ESP32-S3 DevKitC-1 mit 32 MB Flash / 16 MB PSRAM-Konfiguration.
- Faderposition wird analog gelesen und ueber einen Moving Average geglaettet.
- Zielposition kommt momentan ueber einzelne serielle Zeichen.
- Motoransteuerung erfolgt per PWM ueber DRV8833.
- Reglerparameter `Kp` und `Tn` werden ueber zwei analoge Eingange gelesen.
- CAN, Heartbeat und Eos-Integration sind noch nicht vorhanden.

### FM03 ESP Display

Das Displaymodul verbindet LVGL-UI mit CAN-Kommandos:

- Zielplattform: ESP32-S3 DevKitC-1 mit 4 MB Flash / 8 MB OPI-PSRAM-Konfiguration.
- Display: 800x480 RGB-Panel ueber LovyanGFX.
- UI: LVGL 8.3, fuenf Fader-/Button-Sektionen mit insgesamt 80 Labels.
- CAN: ESP32-TWAI auf `GPIO17` TX und `GPIO18` RX, 500 kbit/s.
- Zwei Build-Varianten: `display_1` und `display_2`; `display_2` setzt `SECOND_DISPLAY`.
- Heartbeat sendet je nach Build-Variante `0x705` oder `0x706`.

Bekannte Einschraenkungen:

- Farbupdates fuer Boxen sind noch nicht implementiert.
- Touch ist im Displaytreiber vorbereitet, aber aktuell nicht als Eingabe in die Anwendung integriert.
- Das Display akzeptiert Textpayloads nur nach passendem Header fuer die eigene `DISPLAY_ID`.

### PM02 ESP Haptic Encoder

Der haptische Encoder ist derzeit ein SimpleFOC-Versuchsstand:

- Zielplattform: ESP32-S3 DevKitC-1 N32R8V.
- Zwei Motor-/LED-Kanaele sind im Code angelegt, aktiv genutzt wird aktuell nur Motor 0.
- AS5600-Magnetsensor wird ueber I2C gesucht und fuer Motor 0 initialisiert.
- SimpleFOC arbeitet in Torque/Voltage-Control.
- Der Code erzeugt eine virtuelle Rastung mit 24 Zaehnen, Daempfung und virtuellen Endanschlaegen.
- CAN-Pins sind in `pins.h` definiert und TWAI-Code ist als Kommentar vorhanden, aber nicht aktiv.

## Offene naechste Schritte

- Einheitliches Eventformat fuer Encoder, Fader und haptische Encoder definieren.
- Button-Mapping der MCP23S17-Eingaenge als CAN-Frames senden.
- Host-Software fuer den Mac mini dokumentieren oder in dieses Repository aufnehmen.
- Fader-Firmware an CAN anbinden: Positions-Istwert, Zielwert, Touch/Taster und Heartbeat.
- Display-Farbkommandos (`CID_DISPLAY_SET_BOX_COLOR`) implementieren.
- CAN-Empfang auf Modulen mit hoeherer Last auf Queue/Ringbuffer erweitern.
- Legacy-Ordner nach Bedarf archivieren oder aus dem aktiven Workspace entfernen.

