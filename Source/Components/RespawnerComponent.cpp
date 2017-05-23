#include "stdafx.h"
#include "RespawnerComponent.h"

const float takeNewGroundCooldown = 0.3f;
CRespawnerComponent::CRespawnerComponent()
{
	myGroundData.Init(20);
	myTakeNewGroundCountDown = 0.0f;
}


CRespawnerComponent::~CRespawnerComponent()
{
}

void CRespawnerComponent::Update(float aDeltaTime)
{
	UpdateGroundedPosition(aDeltaTime);
}

void CRespawnerComponent::UpdateGroundedPosition(float aDeltaTime)
{
	myTakeNewGroundCountDown -= aDeltaTime;
	SComponentQuestionData groundedPositionData;
	if(GetParent()->AskComponents(eComponentQuestionType::eGetIsGrounded, groundedPositionData) == true)
	{
		for(unsigned int i = 0; i < myGroundData.Size(); i++)
		{
			myGroundData[i].timeSinceCreated += aDeltaTime;
			if(myGroundData[i].timeSinceCreated > 1.0f)
			{
				myGroundData.RemoveAtIndex(i);
			}
		}

		if(myTakeNewGroundCountDown < 0.0f)
		{
			myTakeNewGroundCountDown = takeNewGroundCooldown;
			SLastGroundData lastGroundData;
			lastGroundData.lastGroundPosition = GetParent()->GetWorldPosition();
			myGroundData.Add(lastGroundData);
		}
	}
}

void CRespawnerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eRespawn:
	{
		GetParent()->SetWorldTransformation(CU::Matrix44f());
		GetParent()->SetWorldPosition(myGroundData[0].lastGroundPosition);
		myGroundData[0].lastGroundPosition.Print();
		SComponentQuestionData spineDirectionQuestionData;
		if (GetParent()->AskComponents(eComponentQuestionType::eGetSplineDirection, spineDirectionQuestionData) == true)
		{
			CU::Vector3f direction = spineDirectionQuestionData.myVector3f;
			CU::Vector3f LookTowardsDirection = direction + myGroundData[0].lastGroundPosition;
			GetParent()->GetLocalTransform().LookAt(LookTowardsDirection);
		}
		break;
	}
	default:
		break;
	}
}