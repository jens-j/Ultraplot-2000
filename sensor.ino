#include "plotter.h"
#include "sensor.h"

Sensor::Sensor(){};

// convert the sensor reading to an integer value
int Sensor::decode(sVal_t data){
  if(     data.s0 == 0 and data.s1 == 0)
    return 0;
  else if(data.s0 == 0 and data.s1 == 1)
    return 1;
  else if(data.s0 == 1 and data.s1 == 1)
    return 2;
  else
    return 3;
}

// read the sensor 
sVal_t Sensor::readSensor(){
  return {digitalRead(SENSOR_X0), digitalRead(SENSOR_X1)};
}

// read the sensor and return the decoded value
int Sensor::decodeSensor(){
  return Sensor::decode(readSensor());
}
