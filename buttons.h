#ifndef buttons_h
#define buttons_h

#include "plotter.h"

#define BUTTON_NONE 	0

class Buttons{
  private:
    int lastPressed;
    int state0;
  public:
    Buttons();
    void isr();
    int getLastPressed();
};


#endif
