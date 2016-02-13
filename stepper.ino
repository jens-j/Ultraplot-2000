#include "plotter.h"
#include "stepper.h"

// constructor
Stepper::Stepper(int pin0, int pin1, int pin2, int pin3){
	m0 = pin0;
	m1 = pin1;
	m2 = pin2;
	m3 = pin3;
	state = 0;
	setState(state);
}

StepperZ::StepperZ(int pin0, int pin1, int pin2, int pin3) : Stepper(pin0, pin1, pin2, pin3)
{
	setPosition(UP);
}

// set the motor to a certain state (private)
void Stepper::setState(int s){
	digitalWrite(m0, PHASE[s].s0);
	digitalWrite(m1, PHASE[s].s1);
	digitalWrite(m2, PHASE[s].s2);
	digitalWrite(m3, PHASE[s].s3);	
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

// get the current poition of the stepper
position_t StepperZ::getPosition(){
	return position;
}

// set the steppers position
void StepperZ::setPosition(position_t pos){
	int i, step;

	if(position == pos || position == MOVING)
		return;

	position = MOVING;

	for(i = 0; i < 40; i++){
		if(pos == UP){
			for(step = 0; step < N_PHASE; step++){
				setState(step);
				delayMicroseconds(800);
			}
                }
		else{
			for(step = N_PHASE-1; step >= 0; step--){
				setState(step);
				delayMicroseconds(800);
			}			
		}
	}

	position = pos;
}
