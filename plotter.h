#ifndef plotter_h
#define plotter_h

#include "ultraPlot2000.h"
#include "axis.h"

enum {CW, CCW};

enum {POSITIVE, ZERO, NEGATIVE};
#define SIGN(X) ((X > 0) ? POSITIVE : ((X == 0) ? ZERO : NEGATIVE)

typedef struct position_s{
  double x;
  double y;
}position_t;

class Plotter{
	private:
		position_t position;
	public:
		X_axis x_axis;
		Y_axis y_axis;
		Z_axis z_axis;
		
		Plotter();
		void moveHeadUp();
		void moveHeadDown();
		// pixel coordinate functions
		void moveAbsolute(int, int);
		void quickAbsolute(int, int);
		void moveRelative(int, int);
		void arcAbsoluteCW(int, int, double, double);
		void arcAbsoluteCCW(int, int, double, double);
		void arcAbsolute(int, int, int, int, int);
		// mm coordinates functions
		void initPosition(double, double);
		void moveAbsolute(double, double);
		void quickAbsolute(double, double);   
		void arcAbsolute(double, double, double, double, int);
};

#endif
