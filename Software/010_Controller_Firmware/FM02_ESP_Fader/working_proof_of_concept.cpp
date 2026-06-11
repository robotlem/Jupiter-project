#include <Arduino.h>

// Hardware Setup
#define FADER_PIN   4
#define TASTER      39
const int A_IN_P =  1;
const int A_IN_I =  2;
/* PWM */
const int PWM_FREQ = 20000;
const int PWM_RES = 10; // 10 Bit Auflösung (0-1023)
const int PIN_IN1 = 7;  // DRV8833 Input 1
const int PIN_IN2 = 15;  // DRV8833 Input 2
const int PIN_SLP = 40; // DRV Sleep
const int CH_IN1 = 0;
const int CH_IN2 = 1;

// Moving Average Setup
#define MA_LEN      4

int movingAverage[MA_LEN];
int MA_index = 0;

// PID Setup
int target = 50;
double integral = 0;
int lastValue = 0;

double Kp = 29; //30.774;
double Tn = 0.14285714; //0.03;

// Communication
char serial_buffer[20];
int serial_index = 0;



// Fader Lesen
void readCycle() {
    movingAverage[MA_index] = analogRead(FADER_PIN);
    MA_index++;
    if (MA_index >= MA_LEN) MA_index = 0;
}

int getValue() {
    int value = 0;
    for (int i = 0; i < MA_LEN; i++) {
        value += movingAverage[i];
    }
    return value / MA_LEN;
}

void PID_Reset() {
    integral = 0;
}

void update_params() {
    Kp = map(analogRead(A_IN_P), 0, 4095, 0, 100);

    Tn = 1/static_cast<double>(map(analogRead(A_IN_I), 0, 4095, 1, 200));
}
void PID_Regler() {

    update_params();

    static int zeros_counter = 0;
    static unsigned long zeros_timer = millis();

    // Regelabweichung
    int current_value = map(getValue(), 145, 2800, 0, 100);
    double difference = target - current_value;

    // Deadzone um die Zielposition
    int tolerance = 1;
    if (difference <= tolerance && difference >= -tolerance) {
        difference = 0;
        zeros_counter++;
    }
    else {
        zeros_counter = 0;
        zeros_timer = millis();
    }
    // Automatische Abschaltung nach n ms auf eingestelltem Zielwert
    unsigned long zeros_timer_timeout = 100;
    //if (millis() - zeros_timer > zeros_timer_timeout) {
    if (zeros_counter >= 40) {
        digitalWrite(PIN_SLP, LOW);
    }

    // Integral
    static unsigned long lastCall = millis();

    integral = integral + difference * static_cast<double>(millis() - lastCall)*0.001;

    /* Begrenzung des Integrals auf maximalwerte */
    if (integral >= 50) integral = 50;
    if (integral <= -50) integral = -50;

    /* Begrenzung des Wirkungsbereichs */
    if (difference > 20 || difference < -20) {
        integral = 0;
    }

    // Berechnung der Stellgröße u
    int u = static_cast<int> (  Kp*(difference + 1/Tn * integral)  );

    // Add offset for motor deadzone
    int offset = 550;
    u < 0 ? u = u - offset : u = u + offset;

    /* Begrenzung auf einen maximalen Wert */
    int u_max = 1023;
    if (u > u_max) u = u_max;
    else if (u < -u_max) u = -u_max;


    // Ausführung der Motor kontrolle
    if (u < 0) {
        ledcWrite(0, (-u));
        ledcWrite(1, 0);
    }
    else {
        ledcWrite(0, 0);
        ledcWrite(1, (u));
    }


    lastCall = millis();

    static unsigned long lastPrint = 0;

    if (millis() - lastPrint > 50 ) {
        Serial.print("PID Kp: ");
        Serial.print(Kp);
        Serial.print(", Tn: ");
        Serial.print(Tn, 8);
        Serial.print(" Diff: ");
        Serial.print(difference);
        Serial.print(" int: ");
        Serial.print(integral);
        Serial.print(" u: ");
        Serial.println(u);

        lastPrint = millis();
    }

}



void setup() {
    // Hardware Configuration
    pinMode(FADER_PIN, INPUT);
    pinMode(TASTER, INPUT_PULLUP);
    pinMode(A_IN_P, INPUT);
    pinMode(A_IN_I, INPUT);

    pinMode(PIN_IN1, OUTPUT);
    pinMode(PIN_IN2, OUTPUT);

    /* PWM */
    ledcSetup(CH_IN1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_IN2, PWM_FREQ, PWM_RES);
    ledcAttachPin(PIN_IN1, CH_IN1);
    ledcAttachPin(PIN_IN2, CH_IN2);

    pinMode(PIN_SLP, OUTPUT);
    digitalWrite(PIN_SLP, LOW);

    // Communication
    Serial.begin(115200);
    Serial.println("Hello World");
}

void loop() {
    // Fader read update cycle:
    readCycle();
    // PID action
    PID_Regler();

    /*while (Serial.available()) {
     *       char c = Serial.read();
     *       if (serial_index >= 20) return;
     *       serial_buffer[serial_index] = c;
     *       if (c == '\n') process_serial();
     *       else serial_index++;
}*/

    if (Serial.available()) {
        char c = Serial.read();
        target = (c -48)*10;
        digitalWrite(PIN_SLP, HIGH);
        PID_Reset();
    }



}
