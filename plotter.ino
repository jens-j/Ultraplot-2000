#include <LiquidCrystal.h>
#include "TimerOne.h"
#include "plotter.h"
#include "axis.h"
#include "buttons.h"
#include "shapes.h"

//#define N_MAIN_MENU_ITEMS 3
//#define N_DRAW_MENU_ITEMS 4


const int  N_MENU_ENTRIES[2] = {4, 4};

const char MAIN_MENU_TEXT[4][20] = {"Calibrate", 
                                    "Move head up", 
                                    "Move head down", 
                                    "Draw"};
const char DRAW_MENU_TEXT[4][20] = {"Sinc", 
                                    "Circle", 
                                    "Square",
                                    "Back"};

char lcdBuffer[20];
menuSel_t menuSel = MENU_MAIN;
int menuPosition = 0;

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

void printMenu(){
  int i;
  const char (*menu)[20];
  
  if(menuSel == MENU_MAIN){
    menu = &MAIN_MENU_TEXT[0];
  }
  if(menuSel == MENU_DRAW){
    menu = &DRAW_MENU_TEXT[0];
  }
  
  //lcd.clear();
  for(i = 0; i < N_MENU_ENTRIES[menuSel]; i++){
    lcd.setCursor(0, i);
    if(i == menuPosition){
      lcd.print(">");
      lcd.print(menu[i]);
    }
    else{
      lcd.print(" ");
      lcd.print(menu[i]);
    }
  } 
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
  Timer1.initialize(BTN_DEBOUNCE_P * 1000); 
  Timer1.disablePwm(MOTOR_X1);
  Timer1.attachInterrupt(timerIsrDispatcher);

  Serial.begin(115200);
  lcd.begin(20, 4);
  printMenu();
}


void loop(){
  int command;

  command = buttons.getButtonEvent();
  if(command == BUTTON_DOWN){
    menuPosition = (menuPosition == N_MENU_ENTRIES[menuSel]-1) ? 0 : menuPosition+1;
  }
  if(command == BUTTON_UP){
    menuPosition = (menuPosition == 0) ? N_MENU_ENTRIES[menuSel]-1 : menuPosition-1;
  }
  if(command == BUTTON_MID){   
    switch(menuSel){
      
      case MENU_MAIN:
        switch(menuPosition){
          case 0:
            break;
          case 1:
            z_axis.setPosition(UP);
            break;
          case 2:
            z_axis.setPosition(DOWN);
            break;
          case 3:
            menuSel = MENU_DRAW;
            menuPosition = 0;
            lcd.clear();
        }
        break;
        
        
      case MENU_DRAW:
        switch(menuPosition){
          case 0:
            drawSinc();
            break;
          case 1:
            drawCircle(5000);
            break;
          case 2:
            drawSquare(5000);
            break;
          case 3:
            menuSel = MENU_MAIN;
            menuPosition = 0;
            lcd.clear();
        }
        break;
    }
  }
  
  printMenu();
  
  
}
