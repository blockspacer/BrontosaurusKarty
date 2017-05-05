#pragma once
#include "Component.h"

class CParticleEmitterInstance;

class CKartControllerComponent : public CComponent
{
public:
	CKartControllerComponent();
	~CKartControllerComponent();

	void TurnRight();
	void TurnLeft();
	void StopMoving();
	void MoveFoward();
	void MoveBackWards();
	void StopTurning();
	void Drift();
	void StopDrifting();

	void Update(const float aDeltaTime);
	void Receive(const eComponentMessageType, const SComponentMessageData&) override;

private:
	float myFowrardSpeed;
	float myMaxSpeed;
	float myMinSpeed;

	float myAcceleration;
	float myMaxAcceleration;
	float myMinAcceleration;

	float myTurnRate;

	float myFriction;

	float mySteering;
	float myAngularAcceleration;

	float myMaxSpeedModifier;
	float myAccelerationModifier;

	float myDriftRate;
	float myDriftTimer;
	float myDriftSteerModifier;

	int myDriftParticleEmitter;

	bool myIsDrifting;
};

