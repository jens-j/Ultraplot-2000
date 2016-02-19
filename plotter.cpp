#include "Arduino.h"
#include "ultraPlot2000.h"
#include "axis.h"

Plotter::Plotter(){
  x_axis = X_axis();
  y_axis = Y_axis();
  z_axis = Z_axis();
  position = {0.0, 0.0};
}


void Plotter::moveAbsolute(int setX, int setY){
  int i;
  int x = x_axis.getPosition();
  int y = y_axis.getPosition();
  int x0 = x;
  int y0 = y;
  int d_x = setX - x0;
  int d_y = setY - y0;
  double slope = (double) d_x / (double) d_y;

  // update mm coordinates
  position = {setX * X_STEPSIZE, setY * Y_STEPSIZE};
  
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

void Plotter::quickAbsolute(int setX, int setY){
  x_axis.setPosition(setX);
  y_axis.setPosition(setY);
}

void Plotter::moveRelative(int x, int y){
  moveAbsolute(x_axis.getPosition() + x, y_axis.getPosition() + y);  
}

void Plotter::moveAbsolute(double setX, double setY){
  moveAbsolute((int) setX / X_STEPSIZE, (int) setY / Y_STEPSIZE);
}

void Plotter::arcAbsoluteCW(double setX, double setY, double i, double j){
  double r, r2;
  double angle;
  int x, y;
  int x0, y0;
  int x_pos;
  char cbuffer[20] = "";
 
  x = x_axis.getPosition();
  y = y_axis.getPosition();
  x0 = x;
  y0 = y;
  r = sqrt(pow(i,2) + pow(j,2));
  r2 = sqrt(pow(setX - (x + i), 2) + pow(setY - (y + j), 2));
  
  if(r != r2){
    sprintf(cbuffer, "arc: %3.2f, %3.2f", r, r2);
    panic(cbuffer);
  }
  
  while(x != setX || y != setY){
    if(x >= x0 + i && y >= y0 + j){
      y_axis.stepDown();
      y--;
      angle = asin((y - (x0 + j)) / r);
      //x_pos = 
    }  
  }
}

void Plotter::moveHeadUp(){
  z_axis.setPosition(UP);
}

void Plotter::moveHeadDown(){
  z_axis.setPosition(DOWN);
}
