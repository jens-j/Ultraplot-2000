#ifndef sensor_h
#define sensor_h


// sensor value datatype
typedef struct sVal_s{
  int s0;
  int s1;
} sVal_t;


class Sensor{
	private:
		int s0, s1; // sensor pins
	public:
		static int decode(sVal_t);
		sVal_t readSensor();
		int decodeSensor();
};


#endif