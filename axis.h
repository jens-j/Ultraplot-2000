#ifndef axis_h
#define axis_h

#include "sensor.h"
#include "stepper.h"


// positions of the plotter head 
enum z_position_t {
  UP = 0, DOWN = 1, MOVING = 2};
enum x_direction_t {
  LEFT = 0, RIGHT = 1, IDLE = 2};


class X_axis{
  int sdata0;
  int position;
  int setPoint;
  x_direction_t direction;
  Sensor sensor;
  void setSpeed();
  
public:
  X_axis();
  void isr();
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
  void stepUp();
  void stepDown();
  int getPosition();
  void setPosition(int);
};


class Z_axis{
  z_position_t position;
  Stepper stepper;
public:
  Z_axis();
  z_position_t getPosition();
  void setPosition(z_position_t);
};


#endif

