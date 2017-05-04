#pragma once
#include "Component.h"
class CKartControllerComponent : public CComponent
{
public:
	CKartControllerComponent();
	~CKartControllerComponent();

	void MoveRight();
	void MoveLeft();
	void MoveFoward();
	void MoveBackWards();

private:

	float myFowrardSpeed;
	float myMaxSpeed;
	float myMinSpeed;

	float myAcceleration;
	float myMaxAcceleration;
	float myMinAcceleration;

	float myFriction;

	float mySteering;
	float myAngularAcceleration;
};

