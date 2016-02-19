#include "Arduino.h"
#include "ultraPlot2000.h"
#include "stepper.h"

// constructor
Stepper::Stepper(int pin0, int pin1, int pin2, int pin3, int dutyCycle){
  m0 = pin0;
  m1 = pin1;
  m2 = pin2;
  m3 = pin3;
  state = 0;
  pwmDutyCycle = dutyCycle;
  setState(state);
}

// set the motor to a certain state (private)
void Stepper::setState(int s){
  analogWrite(m0, PHASE[s].s0 * pwmDutyCycle);
  analogWrite(m1, PHASE[s].s1 * pwmDutyCycle);
  analogWrite(m2, PHASE[s].s2 * pwmDutyCycle);
  analogWrite(m3, PHASE[s].s3 * pwmDutyCycle);	
  state = s;
}

// let the motor take a single step left
void Stepper::stepLeft(){
  if(state == 0)
    state = 7;
  else
    state--;
  setState(state);
}

// let the motor take a single step left
void Stepper::stepRight(){
  if(state == 7)
    state = 0;
  else
    state++;
  setState(state);
}


