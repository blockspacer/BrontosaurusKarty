#pragma once
#include "Component.h"

namespace Physics {
	class CPhysicsScene;
}

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
	void ClearSpeed();
	void ClearHeight();
	void SetHeight(int aWheelIndex, float aHeight, const float aDt);
	float GetHeightSpeed(int anIndex);
	void DoPhysics(const float aDeltaTime);

	enum class AxisPos
	{
		RightBack,
		RightFront,
		LeftBack,
		LeftFront,
		Size
	};
	
	float myAxisSpeed[static_cast<int>(AxisPos::Size)];

	struct
	{
		float width = 1.f;
		float length = 1.f;
	} myAxisDescription;


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

	float myPreviousHeight[static_cast<int>(AxisPos::Size)];
	float myCurrentHeight[static_cast<int>(AxisPos::Size)];
	bool myFirstMovingPass;
};

