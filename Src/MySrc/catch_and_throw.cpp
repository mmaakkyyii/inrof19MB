#include "gpio.h"

#include "catch_and_throw.h"
#include "my_variable.h"


CatchAndThrow::CatchAndThrow(){
}

void CatchAndThrow::Ready(){
	if(mode==CatchAndThrowMode::catch_ready){
		mode=CatchAndThrowMode::ready;
	}
}

void CatchAndThrow::CatchReady(){
	if(mode==CatchAndThrowMode::ready){
		mode=CatchAndThrowMode::catch_ready;
	}
}

void CatchAndThrow::CatchBall(){
	if(mode==CatchAndThrowMode::catch_ready){
		mode=CatchAndThrowMode::catch_ball_mode;
	}
}
void CatchAndThrow::SetBall(){
	mode=CatchAndThrowMode::set_ball_mode;
}

void CatchAndThrow::PushSolenoid(){
	HAL_GPIO_WritePin(SOLENOID2_GPIO_Port,SOLENOID2_Pin,GPIO_PIN_SET);
}
void CatchAndThrow::PullSolenoid(){
	HAL_GPIO_WritePin(SOLENOID2_GPIO_Port,SOLENOID2_Pin,GPIO_PIN_RESET);
}

void CatchAndThrow::ThrowBall(){
	if(solenoid_counter>=solenoid_counter_max && mode==CatchAndThrowMode::catch_ready){
		mode=CatchAndThrowMode::throw_mode;
		solenoid_counter=0;
	}
}

void CatchAndThrow::Update(){
	switch(mode){
	case CatchAndThrowMode::ready:
		PullSolenoid();
		servo2.SetAngle(140);
		servo3.SetAngle(70);
		break;
	case CatchAndThrowMode::catch_ready:
		PullSolenoid();
		servo2.SetAngle(90);
		servo3.SetAngle(70);
		break;
	case CatchAndThrowMode::catch_ball_mode:
		PullSolenoid();
		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
		servo2.SetAngle(140);
		servo3.SetAngle(70);
		catch_counter++;
		if(catch_counter>=catch_counter_max){
			mode=CatchAndThrowMode::set_ball_mode;
			catch_counter=0;
		}
		break;
	case CatchAndThrowMode::set_ball_mode:
		PullSolenoid();
		set_counter++;
		if(set_counter<set_counter_ok){
			servo2.SetAngle(140);
			servo3.SetAngle(5);
		}else{
			servo2.SetAngle(140);
			servo3.SetAngle(50);
		}
		if(set_counter>set_counter_max){
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
			mode=CatchAndThrowMode::catch_ready;
			set_counter=0;
		}
		break;
	case CatchAndThrowMode::throw_mode:
		PushSolenoid();
		solenoid_counter++;
		if(solenoid_counter>solenoid_counter_max){
			PullSolenoid();
			mode=CatchAndThrowMode::ready;
		}
		break;
	default:
		mode=CatchAndThrowMode::ready;
		break;

	}
}
