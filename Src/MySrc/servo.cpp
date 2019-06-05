#include "servo.h"
#include "tim.h"

Servo::Servo(int _zero, int _tim, int _ch):zero(_zero){
	if(_tim==2 || _tim==3)tim=_tim;
	else while(1);
	if(_ch==1 || _ch==2)ch=_ch;

}

void Servo::PWM(int duty){
	if(duty<0)duty=0;
	else if(duty>19999)duty=19999;
	TIM_OC_InitTypeDef sConfigOC;

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = duty;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	if(tim==2){
		if(ch==1){
			HAL_TIM_PWM_ConfigChannel(&htim2,&sConfigOC,TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
		}else if(ch==2){
			HAL_TIM_PWM_ConfigChannel(&htim2,&sConfigOC,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
		}
	}else if(tim==3){
		if(ch==1){
			HAL_TIM_PWM_ConfigChannel(&htim3,&sConfigOC,TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		}else if(ch==2){
			HAL_TIM_PWM_ConfigChannel(&htim3,&sConfigOC,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
		}
	}

}

void Servo::SetAngle(int angle){//angle 0~180;
	const int max_duty=3000;
	const int min_duty=500;

	PWM((int)((max_duty-min_duty)*(angle/180.0f)+min_duty));

}

