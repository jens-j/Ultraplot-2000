#include "plotter.h"
#include "sensor.h"

// constructor
Sensor::Sensor(callback_function_t function){
	s0 = SENSOR_X0;
	s1 = SENSOR_X1;
	attachInterrupt(SENSOR_X0, isr, CHANGE);
	attachInterrupt(SENSOR_X1, isr, CHANGE);
}

// convert the sensor reading to an integer value
static int Sensor::decode(sVal_t){
	if(		data.s0 == 0 and data.s1 == 0)
 		return 0;
 	else if(data.s0 == 0 and data.s1 == 1)
 		return 1;
 	else if(data.s0 == 1 and data.s1 == 0)
 		return 2;
 	else
 		return 3;
}

// read the sensor 
sVal_t Sensor::readSensor(){
	return {digitalRead(s0), digitalRead(s1)};
}

// read the sensor and return the decoded value
sVal_t Sensor::decodeSensor(){
	return Sensor::decode(readSensor());
}