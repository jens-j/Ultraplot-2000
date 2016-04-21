#include "Arduino.h"
#include <EEPROM.h>
#include "EEPROMlib.h"


void romWriteInt(int address, int value){

  // ls byte
  EEPROM.write(2*address, ((int) value) & 0xFF);
  // ms byte
  EEPROM.write(2*address + 1, (((int) value) >> 8) & 0xFF);  
}


int romReadInt(int address){
  int value;

  // ms byte
  value = EEPROM.read(2*address + 1);
  value <<= 8;
  // ls
  value |=  EEPROM.read(2*address);

  return value;
}
