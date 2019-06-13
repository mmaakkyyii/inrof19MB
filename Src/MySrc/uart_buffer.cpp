#include <uart_buffer.h>
#include "usart.h"


UartBuffer::UartBuffer(int _data_size, int _uart_num, char(*_Checksum)(uint8_t*,uint8_t), int(*_CheckStartbit)(uint8_t*, int)):data_size(_data_size),uart_num(_uart_num),Checksum(_Checksum),CheckStartbit(_CheckStartbit){
}

int16_t UartBuffer::Getint16Value(int offset){
	return (int16_t)( ((uint16_t)buffer[start_bit+offset+1]<<8)|(uint16_t)buffer[start_bit+offset] );
}

int8_t UartBuffer::Getint8Value(int offset){
	return (int8_t)buffer[start_bit+offset];
}


static int JetsonCheckStartbit(uint8_t* buf,int buffer_position){
	if(buffer_position>250){
		return -1;//i+1で配列外アクセスを防ぐ
	}

	for(int i=buffer_position;i>=0;i--){
		if((0xff==buf[i]) && (0xff==buf[i+1])){
			if(i+14<255)return i;
			else return -1;
		}
	}
	return -1;

}

static char JetsonChecksum(uint8_t* buf,uint8_t addr){
	uint8_t sum=0;
	for(int i=2;i<13;i++){
		sum=sum^buf[addr+i];
	}
	if(sum==buf[addr+13]){
		return 1;
	}else{
		return 0;
	}
}


static int R1370CheckStartbit(uint8_t* buf,int buffer_position){
	for(int i=buffer_position;i>=0;i--){
		if(buf[i]==0xAA){
			return i;
		}
	}
	return -1;

}
static char R1370Checksum(uint8_t* buf,uint8_t addr){
	char sum=0;
	for(int i=2;i<14;i++){
		sum=sum+buf[i+addr];
	}
	return (unsigned char)(sum&0xff)==(unsigned char)(buf[addr+14]);
}

UartBufferStlink::UartBufferStlink():uart_buffer(15,1,R1370Checksum,R1370CheckStartbit){

}

void UartBufferStlink::Init(){
	HAL_UART_Receive_DMA(&huart1, uart_buffer.GetBufferPointer(), uart_buffer.GetBufferSize());
}

int16_t UartBufferStlink::GetTheta(){
	return -uart_buffer.Getint16Value(3);
	//return -(int16_t)(((uint16_t)(uart_buffer.Getint8Value(3)<<8))|((uint16_t)(uart_buffer.Getint8Value(4))));
}

float UartBufferStlink::GetRadian(){
	static int pre_deg=0;
	static int rotation_n=0;
	int deg = -uart_buffer.Getint16Value(3);
	int diff = deg - pre_deg;
	if(diff<-34000) rotation_n++;
	if(diff>34000) rotation_n--;
	int _theta=rotation_n*36000+deg;
	pre_deg = deg;
	return _theta*3.141592f/(180.0f*100.0f);
}



UartBufferUSB::UartBufferUSB():uart_buffer(14,3,JetsonChecksum,JetsonCheckStartbit){
}

void UartBufferUSB::Init(){
	HAL_UART_Receive_DMA(&huart3, uart_buffer.GetBufferPointer(), uart_buffer.GetBufferSize());

}

int UartBuffer::Update(){
	int buffer_position=0;
	if(uart_num==1){
		buffer_position=buffer_size-huart1.hdmarx->Instance->CNDTR;
	}else if(uart_num==3){
		buffer_position=buffer_size-huart3.hdmarx->Instance->CNDTR;
	}else{
		return -3;
	}
	start_bit=CheckStartbit(buffer,buffer_position);
	if(start_bit<0){
		return -2;
	}

	if(Checksum(buffer,start_bit)){
		return 1;
	}

	return -1;

}


