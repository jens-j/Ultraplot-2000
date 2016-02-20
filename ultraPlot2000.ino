#include <LiquidCrystal.h>
#include "TimerOne.h"
#include "ultraPlot2000.h"
#include "plotter.h"
#include "axis.h"
#include "buttons.h"
#include "shapes.h"


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
char cBuffer[100];
menuSel_t menuSel = MENU_MAIN;
int menuPosition = 0;


Plotter plotter = Plotter();
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
  plotter.x_axis.isr();
}


void timerIsrDispatcher(){
  buttons.isr(); 
}


void panic(char *s){
  detachInterrupt(digitalPinToInterrupt(SENSOR_X0));
  detachInterrupt(digitalPinToInterrupt(SENSOR_X1));
  Timer1.detachInterrupt();
  
  digitalWrite(MOTOR_X0, LOW);
  digitalWrite(MOTOR_X1, LOW);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[Panic!]");
  
  lcd.setCursor(0, 2);
  lcd.print(s);

  plotter.moveHeadUp();
  
  while(1){}
}


void calibrate(){
  int xbound0, xbound1;
  int ybound0, ybound1;
  int xrange, yrange;
  int refresh = 0;
  
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[Calibration mode]");
  
  // X-axis
  lcd.setCursor(0, 2);
  lcd.print("set left x bound");
  plotter.x_axis.setBounds({-10000, 10000});
  buttons.clearEvent();
  while(buttons.getButtonEvent() != BUTTON_MID){
    lcd.setCursor(0, 3);
    lcd.print(plotter.x_axis.getPosition());
    lcd.print("     ");
  }
  xbound0 = plotter.x_axis.getPosition();
  lcd.setCursor(0, 2);
  lcd.print("set right x bound");
  buttons.clearEvent();
  while(buttons.getButtonEvent() != BUTTON_MID){
    lcd.setCursor(0, 3);
    lcd.print(plotter.x_axis.getPosition());
    lcd.print("     ");
  }
  xbound1 = plotter.x_axis.getPosition();
  if(xbound1 <= xbound0){
    panic("right <= left");
  }
  xrange = xbound1 - xbound0;
  plotter.x_axis.setBounds({0, xrange - 1});
  plotter.x_axis.initPosition(xrange - 1); 
  plotter.x_axis.setPosition(xrange / 2);
  
  
  // Y-axis
  lcd.setCursor(0, 2);
  lcd.print("set lower y bound");
  plotter.y_axis.setBounds({-20000, 20000});
  buttons.clearEvent();
  while(buttons.getButtonEvent() != BUTTON_MID){
    if(buttons.isPressed() == BUTTON_LEFT){
      plotter.y_axis.stepDown();  
    }
    if(buttons.isPressed() == BUTTON_RIGHT){
      plotter.y_axis.stepUp();  
    }
    if(millis() - refresh > 100){
      lcd.setCursor(0, 3);
      lcd.print("<- ");
      lcd.print(plotter.y_axis.getPosition());
      lcd.print(" ->  ");
      refresh = millis();
    }
  }  
  ybound0 = plotter.y_axis.getPosition();
  lcd.setCursor(0, 2);
  lcd.print("set upper y bound");
  buttons.clearEvent();
  while(buttons.getButtonEvent() != BUTTON_MID){
    if(buttons.isPressed() == BUTTON_LEFT){
      plotter.y_axis.stepDown();  
    }
    if(buttons.isPressed() == BUTTON_RIGHT){
      plotter.y_axis.stepUp();  
    }
    if(millis() - refresh > 100){
      lcd.setCursor(0, 3);
      lcd.print("<- ");
      lcd.print(plotter.y_axis.getPosition());
      lcd.print(" ->  ");
      refresh = millis();
    }
  }  
  ybound1 = plotter.y_axis.getPosition();
  if(ybound1 <= ybound0){
    panic("upper <= lower");
  }
  yrange = ybound1- ybound0;
  plotter.y_axis.setBounds({0, yrange - 1});
  plotter.y_axis.initPosition(yrange - 1); 
  plotter.y_axis.setPosition(yrange / 2);
  
  // Overview
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration results:");
  lcd.setCursor(0, 1);
  sprintf(lcdBuffer, "X: [%d,%d]", plotter.x_axis.getBounds().b0, plotter.x_axis.getBounds().b1);
  lcd.print(lcdBuffer);
  lcd.setCursor(0, 2);
  sprintf(lcdBuffer, "Y: [%d,%d]", plotter.y_axis.getBounds().b0, plotter.y_axis.getBounds().b1);
  lcd.print(lcdBuffer);
  lcd.setCursor(0, 3);
  sprintf(lcdBuffer, "%umm x %umm", (unsigned int) (xrange * X_STEPSIZE), (unsigned int) (yrange * Y_STEPSIZE));
  lcd.print(lcdBuffer);
  buttons.clearEvent();
  while(buttons.getButtonEvent() != BUTTON_MID){
    delayMicroseconds(1);
  }
  
  lcd.clear();
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
  
  //drawCircle(20);
  plotter.moveHeadDown();
  
  plotter.moveAbsolute(20.0, 20.0);
  plotter.arcAbsoluteCCW(-20.0, 20.0, -20.0, 0.0);
  plotter.arcAbsoluteCCW(20.0, 20.0, 20.0, 0.0);

  plotter.moveHeadUp();
  plotter.quickAbsolute(0, 0);
  while(1){delayMicroseconds(1);}
//  int command;
//
//  command = buttons.getButtonEvent();
//  if(command == BUTTON_DOWN){
//    menuPosition = (menuPosition == N_MENU_ENTRIES[menuSel]-1) ? 0 : menuPosition+1;
//  }
//  if(command == BUTTON_UP){
//    menuPosition = (menuPosition == 0) ? N_MENU_ENTRIES[menuSel]-1 : menuPosition-1;
//  }
//  if(command == BUTTON_MID){   
//    switch(menuSel){
//      
//      case MENU_MAIN:
//        switch(menuPosition){
//          case 0:
//            calibrate();
//            break;
//          case 1:
//            plotter.moveHeadUp();
//            break;
//          case 2:
//            plotter.moveHeadDown();
//            break;
//          case 3:
//            menuSel = MENU_DRAW;
//            menuPosition = 0;
//            lcd.clear();
//        }
//        break;
//        
//        
//      case MENU_DRAW:
//        switch(menuPosition){
//          case 0:
//            drawSinc(2000);
//            break;
//          case 1:
//            drawCircle(5000);
//            break;
//          case 2:
//            drawSquare(5000);
//            break;
//          case 3:
//            menuSel = MENU_MAIN;
//            menuPosition = 0;
//            lcd.clear();
//        }
//        break;
//    }
//  }
//  
//  printMenu();

}
