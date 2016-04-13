#ifndef EEPROMLIB_h
#define EEPROMLIB_h


void romWriteInt(int, int); // These functions accees the memory with a stride of 2 bytes
int romReadInt(int);        // Addresses are therefore integer addresses


#endif
