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
		direction *= 3;
		myLastGroundedPosition = GetParent()->GetWorldPosition();
		myLastGroundedPosition += direction;
	}
}

void CRespawnerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eRespawn:
		GetParent()->SetWorldTransformation(CU::Matrix44f());
		GetParent()->SetWorldPosition(myLastGroundedPosition);
		break;
	default:
		break;
	}
}