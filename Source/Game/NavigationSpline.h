#pragma once

class CGoalComponent;

namespace CU
{
	class CJsonValue;
}

struct SNavigationPoint
{
	SNavigationPoint SNagigationPoint(const SNavigationPoint* aCopy)
	{
		myPosition = aCopy->myPosition;
		myForwardDirection = aCopy->myForwardDirection;
		myWidth = aCopy->myWidth;
	}

	bool operator == (const SNavigationPoint aCompare)
	{
		if (aCompare.myPosition == myPosition && aCompare.myForwardDirection == myForwardDirection && aCompare.myWidth == myWidth)
		{
			return true;
		}
		return false;
	}

	CU::Vector2f myPosition;
	CU::Vector2f myForwardDirection;
	float myWidth;
	float myDistanceToGoal;
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
	const CU::GrowingArray<SNavigationPoint, short> GetNavigationPoints() const;
	void SetDistancesToGoal(CGoalComponent* aGoalComponent);

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

inline const CU::GrowingArray<SNavigationPoint, short> CNavigationSpline::GetNavigationPoints() const
{
	return myNavPoints;
}


