#ifndef _CATCH_AND_THROW_H_
#define _CATCH_AND_THROW_H_

enum class CatchAndThrowMode{
	ready,
	catch_ready,
	catch_ball_mode,
	set_ball_mode,
	throw_mode
};

class CatchAndThrow{
private:
	int catch_counter=0;
	const int catch_counter_max=100;
	int set_counter=0;
	const int set_counter_ok=100;
	const int set_counter_max=set_counter_ok+100;

	const int solenoid_counter_max=10;
	int solenoid_counter=solenoid_counter_max;

	CatchAndThrowMode mode=CatchAndThrowMode::catch_ready;

	void PushSolenoid();
	void PullSolenoid();

	void SetBall();


public:
	CatchAndThrow();
	void CatchReady();
	void CatchBall();
	void ThrowBall();

	void Update();

};


#endif //_CATCH_AND_THROW_H_
