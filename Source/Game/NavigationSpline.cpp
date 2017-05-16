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
		navigationPoint.myPosition = pontData.at("postion").GetVector2f();

		const float direction = pontData.at("rotation").GetFloat() / 180 * PI + PI / 2;
		navigationPoint.myForwardDirection = CU::Vector2f(cos(direction), sin(direction));

		navigationPoint.myWidth = pontData.at("width").GetFloat();

		myNavPoints.Add(navigationPoint);
	}
}


