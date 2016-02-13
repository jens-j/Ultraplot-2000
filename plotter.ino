#include "plotter.h"
#include "stepper.h"
#include "sensor.h"

StepperZ head = StepperZ(MOTOR_Z0, MOTOR_Z1, MOTOR_Z2, MOTOR_Z3);

void setup(){
	pinMode(MOTOR_Z0, OUTPUT);
	pinMode(MOTOR_Z1, OUTPUT);
	pinMode(MOTOR_Z2, OUTPUT);
	pinMode(MOTOR_Z3, OUTPUT);
	Serial.begin(115200);
}

void loop(){
	delay(1000);
	head.setPosition(DOWN);
	delay(1000);
	head.setPosition(UP);
}