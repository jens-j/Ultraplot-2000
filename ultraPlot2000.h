#ifndef main_h
#define main_h

#include "plotter.h"
#include "axis.h"


/*********************************************************/
/*	Pinout			    
/*********************************************************/

// X-axis
#define SENSOR_X0  		20
#define SENSOR_X1  		21
#define SENSOR_INT              19 
#define MOTOR_X0                10
#define MOTOR_X1 		9

// Y-axis
#define BUTTON_Y0               7
#define BUTTON_Y1               8 
#define MOTOR_Y0 		5
#define MOTOR_Y1 		4
#define MOTOR_Y2 		3
#define MOTOR_Y3 		2

// Z-axis
#define MOTOR_Z0 		46
#define MOTOR_Z1 		50
#define MOTOR_Z2 		48
#define MOTOR_Z3 		52

// LCD Display
#define LCD_RS			43
#define LCD_E			45
#define LCD_D4			47
#define LCD_D5			49
#define LCD_D6			51
#define LCD_D7			53

// Buttons
#define BUTTON_UP 		35
#define BUTTON_DOWN 	        27	
#define BUTTON_LEFT 	        33
#define BUTTON_RIGHT 	        29
#define BUTTON_MID 		31

// Other
#define LED       		13


/*********************************************************/
/*	Constants			    
/*********************************************************/
#define X_SPEED_MIN     80
#define X_SPEED_MAX   	120
#define BTN_DEBOUNCE_P  20  // button debounce period in ms
#define BTN_RETRIGGER_P 300 // button retrigger period in ms
#define X_STEPSIZE      0.0353 // in mm | Because the X resoluton is scaled to the Y resolution,
#define Y_STEPSIZE      0.0212 // in mm | Always use the Y_STEPSIZE
#define XY_SCALE        (X_STEPSIZE / Y_STEPSIZE)
#define YX_SCALE        (Y_STEPSIZE / X_STEPSIZE)
#define Y_STEPPER_PWM   100
#define X_COOLDOWN      2000 // in us
#define Y_COOLDOWN      1200 // in us
#define Z_COOLDOWN 		800  // in us

/*********************************************************/
/*	Types			    
/*********************************************************/
typedef void (*callback_function_t)(void);
enum menuSel_t {MENU_MAIN = 0, MENU_DRAW = 1};


/*********************************************************/
/*	Functions			    
/*********************************************************/
int digitalPinToInterrupt(int);
void panic(char *s);


/*********************************************************/
/*	Global variables			    
/*********************************************************/
extern Plotter plotter;

#endif
