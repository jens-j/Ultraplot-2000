#include "Arduino.h"
#include "ultraPlot2000.h"
#include "axis.h"
#include "EEPROMlib.h"

Plotter::Plotter(){
  x_axis = X_axis();
  y_axis = Y_axis();
  z_axis = Z_axis();
  
  position = {0.0, 0.0};
  
  // lead the position and bounds from EEPROM
  //restoreState();
}


void Plotter::saveState(){
  int address = 0;
  
  romWriteInt( address++, x_axis.getRealPosition() );
  romWriteInt( address++, x_axis.getBounds().b0 );
  romWriteInt( address++, x_axis.getBounds().b1 );
  romWriteInt( address++, y_axis.getPosition() );
  romWriteInt( address++, y_axis.getBounds().b0 );
  romWriteInt( address++, y_axis.getBounds().b1 );
  romWriteInt( address++, (int) z_axis.getPosition() );
}


void Plotter::restoreState(){
  int address = 0;
 
  x_axis.initPosition( romReadInt(address++) );
  x_axis.setBounds( {romReadInt(address), romReadInt(address+1)} );
  address += 2;
  y_axis.initPosition( romReadInt(address++) );
  y_axis.setBounds( {romReadInt(address), romReadInt(address+1)} );
  address += 2;
  z_axis.initPosition( (z_position_t) romReadInt(address++) );
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
  x_axis.quickSetPosition(setX);
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

void Plotter::arcAbsolute(long x3, long y3, long i, long j, int direction){
  char cBuffer[200];
  long x0, y0; // origin
  long x1, y1; // start
  long x2, y2; // position
  long rx, ry; // distance of position relative to the origin
  long dx, dy; // distance between the position and end point
  double phi; // angle of position
  double radius;
  int sticky = 0;

  x1 = x2 = (long) x_axis.getPosition(); // in virtual coordinates. this should be the target of the previous move, not the actual position
  y1 = y2 = (long) y_axis.getPosition();

  x0 = x1 + i;
  y0 = y1 + j;
  
  dx = x3 - x2;
  dy = y3 - y2;
  
  rx = x2 - x0;
  ry = y2 - y0;
  
  radius = sqrt(pow((x3 - x0), 2) + pow((y3 - y0), 2));
  
  if(DEBUG){
    sprintf(cBuffer, "\nstart(%ld, %ld), rPos(%ld, %ld), end(%ld, %ld), O(%ld, %ld), IJ(%ld, %ld), d(%ld, %ld), R=",  
              x1, y1, (long) (x_axis.getRealPosition() * XY_SCALE), y2, x3, y3, x0, y0, i, j, dx, dy);
    Serial.print(cBuffer);
    Serial.println(radius,3);
  }

  while(!(y2 == y3 && (SIGN(x2 - x0) == SIGN(x3 - x0) || x3 - x0 == 0 || x2 - x0 == 0))){
    
    rx = x2 - x0;
    ry = y2 - y0;

    phi = -999; // for debug
    
    if(direction == CCW && (rx > 0 || (rx == 0 && ry < 0))){
      //Serial.print("[CCW RH] ");
      sticky |= 1;
      y2++;
      y_axis.stepUp();
      dy = y2 - y0;
      if((double) dy >= radius || (double) dy <= -radius)
        x2 = x0;
      else{
        phi = asin( (double) dy / radius );
        x2 = x0 + (long) (radius * cos(phi));
//        if(DEBUG){
//          Serial.print(phi);
//          sprintf(cBuffer, " = asin(");
//          Serial.print(cBuffer);
//          Serial.print( (double) dy / radius );
//          Serial.println(")");
//        }
      }  
    } 
    else if(direction == CCW && (rx < 0 || (rx == 0 && ry > 0))){
      //Serial.print("[CCW LH] ");
      sticky |= 2;
      y2--;
      y_axis.stepDown();
      dy = y2 - y0;
      if((double) dy >= radius || (double) dy <= -radius)
        x2 = x0;
      else{
        phi = asin( (double) dy / radius );
        x2 = x0 - (long) (radius * cos(phi));
      }
    }
    else if(direction == CW && (rx > 0 || (rx == 0 && ry > 0))){
      //Serial.print("[CW RH] ");
      sticky |= 4;
      y2--;
      y_axis.stepDown();
      dy = y2 - y0;
      if((double) dy >= radius || (double) dy <= -radius)
        x2 = x0;
      else{
        phi = asin( (double) dy / radius );
        x2 = x0 + (long) (radius * cos(phi));
      }
    }
    else if(direction == CW && (rx < 0 || (rx == 0 && ry < 0))){
      //Serial.print("[CW LH] ");
      sticky |= 8;
      y2++;
      y_axis.stepUp();
      dy = y2 - y0;
      if((double) dy >= radius || (double) dy <= -radius)
        x2 = x0;
      else{
        phi = asin( (double) dy / radius );
        x2 = x0 - (long) (radius * cos(phi));
      }
    }

    x_axis.setPosition(x2);

    if(DEBUG){
      dx = x3 - x2;
      dy = y3 - y2;
      sprintf(cBuffer, "d(%ld, %ld), r(%ld, %ld) pos(%ld, %ld), phi=", dx, dy, rx, ry, x2, y2);
      Serial.print(cBuffer);
      Serial.println(phi, 3);
    }
  }
  if(sticky == 0xF || sticky == 0xE || sticky == 0xD || sticky == 0xB || sticky == 0x7){
    panic("arc error?");
  }
}


void Plotter::arcAbsolute(double setX, double setY, double i, double j, int direction){
  // char cBuffer[100];
  // sprintf(cBuffer, "\nARC: end(%ld, %ld) IJ(%ld, %ld)", (long) setX, (long) setY), (long) i, (long) j;
  // Serial.print(cBuffer);

  // if(DEBUG){
  //   Serial.print("ARC: end(");
  //   Serial.print(setX,2);
  //   Serial.print(",");
  //   Serial.print(setY,2);
  //   Serial.print(") IJ(");
  //   Serial.print(i,2);
  //   Serial.print(",");
  //   Serial.print(j,2);
  //   Serial.print(") direction=");
  //   Serial.println(direction);
  // }

  arcAbsolute((long) (setX / Y_STEPSIZE), (long) (setY / Y_STEPSIZE), (long) (i / Y_STEPSIZE), (long) (j / Y_STEPSIZE), direction);
  
}


void Plotter::moveHead(z_position_t pos){
  z_axis.setPosition(pos);
}

