#include "stdafx.h"
#include "LapTrackerComponent.h"
#include "NavigationSpline.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PlayerFinishedMessage.h"
#include "../ThreadedPostmaster/AIFinishedMessage.h"
#include "../ThreadedPostmaster/PlayerPassedGoalMessage.h"

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

				CU::Vector2f kartPosition2D(kartPosition.x, kartPosition.z);
				CU::Vector3f forward3D = GetParent()->GetToWorldTransform().myForwardVector;
				CU::Vector2f forward2D(forward3D.x, forward3D.z);
				forward2D.Normalize();
				forward2D *= 5.0f;
				kartPosition2D += forward2D;

				if (splineQuestionData.myNavigationPoint->myForwardDirection.Dot(splineQuestionData.myNavigationPoint->myPosition - kartPosition2D) < 0.0f)
				{
					mySplineIndex++;
					myPlacementValue++;
					GetParent()->NotifyOnlyComponents(eComponentMessageType::ePassedASpline, SComponentMessageData());
				}

				myTravelledDistance = myLapsTotalDistance - splineQuestionData.myNavigationPoint->myDistanceToGoal;
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
			return CU::Vector3f(splinePosition - GetParent()->GetWorldPosition()).Length();
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
	case eComponentQuestionType::eCheckIfShouldCheckAbove:
	{
		if(mySplineIndex >= 98 && mySplineIndex <= 105 )
		{
			return false;
		}
		return true;
		break;
	}
	case eComponentQuestionType::eGetLapTraversedPercentage:
	{
		aQuestionData.myFloat = GetLapDistanceTravelledPercentage();
		return true;
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
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerPassedGoalMessage(GetParent()));

			if (myLapIndex > 3)
			{
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

			if (myLapIndex == 4)
			{
				GetParent()->NotifyOnlyComponents(eComponentMessageType::eFinishedRace, SComponentMessageData());
				if (GetParent()->AskComponents(eComponentQuestionType::eHasCameraComponent, SComponentQuestionData()) == true)
				{
					Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerFinishedMessage(GetParent()));
				}
				else
				{
					Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAIFinishedMessage(GetParent()));
				}

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
			myLapsTotalDistance = splineQuestionData.myNavigationPoint->myDistanceToGoal;
		}
	}
}

const float CLapTrackerComponent::GetTotalTravelledDistance()
{
	float totalTravelledDistance = myTravelledDistance + (myLapIndex - 1) * myLapsTotalDistance;
	totalTravelledDistance -= GetDistanceToNextSpline();
	return totalTravelledDistance;
}

const float CLapTrackerComponent::GetLapDistanceTravelledPercentage()
{
	return (myTravelledDistance - GetDistanceToNextSpline()) / myLapsTotalDistance;
}