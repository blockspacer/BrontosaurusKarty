#include "stdafx.h"
#include "AnimationEvent.h"

CAnimationEvent::CAnimationEvent()
	: myTimer(0.f)
{
}

CAnimationEvent::~CAnimationEvent()
{
}

bool CAnimationEvent::Update(const float aDeltaTime)
{
	return false;
}
