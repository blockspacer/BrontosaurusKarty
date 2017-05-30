#pragma once

class CAnimationEvent;
class CAnimationEventFactory;
class CModelComponent;

enum class eAnimationType;

class CKartAnimator
{
public:
	CKartAnimator(CModelComponent& aModelComponent);
	~CKartAnimator();

	void AddAnimation(const eAnimationType aType);
	void Update(const float aDeltaTime);

	void OnTurnRight(const float aNormalizedModifier);
	void OnTurnLeft(const float aNormalizedModifier);
	void OnStopMoving();
	void OnMoveFoward();
	void OnMoveBackWards();
	void OnStopTurning();
	void OnDrift();
	void OnStopDrifting();
	void OnGetHit();

	bool IsTurningRight() const;
	bool IsTurningLeft() const;

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
};
