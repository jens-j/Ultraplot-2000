#include "plotter.h"
#include "axis.h"
#include "sensor.h"
#include "stepper.h"


/*********************************************************/
/*	X axis 			    
/*********************************************************/
X_axis::X_axis(){
	sensor = Sensor();
	position = 0;
}

void X_axis::stepLeft(){

}

void X_axis::stepRight(){

}

int X_axis::getPosition(){
	return position;
}

void X_axis::setPosition(int){

}

/*********************************************************/
/*	Y axis 			    
/*********************************************************/
Y_axis::Y_axis() : stepper(MOTOR_Y0, MOTOR_Y1, MOTOR_Y2, MOTOR_Y3)
{
	position = 0;
}

void Y_axis::stepLeft(){
	stepper.stepLeft();
}

void Y_axis::stepRight(){
	stepper.stepRight();
}

int Y_axis::getPosition(){
	return position;
}

void Y_axis::setPosition(int setPoint){
	while(setPoint < position){
		stepper.stepLeft();
		position--;
		delay(1);
	}
	while(setPoint > position){
		stepper.stepRight();
		position++;
		delay(1);	
	}
}

void Y_axis::step_ISR(){
  
}

/*********************************************************/
/*	Z axis 			    
/*********************************************************/
Z_axis::Z_axis() : stepper(MOTOR_Z0, MOTOR_Z1, MOTOR_Z2, MOTOR_Z3) 
{
	setPosition(UP);
}

position_t Z_axis::getPosition(){
	return position;
}

void Z_axis::setPosition(position_t setPoint){
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
