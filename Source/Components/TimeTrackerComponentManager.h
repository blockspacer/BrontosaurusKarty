#pragma once
#include "../ThreadedPostmaster/Subscriber.h"

class CTimeTrackerComponent;

class CTimeTrackerComponentManager : public Postmaster::ISubscriber
{
public:
	CTimeTrackerComponentManager();
	~CTimeTrackerComponentManager();

	CTimeTrackerComponent* CreateComponent();
	void Update(float aDeltaTime);

private:
	eMessageReturn DoEvent(const CRaceStartedMessage& aMessage) override;
	void RaceStart();

	CU::GrowingArray<CTimeTrackerComponent*, unsigned short> myComponents;
};

