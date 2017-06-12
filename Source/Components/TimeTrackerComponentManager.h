#pragma once
class CTimeTrackerComponent;
class CTimeTrackerComponentManager
{
public:
	CTimeTrackerComponentManager();
	~CTimeTrackerComponentManager();

	CTimeTrackerComponent* CreateComponent();
	void Update(float aDeltaTime);
	void RaceStart();
private:
	CU::GrowingArray<CTimeTrackerComponent*, unsigned short> myComponents;
};

