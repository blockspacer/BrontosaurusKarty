#pragma once
class CLapTrackerComponent;
class CLapTrackerComponentManager
{
public:
	CLapTrackerComponentManager();
	~CLapTrackerComponentManager();

	CLapTrackerComponent* CreateAndRegisterComponent();
	void Update();
private:
	CU::GrowingArray<CLapTrackerComponent*> myComponents;
};

