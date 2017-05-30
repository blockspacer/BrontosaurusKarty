#include "stdafx.h"
#include "KartAnimator.h"
#include "AnimationEvent.h"

CKartAnimator::CKartAnimator()
{
}

CKartAnimator::~CKartAnimator()
{
}

void CKartAnimator::AddAnimation()
{
	myEventQueue.insert(myEventQueue.begin(), CAnimationEvent());
}

void CKartAnimator::Update(const float aDeltaTime)
{
	if (myEventQueue.empty())
	{
		return;
	}

	if (!myEventQueue.back().Update(aDeltaTime))
	{
		myEventQueue.pop_back();
	}
}
