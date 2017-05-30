#include "stdafx.h"
#include "AnimationEvent.h"

#include "AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

CAnimationEvent::CAnimationEvent()
	: CAnimationEvent([](float) -> bool { return true; }, eAnimationState::idle01, 0.f, 10000.f)
{
}

CAnimationEvent::CAnimationEvent(const std::function<bool(float)>& aShouldContinueCondition, const eAnimationState aAnimation, const float aStart, const float aEnd)
	: myShouldContinue(aShouldContinueCondition)
	, myAnimation(aAnimation)
	, myTimer(0.f)
	, myStart(aStart)
	, myEnd(aEnd)
{
}

CAnimationEvent::~CAnimationEvent()
{
}

bool CAnimationEvent::Update(const float aDeltaTime)
{
	myTimer += aDeltaTime;
	return myShouldContinue(myTimer);
}

eAnimationState CAnimationEvent::GetCurrent() const
{
	return myAnimation;
}

float CAnimationEvent::GetAnimationCounter()
{
	if (myTimer + myStart >= myEnd)
	{
		if (myShouldContinue(myTimer))
		{
			myTimer = 0.f;
		}
	}

	return myTimer + myStart;
}
