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
char cBuffer[100];
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

  z_axis.setPosition(UP);
  
  while(1){}
}

void quickAbsolute(int setX, int setY){
  x_axis.setPosition(setX);
  y_axis.setPosition(setY);
}

void moveAbsolute(int setX, int setY){
  int i;
  int x = x_axis.getPosition();
  int y = y_axis.getPosition();
  int x0 = x;
  int y0 = y;
  int d_x = setX - x0;
  int d_y = setY - y0;
  double slope = (double) d_x / (double) d_y;
  
  if(d_x == 0){
    y_axis.setPosition(setY);
  }
  else if(d_y == 0){
    x_axis.setPosition(setX);
  }
  else{
    while(y != setY){
      
      x_axis.setPosition(x0 + (int) ((y-y0) * slope));
      
      if(d_y < 0){
        y_axis.stepDown();
        y--;
      }
      else{
        y_axis.stepUp();
        y++;  
      }
    }
  }
  x_axis.setPosition(setX);
}

void moveRelative(int x, int y){
  moveAbsolute(x_axis.getPosition() + x, y_axis.getPosition() + y);  
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
  x_axis.setBounds({-10000, 10000});
  while(buttons.getButtonEvent() != BUTTON_MID){
    lcd.setCursor(0, 3);
    lcd.print(x_axis.getPosition());
    lcd.print("     ");
  }
  xbound0 = x_axis.getPosition();
  lcd.setCursor(0, 2);
  lcd.print("set right x bound");
  while(buttons.getButtonEvent() != BUTTON_MID){
    lcd.setCursor(0, 3);
    lcd.print(x_axis.getPosition());
    lcd.print("     ");
  }
  xbound1 = x_axis.getPosition();
  xrange = xbound1 - xbound0;
  if(xrange & 1){
    x_axis.setBounds({-1 * xrange / 2, xrange / 2});
    x_axis.initPosition(xrange / 2); 
  }
  else{
    x_axis.setBounds({-1 * xrange / 2, xrange / 2 - 1});
    x_axis.initPosition(xrange / 2 - 1); 
  }
  x_axis.setPosition(0);
  
  
  // Y-axis
  lcd.setCursor(0, 2);
  lcd.print("set lower y bound");
  y_axis.setBounds({-20000, 20000});
  while(buttons.getButtonEvent() != BUTTON_MID){
    if(buttons.isPressed() == BUTTON_LEFT){
      y_axis.stepDown();  
    }
    if(buttons.isPressed() == BUTTON_RIGHT){
      y_axis.stepUp();  
    }
    if(millis() - refresh > 100){
      lcd.setCursor(0, 3);
      lcd.print("<- ");
      lcd.print(y_axis.getPosition());
      lcd.print(" ->  ");
      refresh = millis();
    }
  }  
  ybound0 = y_axis.getPosition();
  lcd.setCursor(0, 2);
  lcd.print("set upper y bound");
  while(buttons.getButtonEvent() != BUTTON_MID){
    if(buttons.isPressed() == BUTTON_LEFT){
      y_axis.stepDown();  
    }
    if(buttons.isPressed() == BUTTON_RIGHT){
      y_axis.stepUp();  
    }
    if(millis() - refresh > 100){
      lcd.setCursor(0, 3);
      lcd.print("<- ");
      lcd.print(y_axis.getPosition());
      lcd.print(" ->  ");
      refresh = millis();
    }
  }  
  ybound1 = y_axis.getPosition();
  yrange = ybound1- ybound0;
  if(yrange & 1){
    y_axis.setBounds({-1 * yrange / 2, yrange / 2});
    y_axis.initPosition(yrange / 2); 
  }
  else{
    y_axis.setBounds({-1 * xrange / 2, yrange / 2 - 1});
    y_axis.initPosition(yrange / 2 - 1); 
  }
  y_axis.setPosition(0);
  
  // Overview
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration results:");
  lcd.setCursor(0, 1);
  sprintf(lcdBuffer, "X: [%d,%d]", x_axis.getBounds().b0, x_axis.getBounds().b1);
  lcd.print(lcdBuffer);
  lcd.setCursor(0, 2);
  sprintf(lcdBuffer, "Y: [%d,%d]", y_axis.getBounds().b0, y_axis.getBounds().b1);
  lcd.print(lcdBuffer);
  lcd.setCursor(0, 3);
  sprintf(lcdBuffer, "%umm x %umm", (unsigned int) (xrange * X_RESOLUTION), (unsigned int) (yrange * Y_RESOLUTION));
  lcd.print(lcdBuffer);
  
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
            calibrate();
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
            sprintf(cBuffer, "(x,y) = (%d,%d)", x_axis.getPosition(), y_axis.getPosition());
            Serial.print(cBuffer);
            drawSinc(2000);
            break;
          case 1:
            sprintf(cBuffer, "(x,y) = (%d,%d)", x_axis.getPosition(), y_axis.getPosition());
            Serial.print(cBuffer);
            drawCircle(5000);
            break;
          case 2:
            sprintf(cBuffer, "(x,y) = (%d,%d)", x_axis.getPosition(), y_axis.getPosition());
            Serial.print(cBuffer);
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
