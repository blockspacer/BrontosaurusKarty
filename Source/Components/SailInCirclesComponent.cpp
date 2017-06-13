#include "stdafx.h"
#include "SailInCirclesComponent.h"


CSailInCirclesComponent::CSailInCirclesComponent(const float aRPM, const float aRadius)
{
	myRadius = aRadius;
	myRPS = aRPM / 60.f;
}

CSailInCirclesComponent::~CSailInCirclesComponent()
{
}

void CSailInCirclesComponent::Update(const float aPastTime)
{
	CU::Vector3f offset;
	offset.x = cosf(aPastTime * myRPS) * myRadius;
	offset.y = sinf(aPastTime) * -0.25f;
	offset.z = sinf(aPastTime * myRPS) * myRadius;

	CU::Vector3f forward;
	forward.x = -sinf(aPastTime * myRPS)	* myRPS < 0.0f ? -1.0f : 1.0f;
	forward.z = cosf(aPastTime * myRPS)		* myRPS < 0.0f ? -1.0f : 1.0f;

	GetParent()->GetLocalTransform().LookAt(GetParent()->GetLocalTransform().GetPosition() + forward);
	GetParent()->GetLocalTransform().myPosition = myInitialPosition + offset;

	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}

void CSailInCirclesComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eObjectDone:
		myInitialPosition = GetParent()->GetLocalTransform().GetPosition();
		break;
	default:
		break;
	}
}
