#include "plotter.h"
#include "axis.h"
#include "sensor.h"
#include "stepper.h"
#include "TimerOne.h"


/*********************************************************/
/*	X axis 			    
/*********************************************************/
X_axis::X_axis() : 
sensor() 
{
  //sensor = Sensor();
  sdata0 = 0;
  position = 0;
  setPoint = 0;
  direction = IDLE;
}

void X_axis::isr(){
  int sdata = sensor.decodeSensor();
  
  if(sdata - sdata0 == 1 || sdata - sdata0 == -3)
    position++;
  else
    position--; 
    
  setSpeed();
  
  sdata0 = sdata;  
  
  //Serial.print(setPoint);
  //Serial.print(",");
  //Serial.println(position);
  //print(",");
  //print(setPoint);
}

void X_axis::setSpeed(){
  int speed;
  int diff = abs(setPoint - position);
  
  if(diff < 10)
    speed = 80;
  else if(diff < 50)
    speed = 90;
  else
    speed = 100;
   
   if(direction == LEFT and position > setPoint){
     analogWrite(MOTOR_X0, speed);
//     Serial.print("motor 0: ");
//     Serial.println(speed);
   }
   else if(direction == RIGHT and position < setPoint){
     analogWrite(MOTOR_X1, speed);
//     Serial.print("motor 1: ");
//     Serial.println(speed);
   }
   else{
     Serial.println("done");
     analogWrite(MOTOR_X0, 0);
     analogWrite(MOTOR_X1, 0);
     direction = IDLE;
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
  while(direction != IDLE){delay(1);}
}

/*********************************************************/
/*	Y axis 			    
/*********************************************************/
Y_axis::Y_axis() : 
stepper(MOTOR_Y0, MOTOR_Y1, MOTOR_Y2, MOTOR_Y3, 100)
{
  position = 0;
}

void Y_axis::stepDown(){
  position--;
  if(digitalRead(BUTTON_Y1) == HIGH)
    stepper.stepRight();
    delayMicroseconds(800);
}

void Y_axis::stepUp(){
  position++;
  if(digitalRead(BUTTON_Y0) == HIGH)
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

void Y_axis::step_ISR(){

}

/*********************************************************/
/*	Z axis 			    
/*********************************************************/
Z_axis::Z_axis() : 
stepper(MOTOR_Z0, MOTOR_Z1, MOTOR_Z2, MOTOR_Z3, 255) 
{
  position = DOWN;
  setPosition(UP);
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

