#include "main_func.h"
#include "my_variable.h"

#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "spi.h"
#include "main.h"


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

const int rx_data_size=14;
const int rx_buffer_size=255;
uint8_t RxData[rx_buffer_size];
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

	HAL_UART_Receive_DMA(&huart3, RxData,rx_buffer_size);

}

void Loop(){
}

float vel=0;

int UpdateUartBuffer(int *data){
	for(int i=0;i<6;i++)data[i]=0;
	int start_bit_addr1=-1;
	int start_bit_addr2=-1;
	int check_sum_addr=-1;
	int buffer_position=huart3.hdmarx->Instance->CNDTR-1;

	for(int i=buffer_position;i>=0;i--){
		if(RxData[i]==0xFF && RxData[i+1]==0xFF){
			start_bit_addr1=i;
			start_bit_addr2=i+1;
			if(i+rx_data_size-1<rx_buffer_size-1)check_sum_addr=i+rx_data_size-1;
			else return 0;
			break;
		}
	}

	if(start_bit_addr1==-1){
		return -1;//�X�^�[�g�r�b�g��������Ȃ������Ƃ���false��Ԃ�
	}


	uint8_t sum=0x00;
	for(int i=0;i<rx_buffer_size;i++){
		if(start_bit_addr1<check_sum_addr){
			if(start_bit_addr2<i && i<check_sum_addr){
				sum=sum xor RxData[i];
			}
		}else{
			if(i<check_sum_addr && i<start_bit_addr2){
				sum=sum xor RxData[i];
			}
		}
	}

	if(RxData[check_sum_addr]==sum){
		int count=0;
		int i=start_bit_addr2;
		if(i==rx_buffer_size-2){
			i=0;
		}else if(i==rx_buffer_size-1){
			i=1;
		}else{
			i+=1;
		}

		while(1){
			if(i==rx_buffer_size-1){
				data[count]=(int16_t)( ( ((uint16_t)RxData[i])&0xFF ) |( (((uint16_t)RxData[0])&0xFF)<<8 ) );
				i=1;
			}else if(i==rx_buffer_size){
				data[count]=(int16_t)( ( ((uint16_t)RxData[0])&0xFF ) |( (((uint16_t)RxData[1])& 0xFF)<<8 ) );
				i=2;
			}else{
				data[count]=(int16_t)( ( ((uint16_t)RxData[i])&0xFF ) |( (((uint16_t)RxData[i+1])& 0xFF)<<8 ) );
				i+=2;
			}
			count++;
			if(count==5){
				data[count]=RxData[i];
				break;
			}
		}

		return 1;
	}
  	return 0;

	//////////////
	/*
	for(int i=0;i<rx_data_size-1;i++){
		if(RxData[i]==0xFF && RxData[i+1]==0xFF){
			start_bit_addr1=i;
			start_bit_addr2=i+1;
			if(start_bit_addr1==0){//�X�^�[�g�r�b�g���z��̐擪�ɂ���Ƃ��`�F�b�N�T���͔z��̈�ԍŌ�ɂ���͂͂�
				check_sum_addr=rx_data_size-1;
			}else{
				check_sum_addr=i-1;
			}
		}
	}
	if(RxData[0]==0xFF && RxData[rx_data_size]==0xFF){//�X�^�[�g�r�b�g���z��̏��߂ƍŌ�ɕ�����Ă���
		start_bit_addr1=rx_data_size;
		start_bit_addr2=0;
		check_sum_addr=rx_data_size-1;
	}
	if(start_bit_addr1==-1)return 0;//�X�^�[�g�r�b�g��������Ȃ������Ƃ���false��Ԃ�


	uint8_t sum=0x00;
	for(int i=0;i<rx_data_size;i++){
		if(i!=start_bit_addr1 && i!=start_bit_addr2 && i!=check_sum_addr){
			sum=sum xor RxData[i];
		}
	}

	if(RxData[check_sum_addr]==sum){
		int count=0;
		int i=start_bit_addr2;
		if(i==rx_data_size-2){
			i=0;
		}else if(i==rx_data_size-1){
			i=1;
		}else{
			i+=1;
		}

		while(1){
			if(i==rx_data_size-1){
				data[count]=(int16_t)( ( ((uint16_t)RxData[i])&0xFF ) |( (((uint16_t)RxData[0])&0xFF)<<8 ) );
				i=1;
			}else if(i==rx_data_size){
				data[count]=(int16_t)( ( ((uint16_t)RxData[0])&0xFF ) |( (((uint16_t)RxData[1])& 0xFF)<<8 ) );
				i=2;
			}else{
				data[count]=(int16_t)( ( ((uint16_t)RxData[i])&0xFF ) |( (((uint16_t)RxData[i+1])& 0xFF)<<8 ) );
				i+=2;
			}
			count++;
			if(count==5){
				data[count]=RxData[i];
				break;
			}
		}

		return 1;
	}
	return -1;
	*/
}

