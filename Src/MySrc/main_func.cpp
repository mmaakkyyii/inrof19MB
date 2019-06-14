#include "main_func.h"
#include "my_variable.h"

#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "spi.h"
#include "main.h"
#include "catch_and_throw.h"
#include "math.h"

int GetRotarySW(){
	return   (int)!HAL_GPIO_ReadPin(ROTARY_SW0_GPIO_Port,ROTARY_SW0_Pin)
			|(int)!HAL_GPIO_ReadPin(ROTARY_SW1_GPIO_Port,ROTARY_SW1_Pin)<<1
			|(int)!HAL_GPIO_ReadPin(ROTARY_SW2_GPIO_Port,ROTARY_SW2_Pin)<<2
			|(int)!HAL_GPIO_ReadPin(ROTARY_SW3_GPIO_Port,ROTARY_SW3_Pin)<<3;
}

void BuzzerOn(int frq){
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 71;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = frq-1;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 500;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;


	HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4);

	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);

}

void BuzzerOff(){
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_4);

}

void Debug(char* data,int size){
	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)data,(uint16_t)size);
}

void USBDebug(char* data,int size){
	HAL_UART_Transmit_DMA(&huart3,(uint8_t *)data,(uint16_t)size);
}

bool UartUpdate(){
	return 0;
}


uint8_t spi_buff[2];
int spi_data_size=2;

uint8_t TxData[10]={0,1,2,3,4,5,6,7,8,9};

const int Do=261;
const int Re=293;
const int Mi=329;
const int Fa=349;
const int So=392;
const int Ra=440;
const int Si=493;
const int DoH=523;


void Init(){

	char data[20]="Hello rietion\r\n";
	USBDebug(data,20);


	BuzzerOff();
	HAL_Delay(1000);

	HAL_GPIO_WritePin(SOLENOID1_GPIO_Port,SOLENOID1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(SOLENOID2_GPIO_Port,SOLENOID2_Pin,GPIO_PIN_RESET);

	HAL_TIM_Base_Start_IT(&htim1);

	uart_buffer_stlink.Init();
	uart_buffer_usb.Init();

}

void Loop(){
}

float vel=0;

void TimerInterrupt(){//10ms‚¨‚«‚ÉŒÄ‚Î‚ê‚é
	static float theta=0;
	static float pre_theta=0;
	catch_and_throw.Update();

	static int d=1;
	int max=10;
	vel+=d;
	if(vel>max){
		d=-1;
	}
	if(vel<-max){
		d=1;
	}
	if(d<0){
		HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
	}else{
		HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);
	}

	char poi[40];


	int uart_check=uart_buffer_usb.Update();
	int gyro_check=uart_buffer_stlink.Update();
	if(gyro_check){
		theta=uart_buffer_stlink.GetRadian();
		if(abs(pre_theta-theta)>0.8)theta=pre_theta;
		pre_theta=theta;
	}
	if(uart_check==1){
		float v_ref[3]={(float)uart_buffer_usb.GetVx(),(float)uart_buffer_usb.GetVy(),(float)uart_buffer_usb.GetVw()/1000.0f};
		float v_wheel[4]={0,0,0,0};
		int n=sprintf(poi,"%4d,%4d\r\n",(int)(v_ref[2]*1000),(int)(theta*1000));
		Debug(poi,n);

		controller.SetReference(v_ref[2]);
		v_ref[2]=controller.Update(theta);

		float r=120.0f;
		v_wheel[0]=-v_ref[0]+v_ref[1]-r*v_ref[2];
		v_wheel[1]=-v_ref[0]-v_ref[1]-r*v_ref[2];
		v_wheel[2]=-v_ref[0]+v_ref[1]+r*v_ref[2];
		v_wheel[3]=-v_ref[0]-v_ref[1]+r*v_ref[2];

		motor1.Drive(v_wheel[0]);
		motor2.Drive(v_wheel[1]);
		motor3.Drive(v_wheel[2]);
		motor4.Drive(v_wheel[3]);

		if(uart_buffer_usb.GetCatchThrowFlag()==0x01){
			catch_and_throw.ThrowBall();
		}else if(uart_buffer_usb.GetCatchThrowFlag()==0x10){
			catch_and_throw.CatchBall();
		}else if(uart_buffer_usb.GetCatchThrowFlag()==0x02){
			catch_and_throw.CatchReady();
		}else if(uart_buffer_usb.GetCatchThrowFlag()==0x03){
		catch_and_throw.Ready();
	}




		if(uart_buffer_usb.GetVy()==255){
			HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
		}else{
			HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
		}



	}else{
	}

	//int n=sprintf(poi,"%d,%d,%d,%d,%d,%d\r\n",(int)uart_check,uart_buffer_usb.GetVx(),uart_buffer_usb.GetVy(),uart_buffer_usb.GetVw(),uart_buffer_usb.GetCatchThrowFlag(),uart_buffer_usb.GetCheckSum());
	//int n=sprintf(poi,"%d,%d,%d,%d,%d,%d\r\n",uart_check,uart_buffer_usb.GetVx(),uart_buffer_usb.GetVy(),uart_buffer_usb.GetVw(),uart_buffer_usb.GetCatchThrowFlag(),uart_buffer_usb.GetCheckSum());
/*	int n=sprintf(poi,"%2d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n"
			,uart_check
			,uart_buffer_usb.GetBuffer(0)
			,uart_buffer_usb.GetBuffer(1)
			,uart_buffer_usb.GetBuffer(2)
			,uart_buffer_usb.GetBuffer(3)
			,uart_buffer_usb.GetBuffer(4)
			,uart_buffer_usb.GetBuffer(5)
			,uart_buffer_usb.GetBuffer(6)
			,uart_buffer_usb.GetBuffer(7)
			,uart_buffer_usb.GetBuffer(8)
			,uart_buffer_usb.GetBuffer(9)
			,uart_buffer_usb.GetBuffer(10)
			,uart_buffer_usb.GetBuffer(11)
			,uart_buffer_usb.GetBuffer(12)
			,uart_buffer_usb.GetBuffer(13));
	Debug(poi,n);
*/

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart==&huart3){
		HAL_UART_Receive_DMA(&huart3,uart_buffer_usb.GetBufferPointer(), uart_buffer_usb.GetBufferSize());
	}else if(huart==&huart1){
		HAL_UART_Receive_DMA(&huart1,uart_buffer_stlink.GetBufferPointer(), uart_buffer_stlink.GetBufferSize());
	}
}
