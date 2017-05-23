#include "stdafx.h"
#include "NavigationSpline.h"
#include "CommonUtilities.h"


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


