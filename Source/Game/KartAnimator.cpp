#include "stdafx.h"
#include "KartAnimator.h"
#include "AnimationEvent.h"
#include "ModelComponent.h"

#include "AnimationEventFactory.h"

#include "AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

CKartAnimator::CKartAnimator(CModelComponent& aModelComponent)
	: myModelComponent(aModelComponent)
	, myTurnState(eTurnState::eNone)
{
	myDefaultAnimation = std::make_unique<CAnimationEvent>();
	myModelComponent.SetAnimationManualUpdate(true);
	myModelComponent.SetAnimationCounter(0.f);
	myModelComponent.SetNextAnimation(eAnimationState::none);
	myModelComponent.SetAnimationLerpValue(0.f);
}

CKartAnimator::~CKartAnimator()
{
}

void CKartAnimator::AddAnimation(const eEventType aType)
{
	CAnimationEventFactory* animationEventFactory = CAnimationEventFactory::GetInstance();
	myEventQueue.insert(myEventQueue.begin(), animationEventFactory->CreateEvent(aType, *this));
}

void CKartAnimator::Update(const float aDeltaTime)
{
	CAnimationEvent* currentAnimation = myDefaultAnimation.get();
	if (!myEventQueue.empty())
	{
		currentAnimation = &myEventQueue.back();
	}

	myModelComponent.SetAnimation(currentAnimation->GetCurrent());
	myModelComponent.SetAnimationCounter(currentAnimation->GetAnimationCounter());

	if (!currentAnimation->Update(aDeltaTime))
	{
		myEventQueue.pop_back();
	}
}

void CKartAnimator::OnTurnRight(const float aNormalizedModifier)
{
	if (myTurnState != eTurnState::eRight)
	{
		myEventQueue.clear();
		if (myTurnState == eTurnState::eLeft)
		{
			AddAnimation(eEventType::eFinishLeft);
		}
		
		myTurnState = eTurnState::eRight;

		AddAnimation(eEventType::eBeginRight);
		AddAnimation(eEventType::eContinueRight);
		AddAnimation(eEventType::eFinishRight);
	}
}

void CKartAnimator::OnTurnLeft(const float aNormalizedModifier)
{
	if (myTurnState != eTurnState::eLeft)
	{
		myEventQueue.clear();
		if (myTurnState == eTurnState::eRight)
		{
			AddAnimation(eEventType::eFinishRight);
		}

		myTurnState = eTurnState::eLeft;

		AddAnimation(eEventType::eBeginLeft);
		AddAnimation(eEventType::eContinueLeft);
		AddAnimation(eEventType::eFinishLeft);
	}
}

void CKartAnimator::OnStopMoving()
{
}

void CKartAnimator::OnMoveFoward()
{
}

void CKartAnimator::OnMoveBackWards()
{
}

void CKartAnimator::OnStopTurningLeft()
{
	if (myTurnState == eTurnState::eLeft)
	{
		myTurnState = eTurnState::eNone;
	}
}

void CKartAnimator::OnStopTurningRight()
{
	if (myTurnState == eTurnState::eRight)
	{
		myTurnState = eTurnState::eNone;
	}
}

void CKartAnimator::OnDrift()
{
}

void CKartAnimator::OnStopDrifting()
{
}

void CKartAnimator::OnGetHit()
{
}

bool CKartAnimator::IsTurningRight() const
{
	return myTurnState == CKartAnimator::eTurnState::eRight;
}

bool CKartAnimator::IsTurningLeft() const
{
	return myTurnState == CKartAnimator::eTurnState::eLeft;
}
