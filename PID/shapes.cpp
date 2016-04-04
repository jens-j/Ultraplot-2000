#include "Arduino.h"
#include "ultraPlot2000.h"
#include "plotter.h"
#include "axis.h"

extern Plotter plotter;

void drawSinc(int size){
  long i;
  long y;
  
  plotter.moveHead(Z_UP);
  plotter.quickAbsolute(-size,0);
  plotter.moveHead(Z_DOWN);
  
  for(i = -size; i < size; i++){
    plotter.x_axis.setPosition(i);
    if(i != 0){
      y = ((size / 2) * sin((2 * 3.141 * i) / ((double) size / 10))) / (i / ((double) size / 10));
    }
    plotter.y_axis.setPosition(y);
  }
  
  plotter.moveHead(Z_UP);
  plotter.quickAbsolute(0, 0);
}

void drawCircle(int diameter){
int i;
  double angle;
  int x_pos;
  
  diameter = diameter / 2;
  
  plotter.moveHead(Z_UP);
  plotter.quickAbsolute((int) (diameter * YX_SCALE), 0);
  plotter.moveHead(Z_DOWN);
  
  for(i = 1; i <= diameter; i++){
    plotter.y_axis.stepUp();
    angle = asin((double) i / (double) diameter);
    x_pos = diameter * cos(angle);// * YX_SCALE;
    plotter.x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);
  } 

  for(i = diameter; i >= 1; i--){
    plotter.y_axis.stepDown();
    angle = asin((double) i / (double) diameter);
    x_pos = -1 * diameter * cos(angle);// * YX_SCALE;
    plotter.x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);
  } 
  
  for(i = 1; i <= diameter; i++){
    plotter.y_axis.stepDown();
    angle = asin((double) i / (double) diameter);
    x_pos = -1 * diameter * cos(angle);// * YX_SCALE;
    plotter.x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);    
  } 
  
  for(i = diameter; i >= 1; i--){
    plotter.y_axis.stepUp();
    angle = asin((double) i / (double) diameter);
    x_pos = diameter * cos(angle);// * YX_SCALE;
    plotter.x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);
  }
  
  plotter.moveHead(Z_UP); 
  plotter.quickAbsolute(0, 0);
}

void drawSquare(double size){
  Serial.println("drawSware");
  plotter.quickAbsolute(0.0, 0.0);
  plotter.moveHead(Z_DOWN);
  plotter.moveAbsolute(size, 0.0);
  plotter.moveAbsolute(size, size);
  plotter.moveAbsolute(0.0, size);
  plotter.moveAbsolute(0.0, 0.0);
  plotter.moveHead(Z_UP);  
}