void TimerInterrupt(){//10ms�����ɌĂ΂��
	static int solenoid_flag=0;
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

	if(HAL_GPIO_ReadPin(SW_GPIO_Port,SW_Pin)){
		solenoid_flag++;
		if(solenoid_flag<10){
			HAL_GPIO_WritePin(SOLENOID2_GPIO_Port,SOLENOID2_Pin,GPIO_PIN_SET);
		}else{
			HAL_GPIO_WritePin(SOLENOID2_GPIO_Port,SOLENOID2_Pin,GPIO_PIN_RESET);
		}
	}else{
		solenoid_flag=0;
		HAL_GPIO_WritePin(SOLENOID2_GPIO_Port,SOLENOID2_Pin,GPIO_PIN_RESET);
	}
	if(d==1){
		servo1.SetAngle(0);
		servo2.SetAngle(10);
		servo3.SetAngle(140);
		servo4.SetAngle(180);
	}else{
		servo1.SetAngle(0);
		servo2.SetAngle(50);
		servo3.SetAngle(50);
		servo4.SetAngle(180);
	}

	//char po[5]={};
	//int num = sprintf(po,"%d\r\n",(int)vel);
	//Debug(po,num);

	//USBDebug(po,num);
	char poi[30]={};

//	if(huart3.hdmarx->Instance->CNDTR==(rx_data_size))	huart3.hdmarx->Instance->CNDTR=16;


/*	int n =sprintf(poi,"%d:%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c%c\r\n",
			huart3.hdmarx->Instance->CNDTR,
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
			(char)RxData[12]);
*/



	int receive_data[6];
	int uart_check=UpdateUartBuffer(receive_data);
	if(uart_check==1){
		float v_ref[3]={(float)receive_data[0],(float)receive_data[1],(float)receive_data[2]/1000.0f};
		float v_wheel[4]={0,0,0,0};
		float r=120.0f;
		v_wheel[0]=v_ref[0]+v_ref[1]-r*v_ref[2];
		v_wheel[1]=v_ref[0]-v_ref[1]-r*v_ref[2];
		v_wheel[2]=v_ref[0]+v_ref[1]+r*v_ref[2];
		v_wheel[3]=v_ref[0]-v_ref[1]+r*v_ref[2];

		motor1.Drive(v_wheel[0]);
		motor2.Drive(v_wheel[1]);
		motor3.Drive(v_wheel[2]);
		motor4.Drive(v_wheel[3]);

		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
	}


	if(HAL_GPIO_ReadPin(SW_GPIO_Port,SW_Pin)){
		int n =sprintf(poi,"%d:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
				huart3.hdmarx->Instance->CNDTR,
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

		Debug(poi,n);

	}else{
		int n=sprintf(poi,"%d,%2d,%d,%d,%d\r\n",uart_check,huart3.hdmarx->Instance->CNDTR,receive_data[0],receive_data[1],receive_data[2]);
		Debug(poi,n);

	}

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_UART_Receive_DMA(&huart3,RxData, rx_buffer_size);

//	int heard1=(int)RxData[0];
//	int heard2=(int)RxData[1];
//	int v_x    =(int)( (0xFF & (uint16_t)(RxData[2])) | (uint16_t)(RxData[3])<<8 );
//	int v_y    =(int)( (0xFF & (uint16_t)(RxData[4])) | (uint16_t)(RxData[5])<<8 );
//	int v_theta=(int)( (0xFF & (uint16_t)(RxData[6])) | (uint16_t)(RxData[7])<<8 );
//
//	int angle1 =(int)( (0xFF & (uint16_t)(RxData[8])) | (uint16_t)(RxData[9])<<8 );
//	int angle2 =(int)( (0xFF & (uint16_t)(RxData[10])) | (uint16_t)(RxData[11])<<8 );


//	char po[30]={};
//	int num = sprintf(po,"%d,%d,%d,%d,%d\r\n",heard1,heard2,v_x,v_y,v_theta,angle1,angle2);
//	int num =sprintf(po,"%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c\r\n",
//			(char)RxData[0],
//			(char)RxData[1],
//			(char)RxData[2],
//			(char)RxData[3],
//			(char)RxData[4],
//			(char)RxData[5],
//			(char)RxData[6],
//			(char)RxData[7],
//			(char)RxData[8],
//			(char)RxData[9],
//			(char)RxData[10],
//			(char)RxData[11],
//			(char)RxData[12],
//			(char)RxData[13]);
//	Debug(po,num);

//	for(int i=0;i<16;i++)RxData[i]=' ';
}
