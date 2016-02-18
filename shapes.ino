#include "plotter.h"
#include "axis.h"

extern X_axis x_axis;
extern Y_axis y_axis;
extern Z_axis z_axis;

void drawSinc(int size){
  int i;
  int y;
  
  z_axis.setPosition(UP);
  x_axis.setPosition(-size);
  z_axis.setPosition(DOWN);
  
  for(i = -size; i < size; i++){
    x_axis.stepRight();
    if(i != 0){
      y = ((size / 2) * sin((2 * 3.141 * i) / ((double) size / 10))) / (i / ((double) size / 10));
    }
    y_axis.setPosition(y);
  }
  
  z_axis.setPosition(UP);
  x_axis.setPosition(0);
}

void drawCircle(int diameter){
int i;
  double angle;
  int x_pos;
  
  diameter = diameter / 2;
  
  z_axis.setPosition(UP);
  x_axis.setPosition(diameter * YX_SCALE);
  z_axis.setPosition(DOWN);
  
  for(i = 1; i <= diameter; i++){
    y_axis.stepUp();
    angle = asin((double) i / (double) diameter);
    x_pos = diameter * cos(angle) * YX_SCALE;
    x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);
  } 

  for(i = diameter; i >= 1; i--){
    y_axis.stepDown();
    angle = asin((double) i / (double) diameter);
    x_pos = -1 * diameter * cos(angle) * YX_SCALE;
    x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);
  } 
  
  for(i = 1; i <= diameter; i++){
    y_axis.stepDown();
    angle = asin((double) i / (double) diameter);
    x_pos = -1 * diameter * cos(angle) * YX_SCALE;
    x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);    
  } 
  
  for(i = diameter; i >= 1; i--){
    y_axis.stepUp();
    angle = asin((double) i / (double) diameter);
    x_pos = diameter * cos(angle) * YX_SCALE;
    x_axis.setPosition(x_pos);
    Serial.print(-1 * i);
    Serial.print(",");
    Serial.println(x_pos);
  }
  
  z_axis.setPosition(UP);
  x_axis.setPosition(0);
}

void drawSquare(int size){
size = size / 2;
  z_axis.setPosition(UP);
  x_axis.setPosition(-1 * size);
  y_axis.setPosition(-1 * size * XY_SCALE);
  z_axis.setPosition(DOWN);
  x_axis.setPosition(size);  
  y_axis.setPosition(size * XY_SCALE);
  x_axis.setPosition(-1 * size);
  y_axis.setPosition(-1 * size * XY_SCALE);
  z_axis.setPosition(UP);
  x_axis.setPosition(0);
  y_axis.setPosition(0);  
}


