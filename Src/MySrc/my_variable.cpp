#include "my_variable.h"
#include "main.h"

Motor motor1(1,MOTOR2_GPIO_Port,MOTOR2_Pin);
Motor motor2(1,MOTOR3_GPIO_Port,MOTOR3_Pin);
Motor motor3(-1,MOTOR4_GPIO_Port,MOTOR4_Pin);
Motor motor4(-1,MOTOR5_GPIO_Port,MOTOR5_Pin);

Servo servo1(0,2,1);
Servo servo2(0,2,2);
Servo servo3(0,3,1);
Servo servo4(0,3,2);

CatchAndThrow catch_and_throw;

UartBufferStlink uart_buffer_stlink;
UartBufferUSB uart_buffer_usb;

ControllerZPID controller(6.0f, 0.05f, 0.0f, 0.01f, 10.0f);

