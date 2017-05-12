#pragma once
#include "Component.h"

namespace Physics
{
	class CPhysicsScene;
}

class CParticleEmitterInstance;
class CDrifter;

class CKartControllerComponent : public CComponent
{
public:
	CKartControllerComponent();
	~CKartControllerComponent();

	void Turn(float aDirectionX);
	void TurnRight(const float aNormalizedModifier = 1.f);
	void TurnLeft(const float aNormalizedModifier = -1.f);
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

	//std::function<bool(const CU::Vector3f&, const CU::Vector3f&, float)> myIsGrounded;
	
	float myAxisSpeed[static_cast<int>(AxisPos::Size)];

	std::unique_ptr<CDrifter> myDrifter;

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

	float myBoostSpeedDecay;

	eCurrentAction myCurrentAction;

	//struct SDriftEmitter
	//{
		int myLeftWheelDriftEmmiterHandle;
		int myRightWheelDriftEmmiterHandle;
		int myLeftDriftBoostEmitterhandle;
		int myRightDriftBoostEmitterhandle;
	//} myDriftEmitter;

	Physics::CPhysicsScene* myPhysicsScene;

	float myPreviousHeight[static_cast<int>(AxisPos::Size)];
	float myCurrentHeight[static_cast<int>(AxisPos::Size)];
	bool myFirstMovingPass;
};
