#include <LiquidCrystal.h>
#include "TimerOne.h"
#include "plotter.h"
#include "axis.h"
#include "buttons.h"

char cBuffer[20];

X_axis x_axis = X_axis();
Y_axis y_axis = Y_axis();
Z_axis z_axis = Z_axis();
Buttons buttons = Buttons();
LiquidCrystal lcd = LiquidCrystal(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

int digitalPinToInterrupt(int pin){
  switch(pin){
    case 2:
      return 0;
    case 3:
      return 1;
    case 21:
      return 2;
    case 20:
      return 3;
    case 19:
      return 4;
    case 18:
      return 5;
    default:
      return -1;  
  }
}

void sensorIsrDispatcher(){
  x_axis.isr();
}

void timerIsrDispatcher(){
  buttons.isr(); 
}

void drawSinc(){
  int i;
  int y;
  
  z_axis.setPosition(UP);
  x_axis.setPosition(-2000);
  z_axis.setPosition(DOWN);
  
  for(i = -2000; i < 2000; i++){
    x_axis.stepRight();
    if(i != 0)
      y = (1000 * sin((2 * 3.141 * i) / 200.0)) / (i/200.0);
    y_axis.setPosition(y);
  }
  
  z_axis.setPosition(UP);
  x_axis.setPosition(0);
}

void drawCircle(int diameter){
  int i;
  double angle;
  int x_pos;
  
  z_axis.setPosition(UP);
  x_axis.setPosition(diameter);
  z_axis.setPosition(DOWN);
  
  for(i = 1; i <= diameter; i++){
    y_axis.stepUp();
    angle = asin((double) i / (double) diameter);
    x_pos = diameter * cos(angle);
    x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);
  } 

  for(i = diameter; i >= 1; i--){
    y_axis.stepDown();
    angle = asin((double) i / (double) diameter);
    x_pos = -1 * diameter * cos(angle);
    x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);
  } 
  
  for(i = 1; i <= diameter; i++){
    y_axis.stepDown();
    angle = asin((double) i / (double) diameter);
    x_pos = -1 * diameter * cos(angle);
    x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);    
  } 
  
  for(i = diameter; i >= 1; i--){
    y_axis.stepUp();
    angle = asin((double) i / (double) diameter);
    x_pos = diameter * cos(angle);
    x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);
  }
  
  z_axis.setPosition(UP);
  x_axis.setPosition(0);
  
}


void setup(){
  pinMode(MOTOR_X0, OUTPUT);
  pinMode(MOTOR_X1, OUTPUT);
  pinMode(MOTOR_Y0, OUTPUT);
  pinMode(MOTOR_Y1, OUTPUT);
  pinMode(MOTOR_Y2, OUTPUT);
  pinMode(MOTOR_Y3, OUTPUT);
  pinMode(MOTOR_Z0, OUTPUT);
  pinMode(MOTOR_Z1, OUTPUT);
  pinMode(MOTOR_Z2, OUTPUT);
  pinMode(MOTOR_Z3, OUTPUT);
  // attach external interrupt for the sensor
  attachInterrupt(digitalPinToInterrupt(SENSOR_X0), sensorIsrDispatcher, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SENSOR_X1), sensorIsrDispatcher, CHANGE);
  // intitialize timer interrupt for the buttons
  Timer1.initialize(20000); 
  Timer1.attachInterrupt(timerIsrDispatcher);
  Serial.begin(115200);
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("test");
}


void loop(){
  lcd.setCursor(0, 1);
  //sprintf(cBuffer, "button: %d", buttons.getLastPressed());
  //lcd.print(cBuffer);
  lcd.print(buttons.getLastPressed());

  //drawSinc();
  //drawCircle(1500);
  //while(1){delay(1);}
  
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(millis()/1000);
  
}
