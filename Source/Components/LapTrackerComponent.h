#pragma once
#include "Component.h"
class CLapTrackerComponent : public CComponent
{
public:
	CLapTrackerComponent();
	~CLapTrackerComponent();

	void Update();
	inline const unsigned short GetLapIndex() const;
	inline const unsigned short GetSplineIndex() const;
	const float GetDistanceToNextSpline();
private:
	unsigned short myLapIndex;
	unsigned short mySplineIndex;
};

inline const unsigned short CLapTrackerComponent::GetLapIndex() const
{
	return myLapIndex;
}

inline const unsigned short CLapTrackerComponent::GetSplineIndex() const
{
	return mySplineIndex;
}
