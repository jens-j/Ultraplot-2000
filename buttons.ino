#include "plotter.h"
#include "buttons.h"


Buttons::Buttons(){
  lastPressed0 = BUTTON_NONE;  
  buttonEvent = BUTTON_NONE;
  state0 = 0;
  retriggerCounter = 0;
}

void Buttons::isr(){
  int i;
  int state = 0;
    
  lastPressed = BUTTON_NONE;
  if(digitalRead(BUTTON_UP) == LOW){
    state |= 0x01;
    lastPressed = BUTTON_UP;
    if(!(state0 & 0x01)){
      buttonEvent = BUTTON_UP;
      Serial.println("a");
    }
  }
  if(digitalRead(BUTTON_DOWN) == LOW){
    state |= 0x02;
    lastPressed = BUTTON_DOWN;
    if(!(state0 & 0x02)){
      buttonEvent = BUTTON_DOWN;
      Serial.println("b");
    }
  }
  if(digitalRead(BUTTON_LEFT) == LOW){
    state |= 0x04;
    lastPressed = BUTTON_LEFT;
    if(!(state0 & 0x04))
      buttonEvent = BUTTON_LEFT;
  }
  if(digitalRead(BUTTON_RIGHT) == LOW){
    state |= 0x08;
    lastPressed = BUTTON_RIGHT;
    if(!(state0 & 0x08))
      buttonEvent = BUTTON_RIGHT;
  }
  if(digitalRead(BUTTON_MID) == LOW){
    state |= 0x10;
    lastPressed = BUTTON_MID;
    if(!(state0 & 0x10))
      buttonEvent = BUTTON_MID;
  }
  if(lastPressed != BUTTON_NONE && lastPressed == lastPressed0){
    if(++retriggerCounter == BTN_RETRIGGER_P / BTN_DEBOUNCE_P){
      buttonEvent = lastPressed;
      retriggerCounter = 0; 
    }
  }
  else{
    retriggerCounter = 0;
  }
  
  lastPressed0 = lastPressed;
  state0 = state;
};

int Buttons::getButtonEvent(){
  int ret = buttonEvent;
  buttonEvent = BUTTON_NONE;
  return ret; 
}
