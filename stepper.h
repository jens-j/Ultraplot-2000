#ifndef stepper_h
#define stepper_h


const mState_t PHASE[8] = {{1,0,0,0},
		                   {1,0,1,0},                    
		                   {0,0,1,0},
		                   {0,1,1,0},
		                   {0,1,0,0},
		                   {0,1,0,1},
		                   {0,0,0,1},
		                   {1,0,0,1}};

// positions of the plotter head 
enum position_t {UP = 0, DOWN = 1, MOVING = 2};

typedef mState_s struct{
	int s0;
	int s1;
	int s2;
	int s3;
} mState_t; 

class Stepper{
	private:
		int m0, m1, m2, m3; // Stepper pins
		int state;
		setState(mState_t state);
	public:
		mState_t getState();
		stepLeft();
		stepRight();
};

class StepperZ : public Stepper{
	private:
		position_t position;
	public:
		position_t getPosition();
		setPosition(position_t pos);
};


#endif