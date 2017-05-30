#include "stdafx.h"
#include "KartAnimatorComponent.h"
#include "AnimationEvent.h"

CKartAnimatorComponent::CKartAnimatorComponent()
{
	myType = eComponentType::eAnimationComponent;
}

CKartAnimatorComponent::~CKartAnimatorComponent()
{
}

void CKartAnimatorComponent::UpdateAnimations(const float aDeltaTime)
{
	for (CKartAnimatorComponent* component : ourComponents)
	{
		component->Update(aDeltaTime);
	}
}

void CKartAnimatorComponent::DeleteAnimations()
{
	ourComponents.DeleteAll();
}

void CKartAnimatorComponent::Update(const float aDeltaTime)
{
}
