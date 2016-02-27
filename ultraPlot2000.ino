#include <LiquidCrystal.h>
#include "TimerOne.h"
#include "ultraPlot2000.h"
#include "plotter.h"
#include "axis.h"
#include "buttons.h"
#include "shapes.h"


const int  N_MENU_ENTRIES[2] = {4, 4};

const char MAIN_MENU_TEXT[4][20] = {"Toggle head",
                                    "Calibrate",  
                                    "GCode", 
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
    lcd.print(plotter.x_axis.getRealPosition());
    lcd.print("     ");
  }
  xbound0 = plotter.x_axis.getRealPosition();
  lcd.setCursor(0, 2);
  lcd.print("set right x bound");
  buttons.clearEvent();
  while(buttons.getButtonEvent() != BUTTON_MID){
    lcd.setCursor(0, 3);
    lcd.print(plotter.x_axis.getRealPosition());
    lcd.print("     ");
  }
  xbound1 = plotter.x_axis.getRealPosition();
  if(xbound1 <= xbound0){
    panic("right <= left");
  }
  xrange = xbound1 - xbound0;
  plotter.x_axis.setBounds({0, xrange - 1});
  plotter.x_axis.initPosition(xrange - 1); 
  plotter.x_axis.setPosition(xrange * XY_SCALE / 2);
  
  
  // Y-axis
  lcd.setCursor(0, 2);
  lcd.print("set lower y bound");
  plotter.y_axis.setBounds({-20000, 20000});
  buttons.clearEvent();
  while(buttons.getButtonEvent() != BUTTON_MID){
    if(buttons.isPressed() == BUTTON_LEFT){
      plotter.y_axis.stepDown();  
    }
    else if(buttons.isPressed() == BUTTON_RIGHT){
      plotter.y_axis.stepUp();  
    }
    if(millis() - refresh > 200){
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


void executeGCode(){
  String s;
  char *c1,*c2, *c3, *c4;
  char cBuffer[100];
  char lcdBuffer[20];
  double x,y,z,i,j;
  int count = 1;
  unsigned long pauseTime, startTime = millis(); 
  int ts, tl, t0, t1, t2, t3, t4, t5;
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[Executing GCode]");

  int k = 0;
  char test[5][100] = {   "G01 X76.239258 Y136.657482 Z-0.125000 F400.000000",
                          "G01 X76.174313 Y138.701707 Z-0.125000",
                          "G03 X76.660677 Y138.678242 Z-0.125000 I0.747928 J10.450155",
                          "G03 X83.975031 Y138.496286 Z-0.125000 I942.759146 J37750.374294",
                          "G01 X83.845138 Y139.112552 Z-0.125000"
                          };
  
  //Serial.println("start");
  
  while(1){
    Serial.println("next");
    
    while(Serial.available() == 0){
      delayMicroseconds(1); 
    }
    
    
    //s = test[k++];  
    s = Serial.readString(); 
    s.toCharArray(cBuffer, 100);

    //Serial.print(s);

    if(strstr(cBuffer, "%")){
      break;
    }

    lcd.setCursor(0, 1);
    
    if(strstr(cBuffer, "G00")){     
      if((c1 = strstr(cBuffer, "X")) && (c2 = strstr(cBuffer, "Y"))){
        x = atof(++c1);
        y = atof(++c2);
        lcd.print("move quick      ");
        plotter.quickAbsolute(x,y);
      }
      else if(c1 = strstr(cBuffer, "Z")){
        z = atof(++c1);
        if(z > 0){
          lcd.print("move head mid  ");
          //Serial.println("move head mid");
          plotter.moveHeadMid();
        }
        else{
          lcd.print("move head down");
          //Serial.println("move head down");
          plotter.moveHeadDown();
        }
      }
    }
    else if(strstr(cBuffer, "G01")){
      if((c1 = strstr(cBuffer, "X")) && (c2 = strstr(cBuffer, "Y"))){
        x = atof(++c1);
        y = atof(++c2);
        lcd.print("linear move     ");
        plotter.moveAbsolute(x,y);
      }
      else if(c1 = strstr(cBuffer, "Z")){
        z = atof(++c1);
        if(z > 0){
          lcd.print("move head mid  ");
          //Serial.println("move head mid");
          plotter.moveHeadMid();
        }
        else{
          lcd.print("move head down");
          //Serial.println("move head down");
          plotter.moveHeadDown();
        }
      }
    }
    else if(strstr(cBuffer, "G02")){
      if((c1 = strstr(cBuffer, "X")) && (c2 = strstr(cBuffer, "Y")) && (c3 = strstr(cBuffer, "I")) && (c4 = strstr(cBuffer, "J"))){
        x = atof(++c1);
        y = atof(++c2);
        i = atof(++c3);
        j = atof(++c4);
        lcd.print("arc CW         ");
        plotter.arcAbsolute(x,y,i,j, CW);
      }
    }
    else if(strstr(cBuffer, "G03")){
      //Serial.println(cBuffer);
      if((c1 = strstr(cBuffer, "X")) && (c2 = strstr(cBuffer, "Y")) && (c3 = strstr(cBuffer, "I")) && (c4 = strstr(cBuffer, "J"))){
        x = atof(++c1);
        y = atof(++c2);
        i = atof(++c3);
        j = atof(++c4);
        lcd.print("arc CCW        ");
        //Serial.println(y,6);
        plotter.arcAbsolute(x,y,i,j, CCW);
      }
    }
    else{
      count--;
      lcd.print("invalid command"); 
    }
   
    
    lcd.setCursor(0,2);
    sprintf(lcdBuffer, "%d ops", count++);
    lcd.print(lcdBuffer);
    
    lcd.setCursor(0,3);
    sprintf(lcdBuffer, "time %ds", (millis() - startTime) / 1000);
    lcd.print(lcdBuffer);
    
    if(buttons.getButtonEvent() != BUTTON_NONE){
      pauseTime = millis();
      plotter.moveHeadUp();
      lcd.setCursor(0,1);
      lcd.print("                   ");
      lcd.setCursor(0,2);
      lcd.print("paused             ");
      lcd.setCursor(0,3);
      lcd.print("                   ");
      while(buttons.getButtonEvent() != BUTTON_NONE){
        delayMicroseconds(1);
      }
      startTime += millis() - pauseTime;
    }
  }
  lcd.setCursor(0,1);
  lcd.print("                   ");
  lcd.setCursor(0,2);
  lcd.print("done:              ");
  lcd.setCursor(0,3);
  sprintf(lcdBuffer, "%d moves, %ds      ", count, (millis() - startTime) / 1000);
  lcd.print(lcdBuffer);
  //Serial.print("end");
  
  while(buttons.getButtonEvent() != BUTTON_NONE){
    delayMicroseconds(1);
  }
      
  lcd.clear();
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
  Serial.setTimeout(20);
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
            if(plotter.z_axis.getPosition() == UP){
              plotter.moveHeadDown();
            }
            else if(plotter.z_axis.getPosition() == MID)
              plotter.moveHeadUp();
            else{
              plotter.moveHeadMid();
            }
            break;
          case 1:
            calibrate();
            break;
          case 2:
            executeGCode();
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
            drawSinc(2000l);
            break;
          case 1:
            drawCircle(5000);
            break;
          case 2:
            drawSquare(50.0);
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
