#include <avr/wdt.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "TimerOne.h"
#include "ultraPlot2000.h"
#include "plotter.h"
#include "axis.h"
#include "buttons.h"
#include "shapes.h"


const int  N_MENU_ENTRIES[2] = {9, 4};

const char MAIN_MENU_TEXT[9][20] = {"Toggle head        ",
                                    "Print State        ",
                                    "Reset Diagnostics  ",
                                    "Save State         ",
                                    "Restore State      ",
                                    "Movement Control   ",
                                    "Calibrate          ",  
                                    "GCode              ", 
                                    "Draw               "};
                                    
const char DRAW_MENU_TEXT[4][20] = {"Sinc               ", 
                                    "Circle             ", 
                                    "Square             ",
                                    "Back               "};

char lcdBuffer[20];
char cBuffer[100];
menuSel_t menuSel = MENU_MAIN;
int menuPosition = 0;
int menuWindowPosition = 0;


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


void printStatus(){
  int diag[3];
  char buffer[20];
  
  plotter.x_axis.getDiagnostics(&diag[0]);
  lcd.clear();
  
  // BOUNDS & DARWING AREA SIZE
  lcd.setCursor(0, 0);
  lcd.print("[Bounds & Size]");
  
  lcd.setCursor(0, 1);
  sprintf(buffer, "x [%d,%d]", plotter.x_axis.getBounds().b0, plotter.x_axis.getBounds().b1);
  lcd.print(buffer);
  
  lcd.setCursor(0, 2);
  sprintf(buffer, "y [%d,%d]", plotter.y_axis.getBounds().b0, plotter.y_axis.getBounds().b1);
  lcd.print(buffer);
  
  lcd.setCursor(0, 3);
  sprintf(buffer, "%umm x %umm ", (unsigned int) ( (plotter.x_axis.getBounds().b1 - plotter.x_axis.getBounds().b0) * X_STEPSIZE), 
                                    (unsigned int) ( (plotter.y_axis.getBounds().b1 - plotter.y_axis.getBounds().b0) * Y_STEPSIZE));
  lcd.print(buffer);
  
  buttons.clearEvent();
  while( buttons.getButtonEvent() != BUTTON_MID ){
    delayMicroseconds(1); 
  }
  
  // X, Y, Z POSITIONS
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[Position]");
  
  lcd.setCursor(0, 1);
  sprintf(buffer, "x = %d", plotter.x_axis.getRealPosition());
  lcd.print(buffer);
  
  lcd.setCursor(0, 2);
  sprintf(buffer, "y = %d", plotter.y_axis.getPosition());
  lcd.print(buffer);
  
  lcd.setCursor(0, 3);
  sprintf(buffer, "z = %s (%d)", plotter.z_axis.posToString().c_str(), plotter.z_axis.getPosition());
  lcd.print(buffer);
   
  buttons.clearEvent();
  while( buttons.getButtonEvent() != BUTTON_MID ){
    delayMicroseconds(1); 
  }
  
  // DIAGNOSTICS
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[Diagnostics]");
  
  lcd.setCursor(0, 1);
  sprintf(buffer, "overshoot = %8d", diag[0]);
  lcd.print(buffer);
  
  lcd.setCursor(0, 2);
  sprintf(buffer, "stall     = %8d", diag[1]);
  lcd.print(buffer);
  
  lcd.setCursor(0, 3);
  sprintf(buffer, "retrigger = %8d", diag[2]);
  lcd.print(buffer);
  
  buttons.clearEvent();
  while( buttons.getButtonEvent() != BUTTON_MID ){
    delayMicroseconds(1); 
  }
  
  lcd.clear();
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

  Serial.print("PANIC: ");
  Serial.println(s);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[Panic!]");
  lcd.setCursor(0, 2);
  lcd.print(s);
  
  plotter.saveState();
    
  // pause
  buttons.clearEvent();
  while( buttons.getButtonEvent() != BUTTON_MID ){
    delay(1); 
  }
  
  printStatus(); // this blocks until a buttons is pressed
    
  // restore state (this doesn't really work in any situation)
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
  
  plotter.quickAbsolute( (int) ((xrange * XY_SCALE) / 2), plotter.y_axis.getPosition() );
   
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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[Calibration mode]");
  lcd.setCursor(0, 2);
  lcd.print("wait...");
  plotter.y_axis.setPosition( plotter.x_axis.getPosition() + (160 / Y_STEPSIZE) ); // move up 16 cm
  lcd.setCursor(0, 2);
  lcd.print("set upper y bound");
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
  
  plotter.saveState();
  plotter.moveHead(Z_UP);
  printStatus();
  
  lcd.clear();
}


