#ifndef buttons_h
#define buttons_h

#include "plotter.h"

#define BUTTON_NONE 	0

class Buttons{
  private:
    int buttonEvent;
    int lastPressed;
    int lastPressed0;
    int state0;
    int retriggerCounter;
  public:
    Buttons();
    void isr();
    int getButtonEvent();
    void clearEvent();
    int isPressed();
};


#endif
