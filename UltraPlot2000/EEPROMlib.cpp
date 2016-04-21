#include "Arduino.h"
#include <EEPROM.h>
#include "EEPROMlib.h"


void romWriteInt(int address, int value){
  //char buffer[100];

  //sprintf(buffer, "EEPROM write: [%04X] = %04X", address, value);
  //Serial.println(buffer);
  
  // ls byte
  EEPROM.write(2*address, ((int) value) & 0xFF);
  // ms byte
  EEPROM.write(2*address + 1, (((int) value) >> 8) & 0xFF);  
}


int romReadInt(int address){
  int value;
  //char buffer[100];
  
  // ms byte
  value = EEPROM.read(2*address + 1);
  value <<= 8;
  // ls
  value |=  EEPROM.read(2*address);
  
  //sprintf(buffer, "EEPROM read:  [%04X] = %04X", address, value);
  //Serial.println(buffer);
  
  return value;
}
