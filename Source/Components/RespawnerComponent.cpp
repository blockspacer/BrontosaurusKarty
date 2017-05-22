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
		myLastGroundedPosition = GetParent()->GetWorldPosition();
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