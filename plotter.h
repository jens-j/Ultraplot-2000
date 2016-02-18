#ifndef plotter_h
#define plotter_h

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
#define XY_SCALE        1.673
#define YX_SCALE        0.598


/*********************************************************/
/*	Types			    
/*********************************************************/
typedef void (*callback_function_t)(void);
enum menuSel_t {MENU_MAIN = 0, MENU_DRAW = 1};

/*********************************************************/
/*	Functions			    
/*********************************************************/
int digitalPinToInterrupt(int);


/*********************************************************/
/*	Global variables			    
/*********************************************************/
extern X_axis x_axis;
extern Y_axis y_axis;
extern Z_axis z_axis;

#endif
