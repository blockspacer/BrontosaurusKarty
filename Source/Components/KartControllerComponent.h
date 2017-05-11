#pragma once
#include "Component.h"

namespace Physics {
	class CPhysicsScene;
}

class CParticleEmitterInstance;

class CKartControllerComponent : public CComponent
{
public:
	class CKartAxis
	{
	public:
		CKartAxis();
		CKartAxis(float aLength, float aWidth);
		void DoPhysics();
	private:
	};

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

	void CheckZKill();
	void Update(const float aDeltaTime);
	void Receive(const eComponentMessageType, const SComponentMessageData&) override;
	void Init(Physics::CPhysicsScene* aPhysicsScene);

	enum class eCurrentAction
	{
		eTurningRight,
		eTurningLeft,
		eDefault,
	};

private:
	void DoPhysics(const float aDeltaTime);
	float myFallSpeed;

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
	float myBoostSpeedDecay;
	float myDriftSteeringModifier;
	float myMaxDriftRate;
	float myTimeMultiplier;
	float myMaxDriftSteerAffection;

	eCurrentAction myCurrentAction;

	int myLeftWheelDriftEmmiterHandle;
	int myRightWheelDriftEmmiterHandle;
	int myLeftDriftBoostEmitterhandle;
	int myRightDriftBoostEmitterhandle;

	bool myIsDrifting;
	Physics::CPhysicsScene* myPhysicsScene;
};

