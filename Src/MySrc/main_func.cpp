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

void USBDebug(char* data,int size){
	HAL_UART_Transmit_DMA(&huart3,(uint8_t *)data,(uint16_t)size);
}



uint8_t spi_buff[2];
int spi_data_size=2;

uint8_t RxData[14];
uint8_t TxData[10]={0,1,2,3,4,5,6,7,8,9};


void Init(){
	HAL_TIM_Base_Start_IT(&htim1);

	char data[20]="Hello rietion\r\n";
	USBDebug(data,20);

	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);


	HAL_Delay(100);


	HAL_UART_Receive_DMA(&huart3, RxData,14);
//	HAL_UART_Transmit_DMA(&huart3, TxData,10);

}

void Loop(){
//	HAL_SPI_Receive(&hspi1,spi_buff,spi_data_size,10);
}


float vel=0;

void TimerInterrupt(){//10ms‚¨‚«‚ÉŒÄ‚Î‚ê‚é
	static int d=1;
	int max=100;
	vel+=d;
	if(vel>max){
		d=-1;
	}
	if(vel<-max){
		d=1;
	}
	if(HAL_GPIO_ReadPin(SW_GPIO_Port,SW_Pin)){
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,HAL_GPIO_ReadPin(ROTARY_SW0_GPIO_Port,ROTARY_SW0_Pin));
		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,HAL_GPIO_ReadPin(ROTARY_SW1_GPIO_Port,ROTARY_SW1_Pin));
		HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,HAL_GPIO_ReadPin(ROTARY_SW2_GPIO_Port,ROTARY_SW2_Pin));
		HAL_GPIO_WritePin(SOLENOID1_GPIO_Port,SOLENOID1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(SOLENOID2_GPIO_Port,SOLENOID2_Pin,GPIO_PIN_RESET);
	}else{
		HAL_GPIO_WritePin(SOLENOID1_GPIO_Port,SOLENOID1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SOLENOID2_GPIO_Port,SOLENOID2_Pin,GPIO_PIN_SET);

	}

	char po[5]={};
	int num = sprintf(po,"%d\r\n",(int)vel);
	//Debug(po,num);

	//USBDebug(po,num);

	motor1.Drive(-vel);
	motor2.Drive(vel);
	motor3.Drive(vel);
	motor4.Drive(-vel);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_UART_Receive_DMA(&huart3,RxData, 14);

	int heard1=(int)RxData[0];
	int heard2=(int)RxData[1];
	int v_x    =(int)( (0xFF & (uint16_t)(RxData[2])) | (uint16_t)(RxData[3])<<8 );
	int v_y    =(int)( (0xFF & (uint16_t)(RxData[4])) | (uint16_t)(RxData[5])<<8 );
	int v_theta=(int)( (0xFF & (uint16_t)(RxData[6])) | (uint16_t)(RxData[7])<<8 );

	int angle1 =(int)( (0xFF & (uint16_t)(RxData[8])) | (uint16_t)(RxData[9])<<8 );
	int angle2 =(int)( (0xFF & (uint16_t)(RxData[10])) | (uint16_t)(RxData[11])<<8 );


	char po[30]={};
//	int num = sprintf(po,"%d,%d,%d,%d,%d\r\n",heard1,heard2,v_x,v_y,v_theta,angle1,angle2);
	int num =sprintf(po,"%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c\r\n",
			(char)RxData[0],
			(char)RxData[1],
			(char)RxData[2],
			(char)RxData[3],
			(char)RxData[4],
			(char)RxData[5],
			(char)RxData[6],
			(char)RxData[7],
			(char)RxData[8],
			(char)RxData[9],
			(char)RxData[10],
			(char)RxData[11],
			(char)RxData[12],
			(char)RxData[13]);
	Debug(po,num);

}
