#include "stdafx.h"
#include "../CommonUtilities/JsonValue.h"
#include "NavigationSpline.h"
#include "CommonUtilities.h"
#include "GoalComponent.h"

CNavigationSpline::CNavigationSpline()
{
	myNavPoints.Init(1);
}


CNavigationSpline::~CNavigationSpline()
{
}

void CNavigationSpline::LoadFromJson(const CU::CJsonValue& aJsonData)
{
	CU::CJsonValue pointsArray = aJsonData.at("points");

	myNavPoints.ReInit(pointsArray.Size());

	for (unsigned i = 0; i < pointsArray.Size(); ++i)
	{
		CU::CJsonValue pontData = pointsArray.at(i);

		SNavigationPoint navigationPoint;
		navigationPoint.myPosition = pontData.at("position").GetVector2f();
		navigationPoint.myPosition *= -1;

		const float direction = pontData.at("rotation").GetFloat() / 180 * PI + PI / 2;
		navigationPoint.myForwardDirection = CU::Vector2f(cos(direction), sin(direction));
		navigationPoint.myForwardDirection.y *= -1;

		navigationPoint.myWidth = pontData.at("width").GetFloat();

		myNavPoints.Add(navigationPoint);
	}
}


void CNavigationSpline::SetDistancesToGoal(CGoalComponent* aGoalComponent)
{
	for(short i = myNavPoints.Size() - 1; i >= 0; i--)
	{
		CU::Vector2f currentPosition = myNavPoints[i].myPosition;
		CU::Vector2f nextPosition;
		float previousDistance2;
		if(i == myNavPoints.Size() - 1)
		{
			CU::Vector2f goalComponent(aGoalComponent->GetParent()->GetWorldPosition().x, aGoalComponent->GetParent()->GetWorldPosition().z);
			nextPosition = goalComponent;

			previousDistance2 = 0;
		}
		else
		{
			nextPosition = myNavPoints[i + 1].myPosition;
			previousDistance2 = myNavPoints[i + 1].myDistanceToGoal2;
		}

		float distance2 = CU::Vector2f(nextPosition - currentPosition).Length2() + previousDistance2;
		myNavPoints[i].myDistanceToGoal2 = distance2;
	}
}