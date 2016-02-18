#include "plotter.h"
#include "axis.h"
#include "sensor.h"
#include "stepper.h"
#include "TimerOne.h"


/*********************************************************/
/*	X axis 			    
/*********************************************************/
X_axis::X_axis() : sensor() {
  sdata0 = 0;
  position = 0;
  setPoint = 0;
  direction = IDLE;
  leftBound = -4000;
  rightBound = 4000;
  stall_time = micros();
}

void X_axis::isr(){
  int sdata = sensor.decodeSensor();
  
  if(sdata - sdata0 == 1 || sdata - sdata0 == -3)
    position++;
  else
    position--; 
    
  if(position < leftBound){
    panic("x < SW bound");
  }
  if(position > rightBound){
    panic("x > SW bound");
  }  
    
  setSpeed();
  
  sdata0 = sdata;  
}

void X_axis::setSpeed(){
  int speed;
  int diff = abs(setPoint - position);
  
  if(diff < 10)
    speed = 75;
  else if(diff < 25)
    speed = 80;
  else if(diff < 100)
    speed = 90;
  else
    speed = 100;
   
   if(direction == LEFT and position > setPoint){
     analogWrite(MOTOR_X0, speed);
   }
   else if(direction == RIGHT and position < setPoint){
     analogWrite(MOTOR_X1, speed);
   }
   else{
     analogWrite(MOTOR_X0, 0);
     analogWrite(MOTOR_X1, 0);
     if(direction == IDLE){
       //Serial.println("overshot");
     }
     else{
       stall_time = micros();
       direction = IDLE;
     }
   }
 }

void X_axis::stepLeft(){
  setPosition(setPoint-1);
}

void X_axis::stepRight(){
  setPosition(setPoint+1);
}

int X_axis::getPosition(){
  return position;
}

void X_axis::setPosition(int setp){
  
  while(direction != IDLE || (micros() - stall_time) < X_COOLDOWN){
    delayMicroseconds(1);
  }
  quickSetPosition(setp);
}

void X_axis::quickSetPosition(int setp){
  setPoint = setp;
   
  direction = IDLE;
  if(position > setPoint){
    direction = LEFT;
    setSpeed();
  }
  else if(position < setPoint){
    direction = RIGHT;
    setSpeed(); 
  } 
}

void X_axis::setBounds(int left, int right){
  leftBound = left;
  rightBound = right;  
//  sprintf(cBuffer, "(lb,rb) = (%d,%d)", leftBound, rightBound);
//  Serial.println(cBuffer);
}

void X_axis::initPosition(int pos){
  position = pos; 
}

/*********************************************************/
/*	Y axis 			    
/*********************************************************/
Y_axis::Y_axis() : 
stepper(MOTOR_Y0, MOTOR_Y1, MOTOR_Y2, MOTOR_Y3, Y_STEPPER_PWM)
{
  position = 0;
  upperBound = 6000;
  lowerBound = -6000;
}

void Y_axis::stepDown(){
  if(--position < lowerBound){
    panic("y < SW bound"); 
  }
  if(digitalRead(BUTTON_Y1) == LOW){
    panic("y < HW bound");
  }
  stepper.stepRight();
  delayMicroseconds(800);
}

void Y_axis::stepUp(){
  if(++position > upperBound){
    panic("y > SW bound"); 
  }
  if(digitalRead(BUTTON_Y0) == LOW){
      panic("y > HW bound");
  }
  stepper.stepLeft();
  delayMicroseconds(800);
}

int Y_axis::getPosition(){
  return position;
}

void Y_axis::setPosition(int setPoint){
  while(setPoint < position){
    stepDown();
  }
  while(setPoint > position){
    stepUp(); 
  }
}

void Y_axis::setBounds(int lower, int upper){
  upperBound = upper;
  lowerBound = lower;  
}

void Y_axis::initPosition(int pos){
  position = pos; 
}


/*********************************************************/
/*	Z axis 			    
/*********************************************************/
Z_axis::Z_axis() : 
stepper(MOTOR_Z0, MOTOR_Z1, MOTOR_Z2, MOTOR_Z3, 255) 
{
  position = UNKNOWN;
}

z_position_t Z_axis::getPosition(){
  return position;
}

void Z_axis::setPosition(z_position_t setPoint){
  int i, step;

  if(position == setPoint || position == MOVING)
    return;

  position = MOVING;

  for(i = 0; i < 40 * N_PHASE ; i++){
    if(setPoint == UP){
      stepper.stepLeft();
      delayMicroseconds(800);
    }
    else{
      stepper.stepRight();
      delayMicroseconds(800);		
    }
  }

  position = setPoint;
}

