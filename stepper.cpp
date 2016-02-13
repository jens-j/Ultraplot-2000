#include "plotter.h"
#include "StepperY.h"

// constructor
Stepper::Stepper(pin0, pin1, pin2, pin3){
	m0 = pin0;
	m1 = pin1;
	m2 = pin2;
	m3 = pin3;
	state = 0;
	setState(PHASE[0]);
}

StepperZ::StepperZ(pin0, pin1, pin2, pin3){
	
}

// returns the current state of the motor
mState_t Stepper::getState(){
	return state;
}

// set the motor to a certain state (private)
Stepper::setState(mState_t state){
	digitalWrite(m0, state.s0);
	digitalWrite(m1, state.s1);
	digitalWrite(m2, state.s2);
	digitalWrite(m3, state.s3);	
}

// let the motor take a single step left
Stepper::stepLeft(){
	if(state == 0)
		state = 7;
	else
		state--;
	setState(PHASE[state]);
}

// let the motor take a single step left
Stepper::stepRight(){
	if(state == 7)
		state = 0;
	else
		state++;
	setState(PHASE[state]);
}