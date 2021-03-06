#include "stdafx.h"
#include "KartAnimator.h"
#include "AnimationEvent.h"
#include "ModelComponent.h"

#include "AnimationEventFactory.h"

#include "AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

#include "LoadManager\LoadManager.h"
#include "ModelComponentManager.h"

#include "..\BrontosaurusEngine\Engine.h"
#include "..\BrontosaurusEngine\ModelManager.h"
#include "..\BrontosaurusEngine\Model.h"
#include "..\BrontosaurusEngine\ModelInstance.h"

#undef CreateEvent

CKartAnimator::CKartAnimator(CModelComponent& aModelComponent, const CU::CJsonValue& aJsonValue)
	: myModelComponent(aModelComponent)
	, myTurnState(eTurnState::eNone)
	, myIsBreaking(false)
	, myIsAccelerating(false)
	, myIsGoingBackwards(false)
	, myIsBoosting(false)
	, myIsDrifting(false)
{
	myDefaultAnimation = std::make_unique<CAnimationEvent>();
	myModelComponent.SetAnimationManualUpdate(true);
	myModelComponent.SetAnimationCounter(0.f);
	myModelComponent.SetAnimation(eAnimationState::idle01);
	myModelComponent.SetNextAnimation(eAnimationState::none);
	myModelComponent.SetAnimationLerpValue(0.f);

	CU::CJsonValue wheels = aJsonValue["Wheels"];
	CModelComponent* wheelModels[4];
	wheelModels[0] = CModelComponentManager::GetInstance().CreateComponent(wheels["FrontLeft"].GetString());
	wheelModels[1] = CModelComponentManager::GetInstance().CreateComponent(wheels["FrontRight"].GetString());
	wheelModels[2] = CModelComponentManager::GetInstance().CreateComponent(wheels["BackLeft"].GetString());
	wheelModels[3] = CModelComponentManager::GetInstance().CreateComponent(wheels["BackRight"].GetString());

	for (int i = 0; i < myWheels.Size(); ++i)
	{
		myWheels[i] = LoadManager::GetInstance()->GetCurrentPLaystate().GetGameObjectManager()->CreateGameObject();

		CGameObject* turnParent = LoadManager::GetInstance()->GetCurrentPLaystate().GetGameObjectManager()->CreateGameObject();
		CModelComponent* wheelModel = wheelModels[i];
		wheelModel->SetIsShadowCasting(false);
		myWheels[i]->AddComponent(wheelModel);

		turnParent->AddComponent(myWheels[i]);

		myModelComponent.GetParent()->AddComponent(turnParent);
	}

	CModelManager* modelManager = MODELMGR;
	CModel* model = modelManager->GetModel(myModelComponent.GetModelInstance().GetModelID());
	if (!model)
	{
		DL_ASSERT("kart got non existing model :(");
		return;
	}

	CU::CJsonValue joints = aJsonValue["WheelJoints"];
	myWheels[0]->GetParent()->GetLocalTransform() = model->GetBoneTransform(0.f, eAnimationState::idle01, joints["FrontLeft"].GetString());
	myWheels[1]->GetParent()->GetLocalTransform() = model->GetBoneTransform(0.f, eAnimationState::idle01, joints["FrontRight"].GetString());
	myWheels[2]->GetParent()->GetLocalTransform() = model->GetBoneTransform(0.f, eAnimationState::idle01, joints["BackLeft"].GetString());
	myWheels[3]->GetParent()->GetLocalTransform() = model->GetBoneTransform(0.f, eAnimationState::idle01, joints["BackRight"].GetString());

	for (CGameObject* wheel : myWheels)
	{
		wheel->GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	}
}

CKartAnimator::~CKartAnimator()
{
}

void CKartAnimator::AddAnimation(const eEventType aType)
{
	CAnimationEventFactory* animationEventFactory = CAnimationEventFactory::GetInstance();
	myEventQueue.insert(myEventQueue.begin(), animationEventFactory->CreateEvent(aType, *this));
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

	CU::Vector3f eulerRotation;
	eulerRotation.y = aSteering;

	myWheels[0]->GetParent()->GetLocalTransform().SetEulerRotation(eulerRotation);
	myWheels[1]->GetParent()->GetLocalTransform().SetEulerRotation(eulerRotation);

	float movement = aForwardVelocity * aDeltaTime;
	for (int i = 0; i < 2; ++i)
	{
		myWheels[i]->GetLocalTransform().RotateAroundAxis(movement, CU::Axees::X);
	}

	for (int i = 2; i < 4; ++i)
	{
		myWheels[i]->GetLocalTransform().RotateAroundAxis(-movement, CU::Axees::X);
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
		//mySteeringTimer = 0.f;
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
	myIsBreaking = false;
	myIsAccelerating;
	myIsGoingBackwards = false;
}

void CKartAnimator::OnMoveFoward()
{
	if (!myIsAccelerating)
	{
		myEventQueue.clear();
		AddAnimation(eEventType::eBeginAccelerate);
		//AddAnimation(eEventType::eContinueAccelerate);
		AddAnimation(eEventType::eFinishAccelerate);
	}

	myIsAccelerating = true;

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

	myIsAccelerating = false;
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
	myIsDrifting = true;
}

void CKartAnimator::OnStopDrifting()
{
	myIsDrifting = false;
}

void CKartAnimator::OnGetHit()
{
}

void CKartAnimator::OnStartBoosting()
{
	if (!myIsBoosting)
	{
		myEventQueue.clear();

		AddAnimation(eEventType::eBeginBoost);
		AddAnimation(eEventType::eContinueBoost);
		AddAnimation(eEventType::eFinishBoost);
	}

	myIsBoosting = true;
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

bool CKartAnimator::IsAccelerating() const
{
	return myIsAccelerating;
}

bool CKartAnimator::IsBoosting() const
{
	return myIsBoosting;
}
