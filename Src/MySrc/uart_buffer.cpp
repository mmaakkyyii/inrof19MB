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
	if(buffer_position<2){
		return -2;//i+1で配列外アクセスを防ぐ
	}

	for(int i=255-buffer_position;i>=0;i--){
		if(buf[i]==0xff && buf[i+1]==0xff){
			if(i<255-14)return i;
			else return 0;
		}
	}
	return -1;

}

static char JetsonChecksum(uint8_t* buf,uint8_t addr){
	uint8_t sum=0;
	for(int i=2;i<13;i++){
		sum=sum^buf[i+addr];
	}
	return (sum==buf[addr+13]);
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

UartBufferUSB::UartBufferUSB():uart_buffer(14,3,JetsonChecksum,JetsonCheckStartbit){
}

void UartBufferUSB::Init(){
	HAL_UART_Receive_DMA(&huart3, uart_buffer.GetBufferPointer(), uart_buffer.GetBufferSize());

}

bool UartBuffer::Update(){
	int buffer_position=0;
	if(uart_num==1){
		buffer_position=huart1.hdmarx->Instance->CNDTR;
	}else if(uart_num==3){
		buffer_position=huart3.hdmarx->Instance->CNDTR;
	}else{
		return 0;
	}
	start_bit=CheckStartbit(buffer,buffer_position);
	if(start_bit<0){
		return 0;
	}

	if(Checksum(buffer,start_bit)){
		return 1;
	}

	return 0;

}


