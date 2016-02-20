#ifndef plotter_h
#define plotter_h

#include "ultraPlot2000.h"
#include "axis.h"

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
		// mm coordinates functions
		void initPosition(double, double);
		void moveAbsolute(double, double);
		void quickAbsolute(double, double);   
		void arcAbsoluteCW(double, double, double, double);
                void arcAbsoluteCCW(double, double, double, double);
};

#endif
