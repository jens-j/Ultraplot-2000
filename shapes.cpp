#include "Arduino.h"
#include "ultraPlot2000.h"
#include "plotter.h"
#include "axis.h"

extern Plotter plotter;

void drawSinc(int size){
  int i;
  int y;
  
  plotter.moveHeadUp();
  plotter.quickAbsolute(-size,0);
  plotter.moveHeadDown();
  
  for(i = -size; i < size; i++){
    plotter.x_axis.stepRight();
    if(i != 0){
      y = ((size / 2) * sin((2 * 3.141 * i) / ((double) size / 10))) / (i / ((double) size / 10));
    }
    plotter.y_axis.setPosition(y);
  }
  
  plotter.moveHeadUp();
  plotter.quickAbsolute(0, 0);
}

void drawCircle(int diameter){
int i;
  double angle;
  int x_pos;
  
  diameter = diameter / 2;
  
  plotter.moveHeadUp();
  plotter.quickAbsolute((int) (diameter * YX_SCALE), 0);
  plotter.moveHeadDown();
  
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
  
  plotter.moveHeadUp();
  plotter.quickAbsolute(0, 0);
}

void drawSquare(int size){
  Serial.println("drawSware");
  plotter.quickAbsolute(0.0, 0.0);
  plotter.moveHeadDown();
  plotter.moveAbsolute(100.0, 0.0);
  plotter.moveAbsolute(100.0, 100.0);
  plotter.moveAbsolute(0.0, 100.0);
  plotter.moveAbsolute(0.0, 0.0);
  plotter.moveHeadUp();  
    
//  size = size / 2;
//  plotter.moveHeadUp();
//  plotter.quickAbsolute(-1 * size, -1 * size);
//  plotter.moveHeadDown();
//  plotter.moveAbsolute(size, -1 * size);
//  plotter.moveAbsolute(size, size);
//  plotter.moveAbsolute(-1 * size, size);
//  plotter.moveAbsolute(-1 * size, -1 * size);
//  plotter.moveHeadUp();
//  plotter.quickAbsolute(0, 0);
}


