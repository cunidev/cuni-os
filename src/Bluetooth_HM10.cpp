#include "Bluetooth_HM10.h"
#define BT_SERIAL Serial2

HM10::HM10(int speed) {
  baudRate = speed;
  BT_SERIAL.begin(speed);
}
boolean HM10::isReady() {
  for(int x = 0; x < 5000; x++) {
    if(x % 1000 == 0) 
      BT_SERIAL.write("AT\n\t");
    if(BT_SERIAL.available() && BT_SERIAL.read() == 79) {
      return true;
    }
    delay(1); 
  }
  return false;
}
