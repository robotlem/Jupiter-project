
#include <Arduino.h>
#include <SimpleFOC.h>

#define PID_vel_P
#define PID_vel_I
#define PID_vel_D


BLDCMotor motor = BLDCMotor(7);
//BLDCDriver3PWM driver = BLDCDriver3PWM(19, 18, 5, 17);
//MagneticSensorI2C sensor = MagneticSensorI2C(0x36, 12, 0x0E, 4);

BLDCDriver3PWM driver = BLDCDriver3PWM(14, 13, 12, 11);
MagneticSensorI2C sensor = MagneticSensorI2C(0x36, 12, 0x0E, 4);

void setup() {
  //Wire.begin(21, 22); 
  Wire.begin(41, 42); 
  Wire.setClock(400000);

  Serial.begin(115200);
  SimpleFOCDebug::enable(&Serial);

  // initialize sensor hardware
  sensor.init();
  motor.linkSensor(&sensor);

  // driver config
  driver.voltage_power_supply = 12;
  driver.init();
  motor.linkDriver(&driver);

  // set control loop type to be used
  motor.controller = MotionControlType::torque;
  motor.torque_controller = TorqueControlType::voltage;

  // comment out if not needed
  //motor.useMonitoring(Serial);
  motor.monitor_downsample = 50; // disable intially
  motor.monitor_variables = _MON_TARGET | _MON_VEL | _MON_ANGLE; // monitor target velocity and angle
  motor.voltage_limit = 6;
  motor.velocity_limit = 10;
  motor.P_angle.P = 2;   // kleiner = weicher

  motor.PID_velocity.P = 0.15;
  motor.PID_velocity.I = 0.5;
  motor.PID_velocity.D = 0.0;

  motor.LPF_velocity.Tf = 0.05;

  // initialise motor
  motor.init();

  // align encoder and start FOC
  motor.initFOC();
  motor.target = motor.shaft_angle;


  _delay(1000);
}

void loop() {
  // iterative setting FOC phase voltage
  motor.loopFOC();

  float angle = motor.shaft_angle;
  float velocity = motor.shaft_velocity;

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
  


  torque = constrain(torque, -motor.voltage_limit, motor.voltage_limit);
  motor.move(torque);


}