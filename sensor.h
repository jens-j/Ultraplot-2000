#ifndef sensor_h
#define sensor_h

#include "plotter.h"

// sensor value datatype
typedef struct sVal_s{
  int s0;
  int s1;
} sVal_t;


class Sensor{
	private:
		int s0, s1; // sensor pins
	public:
                Sensor(callback_function_t function);
		int decode(sVal_t);
		sVal_t readSensor();
		int decodeSensor();
};


#endif
