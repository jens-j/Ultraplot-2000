#ifndef axis_h
#define axis_h

#include "sensor.h"
#include "stepper.h"


// positions of the plotter head 
enum z_position_t  {UP, DOWN, MID, MOVING, UNKNOWN};
enum x_direction_t {LEFT, RIGHT, IDLE};


class X_axis{
  int sdata0;
  int position;
  int setPoint;
  x_direction_t direction;
  int leftBound;
  int rightBound;
  Sensor sensor;
  void setSpeed();
  unsigned long stall_time;
  
public:
  X_axis();
  void isr();
  void stepLeft();
  void stepRight();
  int getPosition();
  void setPosition(int);
  void quickSetPosition(int);
  void setBounds(int, int);
  void initPosition(int);
};


class Y_axis{
  int position;
  int upperBound;
  int lowerBound;
  Stepper stepper;
  void step_ISR();
public:
  Y_axis();
  void stepUp();
  void stepDown();
  int getPosition();
  void setPosition(int);
  void setBounds(int, int);
  void initPosition(int);
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

