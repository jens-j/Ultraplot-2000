#include <avr/wdt.h>
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
  char buffer[20];
  int i;
  logCount = 0;
  for(i = 0; i < LOGSIZE; i++){
    rLog[i] = 0;
    vLog[i] = 0;
    pidLog[i] = 0;
  }
  //logTime = micros();
  pidTime = micros();
  sdata0 = 0;
  rPosition = 0;
  //loadPosition();
  vPosition = (int) (rPosition * XY_SCALE);
  //loadBounds();
  bounds = {-4000, 4000};
  setPoint = 0;
  traveled = 0;
  quick = false;
  kickoff = false;
  direction = IDLE;
  cooldownTime = micros();
}

void X_axis::sensorIsr(){
  char buffer[20];
  wdt_reset();
  int sdata = sensor.decodeSensor();
  
  if(sdata - sdata0 == 1 || sdata - sdata0 == -3){
    rPosition++;
    traveled++;
  }
  else{
    rPosition--; 
    traveled--;
  }
  
  if(rPosition < bounds.b0){
    sprintf(buffer, "x<SWbound %d<%d", rPosition, bounds.b0);
    panic(buffer);
  }
  if(rPosition > bounds.b1){
    sprintf(buffer, "x>SWbound %d>%d", rPosition, bounds.b1);
    panic(buffer);
  }  
 
  setSpeed();
  sdata0 = sdata;  
}


void X_axis::wdTimerIsr(){
  char cBuffer[100];
  sprintf(cBuffer, "wdt ovf (%d, %d)", setPoint - rPosition, traveled);
  panic(cBuffer);
}


