#ifndef stepper_h
#define stepper_h


#define N_PHASE		8

typedef struct{
	int s0;
	int s1;
	int s2;
	int s3;
} mState_t; 

// positions of the plotter head 
enum position_t {UP = 0, DOWN = 1, MOVING = 2};

const mState_t PHASE[N_PHASE] = {{1,0,0,0},
		                   		 {1,0,1,0},                    
		                   		 {0,0,1,0},
		                   		 {0,1,1,0},
		                   		 {0,1,0,0},
		                   		 {0,1,0,1},
		                   		 {0,0,0,1},
		                   		 {1,0,0,1}};


class Stepper{
	protected:
		int m0, m1, m2, m3; // Stepper pins
		int state;
		void setState(int);  
	public:
		Stepper(int, int, int, int);
		void stepLeft();
		void stepRight();
};

class StepperZ : public Stepper{
	private:
		position_t position;
	public:
		StepperZ(int, int, int, int);
		position_t getPosition();
		void setPosition(position_t pos);
};


#endif
