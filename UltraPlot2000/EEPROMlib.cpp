
#include <EEPROM.h>
#include "EEPROMlib.h"


void romWriteInt(int address, int value){
  
  // ls byte
  EEPROM.write(address, ((int) value) & 0xFF);
  // ms byte
  EEPROM.write(address + 1, (((int) value) >> 8) & 0xFF);  
}


int romReadInt(int address){
  int value;
  
  // ms byte
  value = EEPROM.read(address + 1);
  value <<= 8;
  // ls
  value |=  EEPROM.read(address);
  
  return value;
}
