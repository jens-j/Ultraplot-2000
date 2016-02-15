#ifndef stepper_h
#define stepper_h


#define N_PHASE		8

typedef struct{
	int s0;
	int s1;
	int s2;
	int s3;
} mState_t; 


const mState_t PHASE[N_PHASE] = {{1,0,0,0},
		                   		 {1,0,1,0},                    
		                   		 {0,0,1,0},
		                   		 {0,1,1,0},
		                   		 {0,1,0,0},
		                   		 {0,1,0,1},
		                   		 {0,0,0,1},
		                   		 {1,0,0,1}};


class Stepper{
	private:
		int m0, m1, m2, m3; // Stepper pins
                int pwmDutyCycle;
		int state;
		void setState(int);  
	public:
		Stepper(int, int, int, int, int);
		void stepLeft();
		void stepRight();
};

#endif
