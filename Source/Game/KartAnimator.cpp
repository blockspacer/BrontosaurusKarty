#include "stdafx.h"
#include "KartAnimator.h"
#include "AnimationEvent.h"
#include "ModelComponent.h"

#include "AnimationEventFactory.h"

#include "AnimationState.h"
#include "LoadManager\LoadManager.h"
#include "ModelComponentManager.h"

#include "..\BrontosaurusEngine\Engine.h"
#include "..\BrontosaurusEngine\ModelManager.h"
#include "..\BrontosaurusEngine\Model.h"
#include "..\BrontosaurusEngine\ModelInstance.h"

#undef CreateEvent

DECLARE_ANIMATION_ENUM_AND_STRINGS;

CKartAnimator::CKartAnimator(CModelComponent& aModelComponent)
	: myModelComponent(aModelComponent)
	, myTargetSteering(0.f)
	, myCurrentSteering(0.f)
	, mySteeringTimer(0.f)
	, myTurnState(eTurnState::eNone)
	, myIsBreaking(false)
	, myIsGoingBackwards(false)
{
	myDefaultAnimation = std::make_unique<CAnimationEvent>();
	myModelComponent.SetAnimationManualUpdate(true);
	myModelComponent.SetAnimationCounter(0.f);
	myModelComponent.SetNextAnimation(eAnimationState::none);
	myModelComponent.SetAnimationLerpValue(0.f);

	for (int i = 0; i < myWheels.Size(); ++i)
	{
		myWheels[i] = LoadManager::GetInstance()->GetCurrentPLaystate().GetGameObjectManager()->CreateGameObject();

		CGameObject* turnParent = LoadManager::GetInstance()->GetCurrentPLaystate().GetGameObjectManager()->CreateGameObject();
		CModelComponent* wheelModel = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_Kart_01_wheel.fbx");
		myWheels[i]->AddComponent(wheelModel);

		turnParent->AddComponent(myWheels[i]);

		myModelComponent.GetParent()->AddComponent(turnParent);
	}

	myWheels[0]->GetParent()->GetLocalTransform() = MODELMGR->GetModel(myModelComponent.GetModelInstance().GetModelID())->GetBoneTransform(0.f, eAnimationState::idle01, "P_wheelFront_SOCKET_left");
	myWheels[1]->GetParent()->GetLocalTransform() = MODELMGR->GetModel(myModelComponent.GetModelInstance().GetModelID())->GetBoneTransform(0.f, eAnimationState::idle01, "P_wheelFront_SOCKET_right");
	myWheels[2]->GetParent()->GetLocalTransform() = MODELMGR->GetModel(myModelComponent.GetModelInstance().GetModelID())->GetBoneTransform(0.f, eAnimationState::idle01, "P_wheelBack_SOCKET_left");
	myWheels[3]->GetParent()->GetLocalTransform() = MODELMGR->GetModel(myModelComponent.GetModelInstance().GetModelID())->GetBoneTransform(0.f, eAnimationState::idle01, "P_wheelBack_SOCKET_right");
}

CKartAnimator::~CKartAnimator()
{
}

void CKartAnimator::AddAnimation(const eEventType aType)
{
	CAnimationEventFactory* animationEventFactory = CAnimationEventFactory::GetInstance();
	myEventQueue.insert(myEventQueue.begin(), animationEventFactory->CreateEvent(aType, *this));
}

void CKartAnimator::Init()
{
}

void CKartAnimator::Update(const float aDeltaTime, const float aForwardVelocity, const float aSteering)
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

	if (myIsBreaking && aForwardVelocity < 0.f)
	{
		myIsBreaking = false;
	}

	myTargetSteering = aSteering;

	auto fLerp = [](float aStart, float aEnd, float aTime) -> float
	{
		return aEnd + aTime * (aStart - aEnd);
	};

	mySteeringTimer += aDeltaTime;
	if (mySteeringTimer > 1.f)
	{
		//mySteeringTimer -= 1.f;
		mySteeringTimer = 1.f;
	}
	myCurrentSteering = fLerp(0.f, myTargetSteering, mySteeringTimer);

	CU::Vector3f eulerRotation;
	eulerRotation.y = myCurrentSteering;
	eulerRotation.y = aSteering;

	myWheels[0]->GetParent()->GetLocalTransform().SetEulerRotation(eulerRotation);
	myWheels[1]->GetParent()->GetLocalTransform().SetEulerRotation(eulerRotation);

	for (CGameObject* wheelObject : myWheels)
	{
		wheelObject->GetLocalTransform().RotateAroundAxis(aForwardVelocity * aDeltaTime, CU::Axees::X);
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
		mySteeringTimer = 0.f;
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
		mySteeringTimer = 0.f;
	}
}

void CKartAnimator::OnStopMoving()
{
	myIsBreaking = false;
	myIsGoingBackwards = false;
}

void CKartAnimator::OnMoveFoward()
{
	myIsBreaking = false;
	myIsGoingBackwards = false;
}

void CKartAnimator::OnMoveBackWards(const float aCurrentSpeed2)
{
	if (myIsBreaking == false && aCurrentSpeed2 > 0.f)
	{
		myEventQueue.clear();
		AddAnimation(eEventType::eBeginBreak);
		AddAnimation(eEventType::eContinueBreak);
		AddAnimation(eEventType::eFinishBreak);
		myIsBreaking = true;
		myIsGoingBackwards = false;
	}
	else if (myIsGoingBackwards == false)
	{
		myIsBreaking = false;
		myIsGoingBackwards = true;
	}
}

void CKartAnimator::OnStopTurningLeft()
{
	if (myTurnState == eTurnState::eLeft)
	{
		myTurnState = eTurnState::eNone;
	}
	mySteeringTimer = 0.f;
}

void CKartAnimator::OnStopTurningRight()
{
	if (myTurnState == eTurnState::eRight)
	{
		myTurnState = eTurnState::eNone;
	}
	mySteeringTimer = 0.f;
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

bool CKartAnimator::IsBreaking() const
{
	return myIsBreaking;
}
