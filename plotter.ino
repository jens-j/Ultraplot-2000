#include "plotter.h"
#include "axis.h"

X_axis x_axis = X_axis();
Y_axis y_axis = Y_axis();
Z_axis z_axis = Z_axis();


int digitalPinToInterrupt(int pin){
  switch(pin){
    case 2:
      return 0;
    case 3:
      return 1;
    case 21:
      return 2;
    case 20:
      return 3;
    case 19:
      return 4;
    case 18:
      return 5;
    default:
      return -1;  
  }
}

void isrDispatcher(){
  x_axis.isr();
}

void setup(){
  pinMode(MOTOR_X0, OUTPUT);
  pinMode(MOTOR_X1, OUTPUT);
  pinMode(MOTOR_Y0, OUTPUT);
  pinMode(MOTOR_Y1, OUTPUT);
  pinMode(MOTOR_Y2, OUTPUT);
  pinMode(MOTOR_Y3, OUTPUT);
  pinMode(MOTOR_Z0, OUTPUT);
  pinMode(MOTOR_Z1, OUTPUT);
  pinMode(MOTOR_Z2, OUTPUT);
  pinMode(MOTOR_Z3, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(SENSOR_X0), isrDispatcher, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SENSOR_X1), isrDispatcher, CHANGE);
  Serial.begin(115200);
}


void loop(){
  delay(100);
  z_axis.setPosition(DOWN);
  delay(100);
  z_axis.setPosition(UP);


  y_axis.setPosition(4000);
  delay(100);
  y_axis.setPosition(0);
  delay(100);

  x_axis.setPosition(6000);
  x_axis.setPosition(0);
}
