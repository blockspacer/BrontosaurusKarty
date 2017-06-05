#include "stdafx.h"
#include "LapTrackerComponent.h"
#include "NavigationSpline.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PlayerFinishedMessage.h"
#include "../ThreadedPostmaster/AIFinishedMessage.h"

CLapTrackerComponent::CLapTrackerComponent()
{
	myType = eComponentType::eLapTracker;
	mySplineIndex = 0;
	myLapIndex = 1;
	myPlacementValue = 0.0f;
	myIsReadyToEnterGoal = false;
	myTravelledDistance = 0.0f;
	myLapsTotalDistance = 0.0f;
}

CLapTrackerComponent::~CLapTrackerComponent()
{
}

void CLapTrackerComponent::Update()
{
	if (GetParent()->AskComponents(eComponentQuestionType::eGetIsGrounded, SComponentQuestionData()) == true)
	{
		SComponentQuestionData splineQuestionData;
		splineQuestionData.myInt = mySplineIndex;
		if (GetParent()->AskComponents(eComponentQuestionType::eGetSplineWithIndex, splineQuestionData) == true)
		{
			if (splineQuestionData.myNavigationPoint != nullptr)
			{
				CU::Vector2f splinePosition2D = splineQuestionData.myNavigationPoint->myPosition;
				CU::Vector2f splineDirection2D = splineQuestionData.myNavigationPoint->myForwardDirection;
				CU::Vector3f kartPosition = GetParent()->GetWorldPosition();
				CU::Vector3f splinePosition(splinePosition2D.x, kartPosition.y, splinePosition2D.y);
				CU::Vector3f splineForwardPosition;
				splineForwardPosition.x = splinePosition.x + splineDirection2D.x;
				splineForwardPosition.z = splinePosition.z + splineDirection2D.y;
				splineForwardPosition.y = kartPosition.y;

				float splineDistance = CU::Vector3f(splinePosition - kartPosition).Length2();
				float splineForwardDistance = CU::Vector3f(splineForwardPosition - kartPosition).Length2();

				if (splineForwardDistance < splineDistance)
				{
					mySplineIndex++;
					myPlacementValue++;
					GetParent()->NotifyOnlyComponents(eComponentMessageType::ePassedASpline, SComponentMessageData());
				}

				myTravelledDistance = myLapsTotalDistance - splineQuestionData.myNavigationPoint->myDistanceToGoal2;
			}
			else
			{
				myIsReadyToEnterGoal = true;
			}
		}
	}
}

const float CLapTrackerComponent::GetDistanceToNextSpline()
{
	SComponentQuestionData splineQuestionData;
	splineQuestionData.myInt = mySplineIndex + 1;
	if (GetParent()->AskComponents(eComponentQuestionType::eGetSplineWithIndex, splineQuestionData) == true)
	{
		if (splineQuestionData.myNavigationPoint == nullptr)
		{
			splineQuestionData.myInt = 0;
			GetParent()->AskComponents(eComponentQuestionType::eGetSplineWithIndex, splineQuestionData);
		}
		
		if (splineQuestionData.myNavigationPoint != nullptr)
		{
			CU::Vector3f splinePosition(splineQuestionData.myNavigationPoint->myPosition.x, GetParent()->GetWorldPosition().y, splineQuestionData.myNavigationPoint->myPosition.y);
			return CU::Vector3f(splinePosition - GetParent()->GetWorldPosition()).Length2();
		}
	}
	return 999999;
}

bool CLapTrackerComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eGetLapIndex :
	{
		aQuestionData.myInt = myLapIndex;
		return true;
		break;
	}
	case eComponentQuestionType::eGetCurrentSpline:
	{
		SComponentQuestionData splineQuestionData;
		splineQuestionData.myInt = mySplineIndex;
		if (GetParent()->AskComponents(eComponentQuestionType::eGetRespawnSplineWithIndex, splineQuestionData) == true)
		{
			aQuestionData.myNavigationPoint = splineQuestionData.myNavigationPoint;
			return true;
		}
		break;
	}
	default:
		break;
	}
	return false;
}

void CLapTrackerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eEnteredGoal:
	{
		if(myIsReadyToEnterGoal == true)
		{		




			mySplineIndex = 0;
			myLapIndex++;
			myPlacementValue++;
			myIsReadyToEnterGoal = false;

			if (myLapIndex > 3)
			{
				if (GetParent()->AskComponents(eComponentQuestionType::eHasCameraComponent, SComponentQuestionData()) == true)
				{
					Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerFinishedMessage(GetParent()));
				}
				else
				{
					Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAIFinishedMessage(GetParent()));
				}

				SComponentMessageData data;
				data.myString = "PlayFinish";
				GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, data);
			}
			else if (myLapIndex == 3)
			{
				SComponentMessageData data;
				data.myString = "PlayFinalLap";
				GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, data);
			}
			else
			{
				SComponentMessageData data;
				data.myString = "PlayLapDone";
				GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, data);
			}
		
		}
		break;
	}
	default:
		break;
	}
}

void CLapTrackerComponent::Init()
{
	SComponentQuestionData splineQuestionData;
	splineQuestionData.myInt = 0;
	if (GetParent()->AskComponents(eComponentQuestionType::eGetSplineWithIndex, splineQuestionData) == true)
	{
		if (splineQuestionData.myNavigationPoint != nullptr)
		{
			myLapsTotalDistance = splineQuestionData.myNavigationPoint->myDistanceToGoal2;
		}
	}
}