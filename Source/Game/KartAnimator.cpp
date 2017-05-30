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

void CKartAnimator::AddAnimation(const eAnimationType aType)
{
	myEventQueue.insert(myEventQueue.begin(), CAnimationEvent());
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
	CAnimationEventFactory* animationEventFactory = CAnimationEventFactory::GetInstance();

	if (myTurnState != eTurnState::eRight && animationEventFactory)
	{
		myTurnState = eTurnState::eRight;

		myEventQueue.push_back(animationEventFactory->CreateEvent(CAnimationEventFactory::eEventType::eBeginRight, *this));
		myEventQueue.push_back(animationEventFactory->CreateEvent(CAnimationEventFactory::eEventType::eContinueRight, *this));
		myEventQueue.push_back(animationEventFactory->CreateEvent(CAnimationEventFactory::eEventType::eFinnishRight, *this));
	}
}

void CKartAnimator::OnTurnLeft(const float aNormalizedModifier)
{
	CAnimationEventFactory* animationEventFactory = CAnimationEventFactory::GetInstance();

	if (myTurnState != eTurnState::eLeft && animationEventFactory)
	{
		myTurnState = eTurnState::eLeft;

		myEventQueue.push_back(animationEventFactory->CreateEvent(CAnimationEventFactory::eEventType::eBeginRight, *this));
		myEventQueue.push_back(animationEventFactory->CreateEvent(CAnimationEventFactory::eEventType::eContinueRight, *this));
		myEventQueue.push_back(animationEventFactory->CreateEvent(CAnimationEventFactory::eEventType::eFinnishRight, *this));
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

void CKartAnimator::OnStopTurning()
{
	myTurnState = eTurnState::eNone;
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
