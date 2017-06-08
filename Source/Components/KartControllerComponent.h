#pragma once
#include "Component.h"
#include "CurrentAction.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SetVibrationOnController.h"

namespace Physics
{
	class CPhysicsScene;
}

class CNavigationSpline;
class CParticleEmitterInstance;
class CKartControllerComponentManager;
class CDrifter;
class CKartAnimator;
class CModelComponent;

class CKartControllerComponent : public CComponent
{
public:
	CKartControllerComponent(CKartControllerComponentManager* aManager, CModelComponent& aModelComponent, const short aControllerIndex = -1, const short aCharacterIndex = 0);
	~CKartControllerComponent();

	void Turn(float aDirectionX);
	void TurnRight(const float aNormalizedModifier = 1.f);
	void TurnLeft(const float aNormalizedModifier = -1.f);
	void StopMoving();
	void MoveFoward();
	void MoveBackWards();
	void StopTurning();
	void StopDrifting(const bool aShouldGetBoost);
	void GetHit();
	void ApplyStartBoost();

	void IncreasePreGameBoostValue();
	void DecreasePreGameBoostValue();


	void Init(Physics::CPhysicsScene* aPhysicsScene);

	void CheckZKill();
	void Update(const float aDeltaTime);
	void CountDownUpdate(const float aDeltaTime);
	void Receive(const eComponentMessageType, const SComponentMessageData&) override;
	
	const CNavigationSpline& GetNavigationSpline();
	const CU::Vector3f& GetVelocity() const;


	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
	bool Drift();
	bool IsFutureGrounded(const float aDistance);

	inline bool GetIsGrounded() const;
	inline bool GetHitGround();

	inline float GetMaxSpeed() const;
	inline float GetMaxSpeed2() const;
	inline float GetMaxAcceleration() const;
	inline float GetAcceleratiot();
	inline bool GetIsControlledByAI() const;
	void LookBack(bool aLookBack);

private:
	
	void UpdateMovement(const float aDeltaTime);

	void DoCornerTest(unsigned aCornerIndex, const CU::Matrix33f& aRotationMatrix, const CU::Vector3f& aPosition, const float aHalfWidth, const float aLength);
	void CheckWallKartCollision(const float aDetltaTime);
	void DoPhysics(const float aDeltaTime);

	enum class AxisPos
	{
		RightBack,
		RightFront,
		LeftBack,
		LeftFront,
		Size
	};

	std::unique_ptr<CDrifter> myDrifter;
	std::unique_ptr<CKartAnimator> myAnimator;
	struct
	{
		float width = 1.f;
		float length = 1.f;
	} myAxisDescription;

	CU::Vector3f myVelocity;
	CU::Vector3f myGroundNormal;

	CKartControllerComponentManager* myManager;
	Physics::CPhysicsScene* myPhysicsScene;

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

	float myInvurnableTime;
	float myElapsedInvurnableTime;
	float myTimeToBeStunned;
	float myElapsedStunTime;
	float myDriftAngle;
	float myAirControl;

	float myDriftSetupTimer;
	float myDriftSetupTime;
	
	float myTerrainModifier;

	float myPreRaceBoostRate;
	float myPreRaceRate;
	float myPreRaceBoostValue;
	float myPreRaceBoostTarget;
	
	eCurrentAction myCurrentAction;

	int myBoostEmmiterhandle;
	int myGotHitEmmiterhandle;
	int myStarEmmiterhandle1;
	int myStarEmmiterhandle2;
	int mySlowMovment;
	int myGrassEmmiter1;
	int myGrassEmmiter2;

	short myControllerHandle;

	bool myIsOnGround;
	bool myCanAccelerate;
	bool myIsBoosting;

	bool myPreviousGotHit;
	bool myIsInvurnable;
	bool myHasGottenHit;
	bool myIsOnGroundLast;
	bool myIsAIControlled;
	bool increaseCountdownValue;
	bool myIsHoldingForward;
	CComponent* myLastGroundComponent;
	bool myLookingBack;
	bool myIsplayingEngineLoop;
};


inline bool CKartControllerComponent::GetIsGrounded() const
{
	return myIsOnGround;
}

bool CKartControllerComponent::GetHitGround()
{
	bool hitGround = false;
	if (myIsOnGround == true && myIsOnGroundLast == false)
	{
		hitGround = true;
		myIsOnGroundLast = true;
	}
	else if (myIsOnGround == false)
	{
		myIsOnGroundLast = false;
	}

	return hitGround;
}

float CKartControllerComponent::GetMaxSpeed() const
{
	return myMaxSpeed * (myIsBoosting == false ? myTerrainModifier : 1.f);
}

float CKartControllerComponent::GetMaxSpeed2() const
{
	return GetMaxSpeed() * GetMaxSpeed();
}

float CKartControllerComponent::GetMaxAcceleration() const
{
	return myMaxAcceleration;
}

bool CKartControllerComponent::GetIsControlledByAI() const
{
	return myIsAIControlled;
}

float CKartControllerComponent::GetAcceleratiot()
{
	return myAcceleration * (myIsBoosting == false ? myTerrainModifier : 1.f);
}
