#include "plotter.h"
#include "buttons.h"


Buttons::Buttons(){
  lastPressed = BUTTON_NONE;  
  state0 = 0;
}

void Buttons::isr(){
  int i;
  int state = 0;
    
  
  if(digitalRead(BUTTON_UP) == LOW){
    state |= 0x01;
    if(!(state0 & 0x01)){
      lastPressed = BUTTON_UP;
    }
  }
  if(digitalRead(BUTTON_DOWN) == LOW){
    state |= 0x02;
    if(!(state0 & 0x02))
      lastPressed = BUTTON_DOWN;
  }
  if(digitalRead(BUTTON_LEFT) == LOW){
    state |= 0x04;
    if(!(state0 & 0x04))
      lastPressed = BUTTON_LEFT;
  }
  if(digitalRead(BUTTON_RIGHT) == LOW){
    state |= 0x08;
    if(!(state0 & 0x08))
      lastPressed = BUTTON_RIGHT;
  }
  if(digitalRead(BUTTON_MID) == LOW){
    state |= 0x10;
    if(!(state0 & 0x10))
      lastPressed = BUTTON_MID;
  }

  state0 = state;
};

int Buttons::getLastPressed(){
  int ret = lastPressed;
  lastPressed = BUTTON_NONE;
  return ret; 
}
