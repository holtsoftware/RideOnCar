#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 16
#define NEOPIXEL_COUNT 1

#define AIN_FWD  29   // analog input: forward
#define AIN_REV  27   // analog input: reverse
#define PWM_FWD   0   // PWM output: forward
#define PWM_REV   1   // PWM output: reverse

// Analog input thresholds (0–1023)
#define AIN_THRESH  205   // 20% — below this = off; at this = 50% out
#define AIN_FULL    767   // 75% — at or above = 100% out

Adafruit_NeoPixel led(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Serial control mode: 'F'=forward, 'R'=reverse, 'S'=stop, 0=analog
char serialCmd = 0;

void setLed(uint8_t r, uint8_t g, uint8_t b) {
  led.setPixelColor(0, led.Color(r, g, b));
  led.show();
}

// Maps analog input to PWM output with floor/ceiling curve:
//   <20%  → 0    (off)
//   20%   → 127  (50%)
//   75%+  → 255  (100%)
int mapAnalog(int value) {
  if (value < AIN_THRESH) return 0;
  if (value >= AIN_FULL)  return 255;
  return map(value, AIN_THRESH, AIN_FULL, 127, 255);
}

void setup() {
  Serial.begin(115200);
  led.begin();
  led.setBrightness(50);
  setLed(255, 165, 0);  // yellow during boot

  pinMode(PWM_FWD, OUTPUT);
  pinMode(PWM_REV, OUTPUT);
  pinMode(AIN_FWD, INPUT_PULLDOWN);
  pinMode(AIN_REV, INPUT_PULLDOWN);

  setLed(0, 0, 255);  // blue when boot complete
  Serial.println("Ready. Commands: F=forward, R=reverse, S=stop, A=analog mode");
}

void loop() {
  // Handle incoming serial commands
  if (Serial.available()) {
    char c = toupper((char)Serial.read());
    switch (c) {
      case 'F':
        serialCmd = 'F';
        Serial.println("Serial: FORWARD");
        break;
      case 'R':
        serialCmd = 'R';
        Serial.println("Serial: REVERSE");
        break;
      case 'S':
        serialCmd = 'S';
        Serial.println("Serial: STOP");
        break;
      case 'A':
        serialCmd = 0;
        Serial.println("Serial: ANALOG mode");
        break;
    }
  }

  if (serialCmd != 0) {
    // Serial override — only one output high at a time
    if (serialCmd == 'F') {
      analogWrite(PWM_REV, 0);
      analogWrite(PWM_FWD, 255);
      setLed(0, 255, 0);    // green
    } else if (serialCmd == 'R') {
      analogWrite(PWM_FWD, 0);
      analogWrite(PWM_REV, 255);
      setLed(255, 60, 0);   // orange
    } else {
      analogWrite(PWM_FWD, 0);
      analogWrite(PWM_REV, 0);
      setLed(0, 0, 255);    // blue
    }
  } else {
    // Analog mode
    int fwd = analogRead(AIN_FWD);
    int rev = analogRead(AIN_REV);
    int fwdOut = mapAnalog(fwd);
    int revOut = mapAnalog(rev);

    Serial.print("A29="); Serial.print(fwd);
    Serial.print(" A27="); Serial.println(rev);

    if (fwdOut > 0 && revOut > 0) {
      // Both active — forward wins, reverse always cleared first
      analogWrite(PWM_REV, 0);
      analogWrite(PWM_FWD, fwdOut);
      setLed(128, 0, 128);  // purple
    } else if (fwdOut > 0) {
      analogWrite(PWM_REV, 0);
      analogWrite(PWM_FWD, fwdOut);
      setLed(0, 255, 0);    // green
    } else if (revOut > 0) {
      analogWrite(PWM_FWD, 0);
      analogWrite(PWM_REV, revOut);
      setLed(255, 60, 0);   // orange
    } else {
      analogWrite(PWM_FWD, 0);
      analogWrite(PWM_REV, 0);
      setLed(0, 0, 255);    // blue (idle)
    }
  }
}