void printMenu(){
  int i, j;
  const char (*menu)[20];
  
  if(menuSel == MENU_MAIN){
    menu = &MAIN_MENU_TEXT[0];
  }
  if(menuSel == MENU_DRAW){
    menu = &DRAW_MENU_TEXT[0];
  }
  
  //lcd.clear();
  for(i = 0; i < 4; i++){
    lcd.setCursor(0, i);
    j = i + menuWindowPosition;
    if(j == menuPosition){
      lcd.print(">");
      lcd.print(menu[j]);
    }
    else{
      lcd.print(" ");
      lcd.print(menu[j]);
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
  
  while(1){
    Serial.println("next");
    
    while(Serial.available() == 0){
      delayMicroseconds(1); 
    }
    
    s = Serial.readString(); 
    s.toCharArray(cBuffer, 100);
    
    if(strstr(cBuffer, "%")){
      break;
    }

    // echo the string
    if(DEBUG)
      Serial.println(s);

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
          plotter.moveHead(Z_LOW);
        }
        else{
          lcd.print("move head down");
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
          plotter.moveHead(Z_LOW);
        }
        else{
          lcd.print("move head down");
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
      if((c1 = strstr(cBuffer, "X")) && (c2 = strstr(cBuffer, "Y")) && (c3 = strstr(cBuffer, "I")) && (c4 = strstr(cBuffer, "J"))){
        x = atof(++c1);
        y = atof(++c2);
        i = atof(++c3);
        j = atof(++c4);
        lcd.print("arc CCW        ");
        plotter.arcAbsolute(x,y,i,j, CCW);
      }
    }
    else{
      count--;
      lcd.print("invalid command"); 
    }
    
    // print LCD header (required because pausing will draw other stuff on the LCD)
    lcd.setCursor(0, 0);
    lcd.print("[Calibration mode]");
 
    // print number op performed operations
    lcd.setCursor(0,2);
    sprintf(lcdBuffer, "%ld operations", count++);
    lcd.print(lcdBuffer);
    
    // calculate & print elapsed time
    seconds = millis() / 1000;
    hours = seconds / 3600;
    minutes = (seconds / 60) % 60;
    seconds = seconds % 60;
    lcd.setCursor(0,3);
    sprintf(lcdBuffer, "run time: %02d:%02d:%02d", hours, minutes, seconds);
    lcd.print(lcdBuffer);
    

    // pause
    if(buttons.getButtonEvent() != BUTTON_NONE){
      pauseTime = millis();
      while(plotter.x_axis.getDirection() != IDLE){} // finish last move
      pausePos = plotter.z_axis.getPosition();
      plotter.moveHead(Z_UP);

      printStatus();
      
      buttons.clearEvent();
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
  WDTCSR = (1<<WDP2); // timeout = 250 ms 
  //WDTCSR = (1<<WDP1); // timeout = 64 ms 
  
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
    if( menuPosition < N_MENU_ENTRIES[menuSel]-1 ){
      menuPosition++;
    }
    if( menuPosition > menuWindowPosition + 3 ){
      menuWindowPosition++;
    }
  }
  if(command == BUTTON_UP){
    if( menuPosition > 0 ){
      menuPosition--;
    }
    if( menuPosition < menuWindowPosition ){
      menuWindowPosition--;
    }
  }
  if(command == BUTTON_MID){   
    switch(menuSel){
      
      case MENU_MAIN:
        switch(menuPosition){
          
          case 0: // TOGGLE HEAD
            if(plotter.z_axis.getPosition() == Z_UP){
              plotter.moveHead(Z_DOWN);
            }
            else{
              plotter.moveHead(Z_UP);
            }
            break;
            
          case 1: // PRINT DIAGNOSTICS
            printStatus();
            break;
            
          case 2: // RESET DIAGNOSTICS 
            plotter.x_axis.resetDiagnostics();
            
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Cleared diagnostics");
            
            buttons.clearEvent();
            while(buttons.getButtonEvent() == BUTTON_NONE){
              delayMicroseconds(1);
            }
            break;
            
          case 3: // SAVE STATE
            plotter.saveState();
            
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("saved to EEPROM");
            
            buttons.clearEvent();
            while(buttons.getButtonEvent() == BUTTON_NONE){
              delayMicroseconds(1);
            }
            
            break;
            
          case 4: // RESTORE STATE
            plotter.restoreState();
            
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("restored from EEPROM");
            
            buttons.clearEvent();
            while(buttons.getButtonEvent() == BUTTON_NONE){
              delayMicroseconds(1);
            }
            
            printStatus();
            
            break;
          
          case 5: // MOVEMENT CONTROL         
            break;
          
          case 6: // CALIBRATE
            calibrate();
            break;
            
          case 7: // GCODE
            executeGCode();
            break;
            
          case 8: // DRAW
            menuSel = MENU_DRAW;
            menuPosition = 0;
            menuWindowPosition = 0;
            //lcd.clear();
            break;
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
            menuWindowPosition = 0;
            //lcd.clear();
            break;
        }
        break;
    }
  }
  
  printMenu();

}
