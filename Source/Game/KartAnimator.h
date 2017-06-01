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

	void AddAnimation(const eEventType aType);
	void Update(const float aDeltaTime, const float aForwardVelocity);

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

	bool IsTurningRight() const;
	bool IsTurningLeft() const;
	bool IsBreaking() const;

private:
	std::vector<CAnimationEvent> myEventQueue;
	std::unique_ptr<CAnimationEvent> myDefaultAnimation;
	CModelComponent& myModelComponent;

	enum class eTurnState
	{
		eRight,
		eLeft,
		eNone
	} myTurnState;

	bool myIsBreaking;
	bool myIsGoingBackwards;
};
