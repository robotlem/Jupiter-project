
#include <Arduino.h>
#include <SimpleFOC.h>


// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(7);
BLDCDriver3PWM driver = BLDCDriver3PWM(19, 18, 5, 17);

// encoder instance
// MagneticSensorI2C(uint8_t _chip_address, float _cpr, uint8_t _angle_register_msb)
//  chip_address         - I2C chip address
//  bit_resolution       - resolution of the sensor
//  angle_register_msb   - angle read register msb
//  bits_used_msb        - number of used bits in msb register
MagneticSensorI2C sensor = MagneticSensorI2C(0x36, 12, 0x0E, 4);


// commander communication instance
Commander command = Commander(Serial);
// void doMotion(char* cmd){ command.motion(&motor, cmd); }
void doMotor(char* cmd){ command.motor(&motor, cmd); }


// custom PID controller instance for the custom control method
// P controller with gain of 1.0f, no integral or derivative gain
PIDController custom_PID = PIDController(1.0f, 0, 0);

// optional add the PID to command to be able to tune it in runtime
void doPID(char* cmd){ command.pid(&custom_PID, cmd); }

void setup() {
  Wire.begin(21, 22); 
  Wire.setClock(400000);
  // use monitoring with serial 
  Serial.begin(115200);
  // enable more verbose output for debugging
  // comment out if not needed
  SimpleFOCDebug::enable(&Serial);

  // initialize sensor hardware
  sensor.init();
  motor.linkSensor(&sensor);

  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  driver.init();
  // link driver
  motor.linkDriver(&driver);

  // set control loop type to be used
  motor.controller = MotionControlType::torque;
  motor.torque_controller = TorqueControlType::voltage;

  // comment out if not needed
  //motor.useMonitoring(Serial);
  motor.monitor_downsample = 50; // disable intially
  motor.monitor_variables = _MON_TARGET | _MON_VEL | _MON_ANGLE; // monitor target velocity and angle
  motor.voltage_limit = 3;
  motor.velocity_limit = 10;
  motor.P_angle.P = 2;   // kleiner = weicher

  motor.PID_velocity.P = 0.15;
  motor.PID_velocity.I = 0.5;
  motor.PID_velocity.D = 0.0;

  motor.LPF_velocity.Tf = 0.05;

  // subscribe motor to the commander
  //command.add('T', doMotion, "motion control"); // a bit less resouce intensive
  command.add('M', doMotor, "motor");
  command.add('C', doPID, "custom PID");


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
  torque = - error * 10.0;

  // leichte Dämpfung
  torque -= velocity * 0.02;

  motor.move(torque);

  // iterative function setting the outter loop target
  //motor.move();

  // // motor monitoring
  motor.monitor();

  // user communication
  command.run();
}