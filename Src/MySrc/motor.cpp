#include "motor.h"
#include "spi.h"
#include "gpio.h"

Motor::Motor(int _dir,GPIO_TypeDef* _port, uint16_t _pin ):dir(_dir),port(_port),pin(_pin){
	if(dir!=1 && dir!=-1){
		while(1){}
	}
}

void Motor::Drive(float v){
	v*=dir;
	uint8_t rietion[3];
	rietion[0]=((int16_t)v) >> 8;
	rietion[1]=((int16_t)v) & 0xff;
	rietion[2]=rietion[0]^rietion[1];

	HAL_GPIO_WritePin(port,pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,(uint8_t*)rietion,3,200);
	HAL_GPIO_WritePin(port,pin,GPIO_PIN_SET);


}
