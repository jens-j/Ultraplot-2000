#ifndef sensor_h
#define sensor_h

#include "ultraPlot2000.h"

// sensor value datatype
typedef struct sVal_s{
  int s0;
  int s1;
} sVal_t;


class Sensor{
  private:
  public:
    Sensor();
    void isr();
    //void setCallback(callback_function_t function);
    //void removeCallback();
    int getDirection();
    int decode(sVal_t);
    sVal_t readSensor();
    int decodeSensor();
};


#endif
