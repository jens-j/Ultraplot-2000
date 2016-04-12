#ifndef axis_h
#define axis_h

#include "ultraPlot2000.h"
#include "sensor.h"
#include "stepper.h"
#include "EEPROMlib.h"


#define LOGSIZE  3000
#define FILTER_N 1

// positions of the plotter head 
enum z_position_t  {Z_UP = 0, Z_DOWN = 40, Z_MID = 30, Z_LOW = 36, Z_UNKNOWN = 255};
enum x_direction_t {LEFT, RIGHT, IDLE};
enum rom_address_t {ROM_X = 0, ROM_Y = 2, ROM_Z = 4, ROM_X_LBOUND = 6, ROM_X_RBOUND = 8, ROM_Y_LBOUND = 6, ROM_Y_RBOUND = 8};

typedef struct bounds_s{
  int b0;
  int b1;
}bounds_t;

class X_axis{
  
private:
  //double filterDelay[FILTER_N];
  //int filterCount;
  //int filterIndex;
  double pidOutput; // PWM
  double previousError;
  unsigned long pidTime;
  int logCount;
  //unsigned char pidLog[LOGSIZE]; // in PWM
  //unsigned char vLog[LOGSIZE]; // speed setpoint in mm/s
  //unsigned char rLog[LOGSIZE]; // speed in mm/s
  int sdata0;      // previous sensor data
  int vPosition;   // virtual postition, interface to higher levels
  int rPosition;   // real position in physical steps
  int previousRPosition; // previous real position, used for speed calsulation
  int setPoint;    // in real coordinates
  int traveled;    // number of steps taken in current move
  bounds_t bounds; // in real coordinates
  boolean quick;   // flag to enable quick move
  boolean kickoff; // flags start of move, used to overcome static friction
  x_direction_t direction;
  unsigned long cooldownTime;
  Sensor sensor;
  void initMove(int); 
  void setSpeed();
  
public:
  X_axis();
  void sensorIsr();
  void wdTimerIsr();
  void stepLeft();
  void stepRight();
  int getPosition();
  int getSetPoint();
  int getRealPosition();
  void setPosition(int);
  void quickSetPosition(int);
  void setBounds(bounds_t);
  bounds_t getBounds();
  void initPosition(int); // real coordinates
  void stop();
  x_direction_t getDirection();
  void loadPosition()  {rPosition = romReadInt(ROM_X); };
  void storePosition() {romWriteInt(ROM_X, rPosition); };
  void loadBounds()  {bounds = {romReadInt(ROM_X_LBOUND), romReadInt(ROM_X_LBOUND)}; };
  void storeBounds() {romWriteInt(ROM_X_LBOUND, bounds.b0); 
                      romWriteInt(ROM_X_RBOUND, bounds.b1); 
                     };
  void uploadLog();
  
  int debugCount;
};


class Y_axis{
  
private:
  int position; 
  bounds_t bounds;
  unsigned long cooldownTime;
  Stepper stepper;
  void step_ISR();
  void stepUp(int); // step with specific cooldown
  void stepDown(int);
  
public:
  Y_axis();
  void stepUp();
  void stepDown();
  int getPosition();
  void setPosition(int);
  void setBounds(bounds_t);
  bounds_t getBounds();
  void initPosition(int);
  void loadPosition()  {position = romReadInt(ROM_Y);};
  void storePosition() {romWriteInt(ROM_Y, position);};
  void loadBounds()  {bounds = {romReadInt(ROM_Y_LBOUND), romReadInt(ROM_Y_LBOUND)}; };
  void storeBounds() {romWriteInt(ROM_Y_LBOUND, bounds.b0); 
                      romWriteInt(ROM_Y_RBOUND, bounds.b1); 
                     };
};

class Z_axis{
  
private:
  z_position_t position;
  Stepper stepper;
  void moveRelative(int);
  
public:
  Z_axis();
  z_position_t getPosition();
  void setPosition(z_position_t);
  void initPosition(z_position_t pos) {position = pos;}
  void loadPosition()  {position = (z_position_t) romReadInt(ROM_Z);} // initialize from EEPROM memory
  void storePosition() {romWriteInt(ROM_Z, position);} // save to EEPROM memory
  void storePosition(z_position_t pos) {romWriteInt(ROM_Z, pos);}
};


#endif

