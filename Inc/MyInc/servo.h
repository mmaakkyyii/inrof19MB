#ifndef _SERVO_H_
#define _SERVO_H_

class Servo{
private:

	int zero;
	int tim;
	int ch;


public:
	Servo(int _zero, int _tim, int _ch);
	void PWM(int duty);
	void SetAngle(int angle);

};

#endif //_SERVO_H_
