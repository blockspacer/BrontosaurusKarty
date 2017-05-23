#include "stdafx.h"
#include "RespawnerComponent.h"


CRespawnerComponent::CRespawnerComponent()
{
}


CRespawnerComponent::~CRespawnerComponent()
{
}

void CRespawnerComponent::Update()
{
	UpdateGroundedPosition();
}

void CRespawnerComponent::UpdateGroundedPosition()
{
	SComponentQuestionData groundedPositionData;
	if(GetParent()->AskComponents(eComponentQuestionType::eGetIsGrounded, groundedPositionData) == true)
	{
		CU::Vector3f direction = myLastGroundedPosition - GetParent()->GetWorldPosition();
		direction.Normalize();
		direction *= 5;
		myLastGroundedPosition = GetParent()->GetWorldPosition();
		myLastGroundedPosition += direction;
	}
}

void CRespawnerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eRespawn:
	{
		GetParent()->SetWorldTransformation(CU::Matrix44f());
		GetParent()->SetWorldPosition(myLastGroundedPosition);
		SComponentQuestionData spineDirectionQuestionData;
		if (GetParent()->AskComponents(eComponentQuestionType::eGetSplineDirection, spineDirectionQuestionData) == true)
		{
			CU::Vector3f direction = spineDirectionQuestionData.myVector3f;
			CU::Vector3f LookTowardsDirection = direction + myLastGroundedPosition;
			GetParent()->GetLocalTransform().LookAt(LookTowardsDirection);
		}
		break;
	}
	default:
		break;
	}
}