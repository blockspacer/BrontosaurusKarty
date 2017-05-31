#pragma once

enum class eAnimationState;

enum class eAnimationType
{
	eStartTurningRight,
	eContinueTurningRight,
	eFinnishTurningRight
};

class CAnimationEvent
{
public:
	CAnimationEvent();
	CAnimationEvent(const CAnimationEvent& aCopy) = default;
	CAnimationEvent(const std::function<bool(float)>& aShouldContinueCondition, const eAnimationState aAnimation, const float aStart, const float aEnd);
	~CAnimationEvent();

	bool Update(const float aDeltaTime);

	eAnimationState GetCurrent() const;
	float GetAnimationCounter();
	bool IsBeginner() const;

private:
	std::function<bool(float)> myShouldContinue;
	eAnimationState myAnimation;
	float myTimer;
	float myStart;
	float myEnd;
};
