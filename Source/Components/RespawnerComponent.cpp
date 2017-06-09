#include "stdafx.h"
#include "RespawnerComponent.h"
#include "NavigationSpline.h"

const float takeNewGroundCooldown = 0.3f;
CRespawnerComponent::CRespawnerComponent()
{
	myGroundData.Init(20);
	myTakeNewGroundCountDown = 0.0f;
	myCurrentSplinePositionY = 0.0f;
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
		SComponentQuestionData spineDirectionQuestionData;
		if (GetParent()->AskComponents(eComponentQuestionType::eGetCurrentSpline, spineDirectionQuestionData) == true)
		{
			if(spineDirectionQuestionData.myNavigationPoint != nullptr)
			{
				GetParent()->SetWorldPosition(CU::Vector3f(spineDirectionQuestionData.myNavigationPoint->myPosition.x, myCurrentSplinePositionY + 5, spineDirectionQuestionData.myNavigationPoint->myPosition.y));
				CU::Vector3f direction = CU::Vector3f(spineDirectionQuestionData.myNavigationPoint->myForwardDirection.x, 0.0f, spineDirectionQuestionData.myNavigationPoint->myForwardDirection.y);
				CU::Vector3f LookTowardsDirection = direction + GetParent()->GetWorldPosition();
				GetParent()->GetLocalTransform().LookAt(LookTowardsDirection);
				myCurrentSplinePositionY += 5;

				if (GetParent()->AskComponents(eComponentQuestionType::eCheckIfShouldCheckAbove, SComponentQuestionData()) == true)
				{
					SComponentQuestionData groundedPositionData;
					if (GetParent()->AskComponents(eComponentQuestionType::eCheckIfGroundAbove, groundedPositionData) == true)
					{
						CU::Vector3f respawnPosition(groundedPositionData.myVector3f.x, groundedPositionData.myVector3f.y + 5.0f, groundedPositionData.myVector3f.z);
						GetParent()->SetWorldPosition(respawnPosition);
					}
				}
			}
		}
		break;
	}
	case eComponentMessageType::ePassedASpline:
	{
		myCurrentSplinePositionY = GetParent()->GetWorldPosition().y;
		break;
	}
	default:
		break;
	}
}