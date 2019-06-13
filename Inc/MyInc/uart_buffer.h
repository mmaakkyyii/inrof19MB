#ifndef _UART_BUFFER_H_
#define _UART_BUFFER_H_

#include "stdint.h"

class UartBuffer{
private:
	int data_size;
	static constexpr uint8_t buffer_size=255;
	uint8_t buffer[255];
	int uart_num=0;
	int start_bit=1;

	char(*Checksum)(uint8_t*,uint8_t);
	int(*CheckStartbit)(uint8_t*, int);


public:
	UartBuffer(int _data_size, int _uart_num, char (*_Checksum)(uint8_t*,uint8_t), int(*_CheckStartbit)(uint8_t*, int));
	int Update();
	void Init();
	int GetUartNum(){return uart_num;}
	void SetUartNum(int num){uart_num=num;}
	void SetStartbit(uint8_t addr){start_bit=addr;}

	uint8_t *GetBufferPointer(){return buffer;}
	uint8_t GetBufferSize(){return buffer_size;}

	int16_t Getint16Value(int offest);
	int8_t Getint8Value(int offset);


};

class UartBufferStlink{
private:
	UartBuffer uart_buffer;

public:
	UartBufferStlink();
	void Init();

	int Update(){return uart_buffer.Update();}

	uint8_t *GetBufferPointer(){return uart_buffer.GetBufferPointer();}
	uint8_t GetBufferSize(){return uart_buffer.GetBufferSize();}

	int16_t GetTheta();
	float GetRadian();

};

class UartBufferUSB{
private:
	UartBuffer uart_buffer;

public:
	UartBufferUSB();
	void Init();

	int Update(){return uart_buffer.Update();}

	uint8_t *GetBufferPointer(){return uart_buffer.GetBufferPointer();}
	uint8_t GetBufferSize(){return uart_buffer.GetBufferSize();}

	uint8_t GetBuffer(int i){return (uint8_t)uart_buffer.Getint8Value(i);}
	int GetVx(){return uart_buffer.Getint16Value(2);}
	int GetVy(){return uart_buffer.Getint16Value(4);}
	int GetVw(){return uart_buffer.Getint16Value(6);}
	int GetCatchThrowFlag(){return uart_buffer.Getint8Value(12);}
	uint8_t GetCheckSum(){return (uint8_t)uart_buffer.Getint8Value(13);}
};

#endif //_UART_BUFFER_H_
