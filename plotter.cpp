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
  char cBuffer[100];
  int i;
  int x = x_axis.getPosition();
  int y = y_axis.getPosition();
  int x0 = x;
  int y0 = y;
  int d_x = setX - x0;
  int d_y = setY - y0;
  double slope = (double) d_x / (double) d_y;
  
  //sprintf(cBuffer, "\nmove (%d, %d) -> (%d, %d)", x, y, setX, setY);
  //Serial.println(cBuffer);

  // update mm coordinates
  position = {setX * Y_STEPSIZE, setY * Y_STEPSIZE};
  
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
      
      //sprintf(cBuffer, "(%d, %d)", x0 + (int) ((y-y0) * slope), y);
      //Serial.println(cBuffer);
    }
  }
  x_axis.setPosition(setX);
  

  
  while(x_axis.getDirection() != IDLE){delayMicroseconds(1);}
}

void Plotter::quickAbsolute(int setX, int setY){
  x_axis.setPosition(setX);
  y_axis.setPosition(setY);
  while(x_axis.getDirection() != IDLE){delayMicroseconds(1);}
}

void Plotter::moveRelative(int x, int y){
  moveAbsolute(x_axis.getPosition() + x, y_axis.getPosition() + y);  
}

position_t Plotter::getPosition(){
  return position; 
}

void Plotter::moveAbsolute(double setX, double setY){
  //char cBuffer[100];
  
  //sprintf(cBuffer, "\nmove (%d, %d) -> (%d, %d)", (long) getPosition().x, (long) getPosition().y, (long) setX, (long) setY);
  //Serial.println(cBuffer);
  
  position = {setX, setY};
  moveAbsolute((int) (setX / Y_STEPSIZE), (int) (setY / Y_STEPSIZE));
  
  //sprintf(cBuffer, "\ndone (%d, %d)", getPosition().x, getPosition().y);
  //Serial.println(cBuffer);
}

void Plotter::quickAbsolute(double setX, double setY){
  position = {setX, setY};
  quickAbsolute((int) (setX / Y_STEPSIZE), (int) (setY / Y_STEPSIZE));
}

void Plotter::arcAbsolute(int x3, int y3, long i, long j, int direction){
  char cBuffer[100];
  long x0, y0; // origin
  int x1, y1; // start
  int x2, y2; // position
  long dx, dy; // position relative to the origin
  double phi; // angle of position
  double radius;
  double x2_d;

  x1 = x2 = x_axis.getPosition();
  y1 = y2 = y_axis.getPosition();

  x0 = x1 + i;
  y0 = y1 + j;
  
  dx = x2 - x0;
  dy = y2 - y0;
  
  //sprintf(cBuffer, "\nx = %d, y = %d, i = %ld, j = %ld\n", x1, y1, i, j);
  //sprintf(cBuffer, "\nstart(%d, %d), end(%d, %d), O(%ld, %ld), I(%ld, %ld), d(%ld, %ld)",  x1, y1, x3, y3, x0, y0, i, j, dx, dy);
  //Serial.print(cBuffer);

  radius = sqrt(pow((x3 - x0), 2) + pow((y3 - y0), 2));

  //Serial.println("\nStart\n");

  //while(!(abs(x2-x3) < 2 && y2 == y3)){
  while(!(y2 == y3 && (SIGN(x2) == SIGN(x3) || SIGN(x3) == ZERO))){
    
    dx = x2 - x0;
    dy = y2 - y0;
    
    if(direction == CCW && (dx > 0 || (dx == 0 && dy < 0))){
      //Serial.print("[CCW RH] ");
      y2++;
      y_axis.stepUp();
      dy = y2 - y0;
      if(dy > radius)
        x2 = x0;
      else{
        phi = asin(dy / radius);
        x2 = x0 + (long) (radius * cos(phi));
      }
      x2_d = x0 + (radius * cos(phi));   
      x_axis.setPosition(x2);
    } 
    else if(direction == CCW && (dx < 0 || (dx == 0 && dy > 0))){
      //Serial.print("[CCW LH] ");
      y2--;
      y_axis.stepDown();
      dy = y2 - y0;
      if(dy < -radius)
        x2 = x0;
      else{
        phi = asin(dy / radius);
        x2 = x0 - (long) (radius * cos(phi));
      }
      x2_d = x0 - (radius * cos(phi));
      x_axis.setPosition(x2);
    }
    else if(direction == CW && (dx > 0 || (dx == 0 && dy > 0))){
      //Serial.print("[CW RH] ");
      y2--;
      y_axis.stepDown();
      dy = y2 - y0;
      if(dy > radius)
        x2 = x0;
      else{
        phi = asin(dy / radius);
        x2 = x0 + (long) (radius * cos(phi));
      }
      x2_d = x0 + (radius * cos(phi));
      x_axis.setPosition(x2);
    }
    else if(direction == CW && (dx < 0 || (dx == 0 && dy < 0))){
      //Serial.print("[CW LH] ");
      y2++;
      y_axis.stepUp();
      dy = y2 - y0;
      if(dy < -radius)
        x2 = x0;
      else{
        phi = asin(dy / radius);
        x2 = x0 - (long) (radius * cos(phi));
      }
      x2_d = x0 - (radius * cos(phi));
      x_axis.setPosition(x2);
    }
    
//   sprintf(cBuffer, "d(%ld, %ld), pos(%d, %d), end(%d, %d) ", dx, dy, x2, y2, x3, y3);
//   Serial.print(cBuffer);
//   Serial.print(radius, 3);
//   Serial.print(", ");
//   Serial.println(phi, 3);
  }
}


void Plotter::arcAbsolute(double setX, double setY, double i, double j, int direction){
  //char cBuffer[100];
  //sprintf(cBuffer, "\nARC: dO(%ld, %ld) set(%ld, %ld)", (long) i, (long) j, (long) setX, (long) setY);
  //Serial.print(cBuffer);
  arcAbsolute((long) (setX / Y_STEPSIZE), (long) (setY / Y_STEPSIZE), (long) (i / Y_STEPSIZE), (long) (j / Y_STEPSIZE), direction);
  
}


void Plotter::moveHeadUp(){
  //Serial.println("f: move head up");
  z_axis.setPosition(UP);
}

void Plotter::moveHeadDown(){
  //Serial.println("f: move head down");
  z_axis.setPosition(DOWN);
}

void Plotter::moveHeadMid(){
  //Serial.println("f: move head mid");
  z_axis.setPosition(MID);
}
