#include "stdafx.h"
#include "LapTrackerComponent.h"
#include "NavigationSpline.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PlayerFinishedMessage.h"

CLapTrackerComponent::CLapTrackerComponent()
{
	mySplineIndex = 0;
	myLapIndex = 1;
}


CLapTrackerComponent::~CLapTrackerComponent()
{
}

void CLapTrackerComponent::Update()
{
	SComponentQuestionData splineQuestionData;
	splineQuestionData.myInt = mySplineIndex;
	if(GetParent()->AskComponents(eComponentQuestionType::eGetSplineWithIndex, splineQuestionData) == true)
	{
		if(splineQuestionData.myNavigationPoint != nullptr)
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

			if(splineForwardDistance < splineDistance)
			{
				mySplineIndex++;
			}

		}
		else
		{
			mySplineIndex = 0;
			myLapIndex++;
			if(myLapIndex > 3)
			{
				if(GetParent()->AskComponents(eComponentQuestionType::eHasCameraComponent, SComponentQuestionData()) == true)
				{
					Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerFinishedMessage(GetParent()));
				}
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
	default:
		break;
	}
	return false;
}