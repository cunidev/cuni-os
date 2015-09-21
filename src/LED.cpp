#include "LED.h"

LED::LED(int pin) {
  ledPin = pin;
  pinMode(ledPin,OUTPUT);
}
void LED::tick(int milliseconds, int strength) {
  if(strength > 255) strength = 255;
  analogWrite(ledPin,strength);
  delay(milliseconds);
  analogWrite(ledPin,0);
}
void LED::enable(int strength) {
  if(strength > 255) strength = 255;
  analogWrite(ledPin,strength);
}
void LED::disable() {
  analogWrite(ledPin,0);
}

