#include "Config.h"
#if CUNI_OS_PLATFORM_ID == 1
#include "Governor_AVR_Default.h"

//#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00
#define CPU_8MHz        0x01
#define CPU_4MHz        0x02
#define CPU_2MHz        0x03
#define CPU_1MHz        0x04
#define CPU_500kHz      0x05
#define CPU_250kHz      0x06
#define CPU_125kHz      0x07
#define CPU_62kHz       0x08

Governor_AVR::Governor_AVR(CUNI_HW_KEYPAD_NAME &keypad) : _keypad(keypad) {
  
}

void Governor_AVR::sleepUntilInterrupt() {
  //CPU_PRESCALE(CPU_125kHz);
  // TODO: set interrupts
  //CPU_PRESCALE(CPU_8MHz);
}
#endif
