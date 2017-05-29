#pragma once
#include "Component.h"
#include "CurrentAction.h"

namespace Physics
{
	class CPhysicsScene;
}

class CNavigationSpline;
class CParticleEmitterInstance;
class CKartControllerComponentManager;
class CDrifter;

class CKartControllerComponent : public CComponent
{
public:

	CKartControllerComponent(CKartControllerComponentManager* aManager);
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
	void GetHit();

	void CheckZKill();
	void Update(const float aDeltaTime);
	
	const CNavigationSpline& GetNavigationSpline();
	const CU::Vector3f& GetVelocity() const;

	void Receive(const eComponentMessageType, const SComponentMessageData&) override;
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
	void Init(Physics::CPhysicsScene* aPhysicsScene);

	bool IsFutureGrounded(const float aDistance);


private:
	void DoWallCollision(CColliderComponent& aCollider);
	void UpdateMovement(const float aDeltaTime);
	void DoDriftingParticles();

	//void SetHeight(float aHeight, const float aDt);
	//float GetHeightSpeed();
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
	

	std::unique_ptr<CDrifter> myDrifter;
	struct
	{
		float width = 1.f;
		float length = 1.f;
	} myAxisDescription;

	CU::Vector3f myVelocity;

	//float myFowrardSpeed;
	float myMaxSpeed;
	float myMinSpeed;

	float myAcceleration;
	float myMaxAcceleration;
	float myMinAcceleration;

	float myTurnRate;

	float myGrip;
	float myWeight;
	float mySteering;
	float myAngularAcceleration;

	float myMaxSpeedModifier;
	float myAccelerationModifier;
	float myModifierCopy;
	float myTargetSteering;

	float myBoostSpeedDecay;

	eCurrentAction myCurrentAction;

	//struct SDriftEmitter
	//{
		int myLeftWheelDriftEmmiterHandle;
		int myRightWheelDriftEmmiterHandle;
		int myLeftDriftBoostEmitterhandle;
		int myRightDriftBoostEmitterhandle;
		int myBoostEmmiterhandle;
	//} myDriftEmitter;

	Physics::CPhysicsScene* myPhysicsScene;

	bool myIsOnGround;
	bool myCanAccelerate;
	bool myIsBoosting;
	bool myHasJumped;

	bool myIsInvurnable;
	bool myHasGottenHit;
	float myInvurnableTime;
	float myElapsedInvurnableTime;
	float myTimeToBeStunned;
	float myElapsedStunTime;
	

	CKartControllerComponentManager* myManager;
	float myDriftAngle;
	float myAirControl;
};
