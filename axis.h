#ifndef axis_h
#define axis_h

#include "sensor.h"
#include "stepper.h"


// positions of the plotter head 
enum position_t {UP = 0, DOWN = 1, MOVING = 2};


class X_axis{
		int position;
		Sensor sensor;
	public:
		X_axis();
		void stepLeft();
		void stepRight();
		int getPosition();
		void setPosition(int);
};


class Y_axis{
		int position;
		Stepper stepper;
                void step_ISR();
	public:
		Y_axis();
		void stepLeft();
		void stepRight();
		int getPosition();
		void setPosition(int);
};


class Z_axis{
		position_t position;
		Stepper stepper;
	public:
		Z_axis();
		position_t getPosition();
		void setPosition(position_t);
};


#endif
