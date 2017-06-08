#pragma once

class CAnimationEvent;
class CAnimationEventFactory;
class CModelComponent;

enum class eEventType;

class CKartAnimator
{
public:
	CKartAnimator(CModelComponent& aModelComponent);
	~CKartAnimator();

	void Update(const float aDeltaTime, const float aForwardVelocity, const float aSteering);

	void OnTurnRight(const float aNormalizedModifier);
	void OnTurnLeft(const float aNormalizedModifier);
	void OnStopMoving();
	void OnMoveFoward();
	void OnMoveBackWards(const float aCurrentSpeed2);
	void OnStopTurningLeft();
	void OnStopTurningRight();
	void OnDrift();
	void OnStopDrifting();
	void OnGetHit();
	void OnStartBoosting();

	bool IsTurningRight() const;
	bool IsTurningLeft() const;
	bool IsBreaking() const;
	bool IsAccelerating() const;
	bool IsBoosting() const;

private:
	void AddAnimation(const eEventType aType);

	std::vector<CAnimationEvent> myEventQueue;
	std::unique_ptr<CAnimationEvent> myDefaultAnimation;
	CModelComponent& myModelComponent;

	enum class eTurnState
	{
		eRight,
		eLeft,
		eNone
	} myTurnState;

	enum class eWheels
	{
		eFrontLeft,
		eFrontRight,
		eRearLeft,
		eRearRight
	};

	CU::StaticArray<CGameObject*, 4> myWheels;
	bool myIsBreaking;
	bool myIsAccelerating;
	bool myIsGoingBackwards;
	bool myIsBoosting;
	bool myIsDrifting;
};
