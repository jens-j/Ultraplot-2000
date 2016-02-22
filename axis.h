#ifndef axis_h
#define axis_h

#include "sensor.h"
#include "stepper.h"


// positions of the plotter head 
enum z_position_t  {UP, DOWN, MID, MOVING, UNKNOWN};
enum x_direction_t {LEFT, RIGHT, IDLE};

typedef struct bounds_s{
  int b0;
  int b1;
}bounds_t;

class X_axis{
  int sdata0;
  int vPosition;
  int rPosition;
  int setPoint;    // in real coordinates
  bounds_t bounds; // in real coordinates
  x_direction_t direction;
  unsigned long cooldownTime;
  Sensor sensor;
  void setSpeed();
  
public:
  X_axis();
  void isr();
  void stepLeft();
  void stepRight();
  int getPosition();
  int getRealPosition();
  void setPosition(int);
  void quickSetPosition(int);
  void setBounds(bounds_t);
  bounds_t getBounds();
  void initPosition(int);
  void stop();
  x_direction_t getDirection();
};


class Y_axis{
  int position; 
  bounds_t bounds;
  unsigned long cooldownTime;
  Stepper stepper;
  void step_ISR();
public:
  Y_axis();
  void stepUp();
  void stepDown();
  int getPosition();
  void setPosition(int);
  void setBounds(bounds_t);
  bounds_t getBounds();
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

