#include "stdafx.h"
#include "LapTrackerComponent.h"
#include "NavigationSpline.h"

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
	if(GetParent()->AskComponents(eComponentQuestionType::eGetSplineWithIndex, splineQuestionData))
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
	}
}