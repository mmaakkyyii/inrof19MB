#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "gpio.h"

class Motor{
private:
	int dir;
	GPIO_TypeDef* port;
	uint16_t pin;
public:
	Motor(int _dir,GPIO_TypeDef* _port, uint16_t _pin );

	void Drive(float v);
};

#endif //_MOTOR_H_
