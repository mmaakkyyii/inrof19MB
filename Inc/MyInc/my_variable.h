#ifndef _MY_VARIABLE_H_
#define _MY_VARIABLE_H_

#include <uart_buffer.h>
#include "motor.h"
#include "servo.h"
#include "catch_and_throw.h"


extern Motor motor1;
extern Motor motor2;
extern Motor motor3;
extern Motor motor4;

extern Servo servo1;
extern Servo servo2;
extern Servo servo3;
extern Servo servo4;

extern CatchAndThrow catch_and_throw;

extern UartBufferStlink uart_buffer_stlink;
extern UartBufferUSB uart_buffer_usb;

#endif //_MY_VARIABLE_H_
