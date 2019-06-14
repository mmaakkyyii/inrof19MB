#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

class ControllerBase{
protected:
	float reference;
public:
	virtual float Update(float x)=0;
	virtual void Reset()=0;
	virtual void SetReference(float _reference);
	virtual float GetReference()=0;
};

class ControllerPID:public ControllerBase{
protected:
	float e;
	float sum_e;
	float d_e;
	float kp;
	float ki;
	float kd;

public:
	ControllerPID();
	ControllerPID(float _kp,float _ki,float _kd);
	virtual float Update(float x) override;
	virtual void Reset() override;
	void SetGain(float _kp,float _ki,float _kd);
	float GetReference();
};


class ControllerZPID : public ControllerPID
{
protected:
	float x_kim1,y_kim1;
	float x_kdm1,y_kdm1;
	float ControllerP(float x_k);
	float ControllerI(float x_k);
	float ControllerD(float x_k);
	float ts;       //制御周期[s]
	float n;        //微分器用フィルター係数[rad/s]
public:
	ControllerZPID(float _ts,float _fc);
	ControllerZPID(float _kp,float _ki,float _kd,float _ts,float _fc);
	virtual void Reset() override;
	virtual float Update(float x) override;
};

#endif //_CONTROLLER_H_
