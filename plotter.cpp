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

void Plotter::moveAbsolute(double setX, double setY){
  position = {setX, setY};
  moveAbsolute((int) (setX / Y_STEPSIZE), (int) (setY / Y_STEPSIZE));
}

void Plotter::quickAbsolute(double setX, double setY){
  position = {setX, setY};
  quickAbsolute((int) (setX / Y_STEPSIZE), (int) (setY / Y_STEPSIZE));
}

void Plotter::arcAbsoluteCW(int setX, int setY, double i, double j){
  char cBuffer[100];
  double angle;
  int corner;
  int step;
  int d_x, d_y;
  int x0 = x_axis.getPosition();
  int y0 = y_axis.getPosition();
  double radius = sqrt(pow(i,2) + pow(j,2));
  position_t origin = {(double) x0 + i, (double) y0 + j};
  
  sprintf(cBuffer, "origin = (%d,%d), radius = ", (int) origin.x, (int) origin.y);
  Serial.print(cBuffer);
  Serial.println(radius, 3);
  
  if(i <= 0.0 && j < 0.0)
    corner = 0;
  else if(i > 0.0 && j <= 0.0)
    corner = 1;
  else if(i >= 0.0 && j > 0.0)
    corner = 2;
  else 
    corner = 3;
      
  step = -j;
  
  
  while(x_axis.getPosition() != setX || y_axis.getPosition() != setY){    
    switch(corner){
      case 0:
        step--;
        y_axis.stepDown();
        if(step == 0){
          x_axis.setPosition(origin.x + radius);
          corner = 3;
        }else{
          angle = asin((double) step / radius);
          x_axis.setPosition(origin.x + (radius * cos(angle)));          
        }
        break;
      case 1:
        step++;
        y_axis.stepUp();
        if(step == (int) radius){
          x_axis.setPosition(origin.x);
          corner = 0;
        }else{
          angle = asin((double) step / radius);
          x_axis.setPosition(origin.x - (radius * cos(angle)));          
        }
        break;
      case 2:
        step++;
        y_axis.stepUp();
        if(step == 0){
          x_axis.setPosition(origin.x - radius);
          corner = 1;
        }else{
          angle = asin((double) step / radius);
          x_axis.setPosition(origin.x - (radius * cos(angle)));
        }
        break;      
      default:
        step--;
        y_axis.stepDown();
        if(step == (int) -radius){
          x_axis.setPosition(origin.x);
          corner = 2;
        }else{
          angle = asin((double) step / radius);
          x_axis.setPosition(origin.x + (radius * cos(angle)));
        }
    }
    sprintf(cBuffer, "%d %d (%d,%d) (%d,%d) ", corner, step, x_axis.getPosition(), y_axis.getPosition(), setX, setY);
    Serial.print(cBuffer);
    Serial.println(angle, 5);
    //Serial.println(radius, 5);
  }
}

void Plotter::arcAbsoluteCCW(int setX, int setY, double i, double j){
  char cBuffer[100];
  double angle;
  int corner;
  int step;
  int d_x, d_y;
  int x0 = x_axis.getPosition();
  int y0 = y_axis.getPosition();
  double radius = sqrt(pow(i,2) + pow(j,2));
  position_t origin = {(double) x0 + i, (double) y0 + j};
  
  sprintf(cBuffer, "origin = (%d,%d), radius = ", (int) origin.x, (int) origin.y);
  Serial.print(cBuffer);
  Serial.println(radius, 3);
  
  if(i < 0.0 && j <= 0.0)
    corner = 0;
  else if(i >= 0.0 && j < 0.0)
    corner = 1;
  else if(i > 0.0 && j >= 0.0)
    corner = 2;
  else 
    corner = 3;
      
  step = -j;
  
  
  while(x_axis.getPosition() != setX || y_axis.getPosition() != setY){    
    switch(corner){
      case 0:
        step++;
        y_axis.stepUp();
        if(step == (int) radius){
          x_axis.setPosition(origin.x);
          corner = 1;
        }else{
          angle = asin(step / radius);
          x_axis.setPosition(origin.x + (radius * cos(angle)));          
        }
        break;
      case 1:
        step--;
        y_axis.stepDown();
        if(step == 0){
          x_axis.setPosition(origin.x - radius);
          corner = 2;
        }else{
          angle = asin(step / radius);
          x_axis.setPosition(origin.x - (radius * cos(angle)));          
        }
        break;
      case 2:
        step--;
        y_axis.stepDown();
        if(step == (int) -radius){
          x_axis.setPosition(origin.x);
          corner = 3;
        }else{
          angle = asin(step / radius);
          x_axis.setPosition(origin.x - (radius * cos(angle)));
        }
        break;      
      default:
        step++;
        y_axis.stepUp();
        if(step == 0){
          x_axis.setPosition(origin.x + radius);
          corner = 0;
        }else{
          angle = asin(step / radius);
          x_axis.setPosition(origin.x + (radius * cos(angle)));
        }
    }

//    Serial.print("angle = asin((double)step/radius) => ");
//    Serial.print(angle,3);
//    Serial.print(" = asin(");
//    Serial.print((double) step, 3);
//    Serial.print("/");
//    Serial.print(radius,3);
//    Serial.println(")");
    
//    sprintf(cBuffer, "%d %d (%d,%d) (%d,%d) ", corner, step, x_axis.getPosition(), y_axis.getPosition(), setX, setY);
//    Serial.print(cBuffer);
//    Serial.println(angle, 5);
//    Serial.println(radius, 5);
    
  }
}

void Plotter::arcAbsoluteCW(double setX, double setY, double i, double j){
  
  arcAbsoluteCW((int) (setX / Y_STEPSIZE), (int) (setY / Y_STEPSIZE), i / Y_STEPSIZE, j / Y_STEPSIZE);
  
}

void Plotter::arcAbsoluteCCW(double setX, double setY, double i, double j){
  
  arcAbsoluteCCW((int) (setX / Y_STEPSIZE), (int) (setY / Y_STEPSIZE), i / Y_STEPSIZE, j / Y_STEPSIZE);
  
}

void Plotter::moveHeadUp(){
  z_axis.setPosition(UP);
}

void Plotter::moveHeadDown(){
  z_axis.setPosition(DOWN);
}
