#include "Arduino.h"
#include "ultraPlot2000.h"
#include "axis.h"
#include "sensor.h"
#include "stepper.h"
#include "TimerOne.h"


/*********************************************************/
/*	X axis 			    
/*********************************************************/
X_axis::X_axis() : sensor() {
  sdata0 = 0;
  vPosition = 0;
  rPosition = 0;
  setPoint = 0;
  direction = IDLE;
  bounds = {-4000, 4000};
  cooldownTime = micros();
}

void X_axis::isr(){
  //char cBuffer[100];
  int sdata = sensor.decodeSensor();
  
  if(sdata - sdata0 == 1 || sdata - sdata0 == -3)
    rPosition++;
  else
    rPosition--; 
    
  if(rPosition < bounds.b0){
    panic("x < SW bound");
  }
  if(rPosition > bounds.b1){
    panic("x > SW bound");
  }  
    
  setSpeed();
  
  //sprintf(cBuffer, "x = %d", rPosition);
  //Serial.println(cBuffer);
  
  sdata0 = sdata;  
}

void X_axis::setSpeed(){
  int speed;
  int diff = abs(setPoint - rPosition);
  
  if(diff < 10)
    speed = 75;
  else if(diff < 25)
    speed = 80;
  else if(diff < 100)
    speed = 90;
  else
    speed = 100;
   
   if(direction == LEFT and rPosition > setPoint){
     analogWrite(MOTOR_X0, speed);
   }
   else if(direction == RIGHT and rPosition < setPoint){
     analogWrite(MOTOR_X1, speed);
   }
   else{
     analogWrite(MOTOR_X0, 0);
     analogWrite(MOTOR_X1, 0);
     if(direction == IDLE){
       //Serial.println("overshot");
     }
     else{
       cooldownTime = micros();
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
  //char cBuffer[100];
  // wait for any moves to complete
  while(direction != IDLE){
    delayMicroseconds(1);
  }
  
  //sprintf(cBuffer, "get x (%d)", vPosition);
  //Serial.println(cBuffer);
  
  // convert physical to virtual coordinates
  return vPosition;
}

int X_axis::getRealPosition(){
  return rPosition;
}

void X_axis::setPosition(int setp){
  //char cBuffer[100];
  while(direction != IDLE || (micros() - cooldownTime) < X_COOLDOWN){
    delayMicroseconds(1);
  }
  
  //sprintf(cBuffer, "set x (%d)", setp);
  //Serial.println(cBuffer);
  
  quickSetPosition(setp);
}

void X_axis::quickSetPosition(int setp){
  
  vPosition = setp;
  
  // convert virtual to physical coordinates
  setPoint = (int) (setp * YX_SCALE);
   
  direction = IDLE;
  if(rPosition > setPoint){
    direction = LEFT;
    setSpeed();
  }
  else if(rPosition < setPoint){
    direction = RIGHT;
    setSpeed(); 
  } 
}

void X_axis::setBounds(bounds_t b){
  bounds = b;  
}

void X_axis::initPosition(int pos){
  rPosition = pos; 
  vPosition = (int) (pos * XY_SCALE);
}

bounds_t X_axis::getBounds(){
  return bounds;
}

x_direction_t X_axis::getDirection(){
  return direction; 
}

/*********************************************************/
/*	Y axis 			    
/*********************************************************/
Y_axis::Y_axis() : 
stepper(MOTOR_Y0, MOTOR_Y1, MOTOR_Y2, MOTOR_Y3, Y_STEPPER_PWM)
{
  position = 0;
  bounds = {-10000,10000};
  cooldownTime = micros();
}


void Y_axis::stepDown(){
  if(--position < bounds.b0){
    panic("y < SW bound"); 
  }
  if(digitalRead(BUTTON_Y1) == LOW){
    panic("y < HW bound");
  }
  while((micros() - cooldownTime) < Y_COOLDOWN){}
  stepper.stepRight();
  cooldownTime = micros();
}

void Y_axis::stepUp(){
  if(++position > bounds.b1){
    panic("y > SW bound"); 
  }
  if(digitalRead(BUTTON_Y0) == LOW){
      panic("y > HW bound");
  }
  while((micros() - cooldownTime) < Y_COOLDOWN){}
  stepper.stepLeft();
  cooldownTime = micros(); 
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

void Y_axis::setBounds(bounds_t b){
  bounds = b;
}

void Y_axis::initPosition(int pos){
  position = pos; 
}

bounds_t Y_axis::getBounds(){
  return bounds;
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