void X_axis::setSpeed(){
  int v; // speed setpoint
  int r; // actual speed
  int e; // error 
  double d_e; // differential error
  double d_v;; // setpoint 
   
  int dist = abs(setPoint - rPosition);
  
  // ramp the speed setpoint based on the distance to the setpoint
  v = X_SPEED_MIN + (int) (X_SPEED_SLOPE * dist);
  
  // constrain to the maximal speed for drawing or quick move
  if(quick == true){
    v = constrain(v, X_SPEED_MIN, X_SPEED_QUICK);
  }
  else{
    v = constrain(v, X_SPEED_MIN, X_SPEED_DRAW);
  }
  
  // set a initial PWM value in the first cycle of a new move
  if(kickoff == true){
    previousError = 0;
    cumulativeError = 0;
    pidOutput = X_PWM_INIT;
    kickoff = false;
    r = v;
  } 
  else{
   
    // calculate the actual speed (mm/s)
    r = (X_STEPSIZE * 1E6) / ((double) (micros() - pidTime));
    pidTime = micros();
    
    // calculate pwm change 
    e = v - r;
    d_e = e - previousError;
    previousError = e;
    cumulativeError += e;
    d_v = PID_P * e + PID_D * d_e + PID_I * cumulativeError;
    pidOutput += d_v;
  }
  
  // constrain PWM output
  pidOutput = constrain(pidOutput, X_PWM_MIN, X_PWM_MAX);
  
  // logging 
  rLog[logCount] = r;
  vLog[logCount] = v;
  pidLog[logCount] = pidOutput;
  if(++logCount == LOGSIZE) 
    logCount = 0;
  
  // write the speed to the motor 
  if(direction == LEFT and rPosition > setPoint){
    analogWrite(MOTOR_X0, pidOutput);
  }
  else if(direction == RIGHT and rPosition < setPoint){
    analogWrite(MOTOR_X1, pidOutput);
  }
  else{ // end position reached  
  
    // disable wdt interrupt
    WDTCSR &= ~(1<<WDIE); 
    
    // stop the motor
    analogWrite(MOTOR_X0, 0);
    analogWrite(MOTOR_X1, 0);
    quick = false;
    
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
  // wait for any moves to complete
  while(direction != IDLE){
    delayMicroseconds(1);
  }
  
  // convert physical to virtual coordinates
  return vPosition;
}

int X_axis::getSetPoint(){
  return setPoint;
}

int X_axis::getRealPosition(){
  return rPosition;
}

void X_axis::setPosition(int setp){

  // wait for the head to settle from the previous move
  while(direction != IDLE || (micros() - cooldownTime) < X_COOLDOWN){
    delayMicroseconds(1);
  }
  
  quick = false;
  initMove(setp);
}

void X_axis::quickSetPosition(int setp){
  quick = true;
  initMove(setp);
}

void X_axis::initMove(int setp){
  
  vPosition = setp;
  
  // convert virtual to physical coordinates
  setPoint = (int) (setp * YX_SCALE);  
  
  // enable watchdog timer interrupt
  wdt_reset();  
  //WDTCSR |= (1<<WDIE); 
 
  // flag the start of a new move
  traveled = 0;
  kickoff = true;
  
  // set direction and speed
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

void X_axis::uploadLog(){
  int i;
  
  for(i = 0; i < LOGSIZE; i++){
    Serial.print(i);
    Serial.print(",");
    Serial.print((unsigned int) rLog[i]);
    Serial.print(",");
    Serial.print((unsigned int) vLog[i]);
    Serial.print(",");
    Serial.println((unsigned int) pidLog[i]);
  }
}

/*********************************************************/
/*	Y axis 			    
/*********************************************************/
Y_axis::Y_axis() : 
stepper(MOTOR_Y0, MOTOR_Y1, MOTOR_Y2, MOTOR_Y3, Y_STEPPER_PWM)
{
  loadPosition();
  //loadBounds();
  bounds = {-6000, 6000};
  cooldownTime = micros();
}

void Y_axis::stepDown(){
  stepDown(Y_COOLDOWN_MAX);
}

void Y_axis::stepDown(int cooldown){
  if(--position < bounds.b0){
    panic("y < SW bound %d %d"); 
  }
  if(digitalRead(BUTTON_Y0) == LOW){
    panic("y < HW bound");
  }
  while((micros() - cooldownTime) < cooldown){
    delayMicroseconds(1);
  }
  stepper.stepRight();
  cooldownTime = micros();
}

void Y_axis::stepUp(){
  stepUp(Y_COOLDOWN_MAX);
}

void Y_axis::stepUp(int cooldown){
  if(++position > bounds.b1){
    panic("y > SW bound"); 
  }
  if(digitalRead(BUTTON_Y1) == LOW){
      panic("y > HW bound");
  }
  while((micros() - cooldownTime) < cooldown){
    delayMicroseconds(1);
  }
  stepper.stepLeft();
  cooldownTime = micros(); 
}

int Y_axis::getPosition(){
  return position;
}

void Y_axis::setPosition(int setPoint){
  int cooldown;
  int s = 0;
  int d;
  
  while(setPoint != position){
    
    int d = abs(position - setPoint);
    
    // set cooldown time
    if(abs(d - s) < Y_COOLDOWN_RAMP)
      cooldown = map(abs(d- s), 0, Y_COOLDOWN_RAMP-1, Y_COOLDOWN_MAX, Y_COOLDOWN_MIN);
    else if(abs(s) < Y_COOLDOWN_RAMP)
      cooldown = map(abs(s), 0, Y_COOLDOWN_RAMP-1, Y_COOLDOWN_MAX, Y_COOLDOWN_MIN);
    else 
      cooldown = Y_COOLDOWN_MIN;
    s++;
     
    if(setPoint < position)
      stepDown(cooldown);
    if(setPoint > position)
      stepUp(cooldown);
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
  //loadPosition(); 
  position = Z_UNKNOWN;
}

z_position_t Z_axis::getPosition(){
  return position;
}

void Z_axis::moveRelative(int d){
 char cBuffer[100];
 //sprintf(cBuffer, "move head %d", d);
 //Serial.println(cBuffer); 
 int dist = d * 8;
 int step = dist;
 int cooldown;
 
 while(step != 0){
   
   if(abs(dist - step) < Z_COOLDOWN_RAMP){
     cooldown = map(abs(dist - step), 0, Z_COOLDOWN_RAMP-1, Z_COOLDOWN_MAX, Z_COOLDOWN_MIN);
   }
   else if(abs(step) <= Z_COOLDOWN_RAMP){
     cooldown = map(abs(step), 1, Z_COOLDOWN_RAMP, Z_COOLDOWN_MAX, Z_COOLDOWN_MIN);
   }
   else{
     cooldown = Z_COOLDOWN_MIN;
   }
   
   if(step > 0){
     stepper.stepRight();
     step--;
   }
   else{
     stepper.stepLeft();
     step++;
   }
   
   delayMicroseconds(cooldown);
 } 
}

void Z_axis::setPosition(z_position_t setPoint){
  int i;
  char cBuffer[100];
  
  if(position == setPoint)
    return;

  // force the head in a up position after startup
  if(position == Z_UNKNOWN){
    moveRelative(-40);
    position = Z_UP;
  }
    
  //sprintf(cBuffer, "set head from %d to %d", position, setPoint);
  //Serial.println(cBuffer); 

  moveRelative(setPoint - position);

  position = setPoint;
}


