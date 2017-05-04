#pragma once
#include "Component.h"
class CKartComponent : public CComponent
{
public:
	CKartComponent();
	~CKartComponent();

	void Receive(const eComponentMessageType, const SComponentMessageData&) override;
	void Update(float aDeltaTime);

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

