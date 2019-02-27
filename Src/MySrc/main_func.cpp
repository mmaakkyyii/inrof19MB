#include "main_func.h"
#include "my_variable.h"

#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "spi.h"
#include "main.h"


void Debug(char* data,int size){
	HAL_UART_Transmit_IT(&huart1,(uint8_t *)data,(uint16_t)size);

}


uint8_t spi_buff[2];
int spi_data_size=2;

void Init(){

	HAL_TIM_Base_Start_IT(&htim1);

	char data[20]="Hello rietion\r\n";
	Debug(data,20);

	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);


	HAL_Delay(100);
}

void Loop(){
//	HAL_SPI_Receive(&hspi1,spi_buff,spi_data_size,10);
}


float vel=0;

void TimerInterrupt(){//10ms‚¨‚«‚ÉŒÄ‚Î‚ê‚é
	vel++;
	if(vel>1000)vel=-1000;
	if((int)vel%100==0){
		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
		HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);
	}

	char po[5]={};
	int num = sprintf(po,"%d\r\n",(int)vel);
	Debug(po,num);

	motor1.Drive(vel);
	motor2.Drive(-vel);

}
