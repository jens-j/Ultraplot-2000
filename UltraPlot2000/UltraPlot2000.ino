#include <avr/wdt.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
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
  plotter.x_axis.sensorIsr();
}


ISR(WDT_vect){
  plotter.x_axis.wdTimerIsr();
}
  

void timerIsrDispatcher(){
  buttons.isr(); 
}


void panic(char *s){
  z_position_t head_pos;
  
  // disable interrupts
  WDTCSR &= ~(1<<WDIE); 
  detachInterrupt(digitalPinToInterrupt(SENSOR_X0));
  detachInterrupt(digitalPinToInterrupt(SENSOR_X1));
  
  // stop X axis motor
  digitalWrite(MOTOR_X0, LOW);
  digitalWrite(MOTOR_X1, LOW);
  
  // retract head
  head_pos = plotter.z_axis.getPosition();
  plotter.moveHead(Z_UP);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[Panic!]");
  lcd.setCursor(0, 2);
  lcd.print(s);
  
  // pause
  void clearEvent();
  while(buttons.getButtonEvent() != BUTTON_MID){
    delayMicroseconds(1);
  }
  
  // restore state
  lcd.clear();
  plotter.moveHead(head_pos);
  attachInterrupt(digitalPinToInterrupt(SENSOR_X0), sensorIsrDispatcher, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SENSOR_X1), sensorIsrDispatcher, CHANGE);
  if( plotter.x_axis.getPosition() != plotter.x_axis.getSetPoint() ){
    plotter.x_axis.setPosition( plotter.x_axis.getSetPoint() );
  }
}


void calibrate(){
  int xbound0, xbound1;
  int ybound0, ybound1;
  int xrange, yrange;
  unsigned long refresh = 0;
  
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[Calibration mode]");
  
  plotter.moveHead(Z_MID);
  
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
  //Serial.println(xrange / 2);
  plotter.quickAbsolute( (int) ((xrange * XY_SCALE) / 2), plotter.y_axis.getPosition() );
  //delay(3);
  //Serial.println(plotter.x_axis.getRealPosition());
  
  
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
  plotter.y_axis.setPosition( plotter.x_axis.getPosition() + (160 / Y_STEPSIZE) ); // move up 16 cm
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
  
  plotter.moveHead(Z_UP);
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
  long int count = 1;
  int seconds, minutes, hours;
  unsigned long pauseTime;
  unsigned long startTime = millis(); 
  z_position_t pausePos;
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[Executing GCode]");
  
  plotter.moveHead(Z_UP);

//  int k = 0;
//  char test[6][100] = {   "G00 X77.583856 Y176.185749",
//                          "G01 Z-0.125000 F100.0(Penetrate)",
//                          "G03 X77.760509 Y176.412419 Z-0.125000 I-0.017843 J0.196078",
//                          "G03 X77.543226 Y176.596975 Z-0.125000 I-0.215832 J-0.033921",
//                          "G02 X77.269796 Y176.608211 Z-0.125000 I-0.019060 J2.868845",
//                          "G00 Z5.000000"
//                          };
  
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
          lcd.print("move head up   ");
          //Serial.println("move head mid");
          plotter.moveHead(Z_LOW);
        }
        else{
          lcd.print("move head down");
          //Serial.println("move head down");
          plotter.moveHead(Z_DOWN);
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
          lcd.print("move head up   ");
          //Serial.println("move head up");
          plotter.moveHead(Z_LOW);
        }
        else{
          lcd.print("move head down");
          //Serial.println("move head down");
          plotter.moveHead(Z_DOWN);
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
    sprintf(lcdBuffer, "%ld ops", count++);
    lcd.print(lcdBuffer);
    
    // calculate & print elapsed time
    seconds = millis() / 1000;
    hours = seconds / 3600;
    minutes = (seconds / 60) % 60;
    seconds = seconds % 60;
    lcd.setCursor(0,3);
    sprintf(lcdBuffer, "time: %02d:%02d:%02d", hours, minutes, seconds);
    lcd.print(lcdBuffer);

    // pause
    if(buttons.getButtonEvent() != BUTTON_NONE){
      pauseTime = millis();
      while(plotter.x_axis.getDirection() != IDLE){} // finish last move
      pausePos = plotter.z_axis.getPosition();
      plotter.moveHead(Z_UP);
      lcd.setCursor(0,1);
      lcd.print("                   ");
      lcd.setCursor(0,2);
      lcd.print("paused             ");
      lcd.setCursor(0,3);
      lcd.print("                   ");
      while(buttons.getButtonEvent() == BUTTON_NONE){
        delayMicroseconds(1);
      }
      startTime += millis() - pauseTime;
      plotter.moveHead(pausePos);
      lcd.setCursor(0,2);
      lcd.print("                   ");
    }
  }
  
  // Done
  plotter.moveHead(Z_UP);
  
  lcd.setCursor(0,1);
  lcd.print("                   ");
  lcd.setCursor(0,2);
  lcd.print("done:              ");
  lcd.setCursor(0,3);
  sprintf(lcdBuffer, "%ld moves, %ds      ", count, (millis() - startTime) / 1000);
  lcd.print(lcdBuffer);
  
  while(buttons.getButtonEvent() != BUTTON_NONE){
    delayMicroseconds(1);
  }
      
  lcd.clear();
}



void setup(){
  char buffer[100];
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
  
  Serial.begin(115200);
  Serial.setTimeout(20);
  
  // set up WDT interrupt
  wdt_reset();
  WDTCSR |= (1<<WDCE) | (1<<WDE); // change enable
  WDTCSR = (1<<WDP3) | (1<<WDP0); // timeout = 8 seconds 
  
  // attach external interrupt for the sensor
  attachInterrupt(digitalPinToInterrupt(SENSOR_X0), sensorIsrDispatcher, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SENSOR_X1), sensorIsrDispatcher, CHANGE);
  
  // intitialize timer interrupt for the buttons
  Timer1.initialize(BTN_DEBOUNCE_P * 1000); 
  Timer1.disablePwm(MOTOR_X1);
  Timer1.attachInterrupt(timerIsrDispatcher);

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
            if(plotter.z_axis.getPosition() == Z_UP){
              plotter.moveHead(Z_DOWN);
            }
            else{
              plotter.moveHead(Z_UP);
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
