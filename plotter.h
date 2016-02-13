#ifndef plotter_h
#define plotter_h


/**********************************/
/*	Pinout			    
/**********************************/

// X-axis
#define SENSOR_X0  		20
#define SENSOR_X1  		21
#define SENSOR_INT  	        19 
#define MOTOR_X0    	        11
#define MOTOR_X1 		10

// Y-axis
#define MOTOR_Y0 		38
#define MOTOR_Y1 		36
#define MOTOR_Y2 		34
#define MOTOR_Y3 		32

// Z-axis
#define MOTOR_Z0 		46
#define MOTOR_Z1 		50
#define MOTOR_Z2 		48
#define MOTOR_Z3 		52

// LCD Display
#define LCD_RS			51
#define LCD_E			53
#define LCD_D4			43
#define LCD_D5			45
#define LCD_D6			47
#define LCD_D7			49

// Buttons
#define BUTTON_UP 		35
#define BUTTON_DOWN 	27	
#define BUTTON_LEFT 	33
#define BUTTON_RIGHT 	29
#define BUTTON_MID 		31

// Other
#define LED       		13


/**********************************/
/*	Constants			    
/**********************************/
#define X_SPEED   		75


/**********************************/
/*	Types			    
/**********************************/
typedef void (*callback_function_t)(void);


#endif
