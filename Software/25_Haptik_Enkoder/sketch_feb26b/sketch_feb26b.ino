#include <Wire.h>

#define AS5600_ADDR 0x36
#define ANGLE_REG 0x0E   // Raw angle MSB

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin();  // ESP8266: Wire.begin(D2, D1); falls nötig
  Serial.println("AS5600 Test gestartet...");
}

uint16_t readRawAngle() {
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(ANGLE_REG);
  Wire.endTransmission();

  Wire.requestFrom(AS5600_ADDR, 2);
  if (Wire.available() == 2) {
    uint8_t highByte = Wire.read();
    uint8_t lowByte  = Wire.read();
    return ((highByte << 8) | lowByte) & 0x0FFF; // 12 Bit
  }
  return 0;
}

void loop() {
  uint16_t raw = readRawAngle();
  float angleDeg = raw * 360.0 / 4096.0;

  Serial.print("Raw: ");
  Serial.print(raw);
  Serial.print("  Angle: ");
  Serial.print(angleDeg);
  Serial.println(" deg");

  delay(200);
}