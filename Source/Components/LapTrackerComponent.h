#pragma once
#include "Component.h"
class CLapTrackerComponent : public CComponent
{
public:
	CLapTrackerComponent();
	~CLapTrackerComponent();

	void Update();
private:
	unsigned short myLapIndex;
	unsigned short mySplineIndex;
};

