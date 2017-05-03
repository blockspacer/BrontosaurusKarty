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
	float myAcceleration;
	float mySpeed;
	float myMaxSpeed;
	float myDecceleration;
};

