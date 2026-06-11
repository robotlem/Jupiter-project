#define SERVO_PWM         12
#define RELAY             11
#define LED_BUILDIN       13
#define A_IN              0
#define BTN               2
#define NEO_PIN           3

#define BTN_PRESSED       HIGH
#define A_THRESH          600
#define SERVO_TIMEOUT_MS  1000

#define POS_RELEASED      95
#define POS_PRESSED       75

#include <FastLED.h>
#include <Servo.h>

const CRGB COL_Standby = CRGB(255,  0,  0);
const CRGB COL_Pressed = CRGB(255,  0,255);
const CRGB COL_Running = CRGB(  0,  0,255);

CRGB led;
Servo myservo;
unsigned long long lastServoAction = millis();


typedef enum state{
  Standby,
  Pressed,
  Running
}state;

state currentState = Standby;

void servoRelease(){
  myservo.attach(SERVO_PWM);
  myservo.write(POS_RELEASED);
  lastServoAction = millis();
}
void servoPress(){
  myservo.attach(SERVO_PWM);
  myservo.write(POS_PRESSED);
  lastServoAction = millis();
}
void relayOpen(){
  digitalWrite(RELAY, LOW);
  digitalWrite(LED_BUILDIN, LOW);
}
void relayClose(){
  digitalWrite(RELAY, HIGH);
  digitalWrite(LED_BUILDIN, HIGH);
}

void setNeo(CRGB color){
  if (led == color) return;
  led = color;
  FastLED.show();
}



void setup() {
  pinMode(RELAY, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);

  FastLED.addLeds<WS2812B, NEO_PIN, GRB>(&led, 1);
  myservo.attach(SERVO_PWM);
  myservo.write(POS_RELEASED);
  lastServoAction = millis();

}

void loop() {
  if (digitalRead(BTN) == BTN_PRESSED && currentState != Pressed){
    currentState = Pressed;
    servoPress();
  }
  else if (digitalRead(BTN) != BTN_PRESSED && currentState == Pressed){
    servoRelease();
    currentState = Standby;
  } 
  if (currentState != Pressed && analogRead(A_IN) < A_THRESH){
    currentState = Standby;
  }
  if (currentState != Pressed && analogRead(A_IN) >= A_THRESH){
    currentState = Running;
  }



  switch(currentState){
    case Pressed:
      setNeo(COL_Pressed);
      break;
    case Running:
      setNeo(COL_Running);
      relayClose();
      break;
    case Standby:
    default:
      setNeo(COL_Standby);
      relayOpen();
      break;
  }

  if(myservo.attached() && millis() > lastServoAction + SERVO_TIMEOUT_MS && currentState != Pressed){
    myservo.detach();
  }

}
