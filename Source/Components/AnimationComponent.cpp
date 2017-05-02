#include "stdafx.h"
#include "AnimationComponent.h"
#include "ModelComponent.h"
#include "AnimationComponentLoader.h"

#include "../CommonUtilities/JsonValue.h"
#include "../TShared/AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

#define NU_HAR_JAG_DRAMATISERAT_KLART_MIN_D�D 3.14f

CU::GrowingArray<CAnimationComponent*> CAnimationComponent::ourAnimations(64u);

CAnimationComponent::CAnimationComponent(CModelComponent& aModelCompoent)
	: myAnimationStack(4u)
	, myModelComponent(aModelCompoent)
	//, myUpdateHasRun(false)
{
	myType = eComponentType::eAnimationComponent;

	AnimationComponentLoader::LoadAnimations(myModelComponent, myAnimationStates);

	PushAnimation(myAnimationStates["idle"]);

	ourAnimations.Add(this);
}

CAnimationComponent::~CAnimationComponent()
{
	ourAnimations.RemoveCyclic(this);
}

void CAnimationComponent::Update(const CU::Time aDeltaTime)
{
	if (myAnimationStack.Empty())
	{
		return;
	}

	SAnimation* currentAnimation = &myAnimationStack.GetLast();

	if (currentAnimation->myBlendTime > 0.f)
	{
		if (currentAnimation->myBlendTime < currentAnimation->myAnimationBlend)
		{
			currentAnimation->myAnimationBlend += aDeltaTime.GetSeconds();
		}
	}

	CU::Vector3f position3D = GetParent()->GetToWorldTransform().GetPosition();
	CU::Vector2f position(position3D.x, position3D.z);
	CU::Vector2f velocity = position - myLastPosition;
	myLastPosition = position;

	float speed = velocity.Length();

	if (speed > 0.01f && currentAnimation->myAnimationKey == eAnimationState::idle01)
	{
		auto it = myAnimationStates.find("moving");
		if (it != myAnimationStates.end())
		{
			currentAnimation = &it->second;
			currentAnimation->myAnimationBlend = 0.f;
		}
	}

	myModelComponent.SetAnimation(currentAnimation->myAnimationKey);
	myModelComponent.SetNextAnimation(currentAnimation->myNextAnimationKey);
	myModelComponent.SetAnimationLerpValue(currentAnimation->myAnimationBlend);
	myModelComponent.SetAnimationLooping(currentAnimation->myIsLooping);

	if (currentAnimation->myCoolDownTime != -1.f)
	{
		currentAnimation->myCoolDownTime -= aDeltaTime.GetSeconds();
		if (currentAnimation->myCoolDownTime <= 0.f)
		{
			if (currentAnimation->myNextAnimationKey == eAnimationState::invisible)
			{
				myModelComponent.SetVisibility(false);
			}

			myAnimationStack.Pop();

			if (myOnFinnishedCallback != nullptr)
			{
				myOnFinnishedCallback();
				myOnFinnishedCallback = nullptr;
			}
		}
	}
}

void CAnimationComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	if (myAnimationStates.empty()) return;
	const eAnimationState lastAnimationKey = myAnimationStack.GetLast().myAnimationKey;
	switch (aMessageType)
	{
	}
}

bool CAnimationComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	return false;
}

void CAnimationComponent::UpdateAnimations(const CU::Time aDeltaTime)
{
	for (CAnimationComponent* animationComponent : ourAnimations)
	{
		animationComponent->Update(aDeltaTime);
	}
}

bool CAnimationComponent::TryPushAnimation(const std::string& aStr)
{
	std::map<std::basic_string<char>, SAnimation>::iterator it = myAnimationStates.find(aStr);
	if (it != myAnimationStates.end())
	{
		PushAnimation(it->second);
		return true;
	}
	return false;
}

void CAnimationComponent::PushAnimation(const SAnimation& aAnimation)
{
	myAnimationStack.Add(aAnimation);
	myModelComponent.ResetAnimation();
}
