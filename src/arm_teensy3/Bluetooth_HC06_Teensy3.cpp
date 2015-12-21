#include "Bluetooth_HC06_Teensy3.h"
#define BT_SERIAL Serial2

Bluetooth::Bluetooth(int speed) {
  baudRate = speed;
  BT_SERIAL.begin(speed);
}
boolean Bluetooth::isReady() {
  for(int x = 0; x < 5000; x++) {
    if(x % 1000 == 0) 
      BT_SERIAL.write("AT");
    if(BT_SERIAL.available() && BT_SERIAL.read() == 79) {
      return true;
    }
    delay(1); 
  }
  return false;
}
