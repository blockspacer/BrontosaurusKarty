#pragma once
#include "JsonValue.h"

struct SNavigationPoint
{
	CU::Vector2f myPosition;
	CU::Vector2f myForwardDirection;
	float myWidth;
};

class CNavigationSpline
{
public:
	CNavigationSpline();
	~CNavigationSpline();

	void AddPoint(const SNavigationPoint& aPoint);
	void LoadFromJson(const CU::CJsonValue& aJsonValue);
	const SNavigationPoint& GetPoint(const int anIndex) const;
	short GetNumberOfPoints() const;

private:
	CU::GrowingArray<SNavigationPoint,  short> myNavPoints;
};

inline void CNavigationSpline::AddPoint(const SNavigationPoint &aPoint)
{
	myNavPoints.Add(aPoint);
}

inline const SNavigationPoint& CNavigationSpline::GetPoint(const int anIndex) const
{
	return myNavPoints[anIndex];
}

inline short CNavigationSpline::GetNumberOfPoints() const
{
	return myNavPoints.Size();
}
