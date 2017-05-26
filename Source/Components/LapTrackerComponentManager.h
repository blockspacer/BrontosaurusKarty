#pragma once
#include "../ThreadedPostmaster/Subscriber.h"
class CLapTrackerComponent;

struct SLapCalculateData
{
	CLapTrackerComponent* lapTrackerComponent;
	unsigned short reversePlacement;
	unsigned short lapIndex;
	unsigned short splineIndex;
	float nextSplineDistance;
};

class CLapTrackerComponentManager : public Postmaster::ISubscriber
{
public:
	~CLapTrackerComponentManager();

	static void CreateInstance();
	static CLapTrackerComponentManager* GetInstance();
	static void DestoyInstance();

	CLapTrackerComponent* CreateAndRegisterComponent();
	void Update(float aDeltaTime);
	void CalculateRacerPlacement();
	void Init();
	CU::GrowingArray<CGameObject*>& GetRacerPlacements();
	eMessageReturn DoEvent(const CPlayerFinishedMessage& aPlayerFinishedMessage) override;
private:
	CLapTrackerComponentManager();
	void DoLapPlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	void DoSplinePlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	void DoDistanceToNextSplinePlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	void SortPlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	void AddToRacerPlacements(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	bool HaveAllPlayersFinished();
	void SendRaceOverMessage();
private:
	CU::GrowingArray<CLapTrackerComponent*> myComponents;
	CU::GrowingArray<CGameObject*> myRacerPlacements;
	CU::GrowingArray<CGameObject*> myWinnerPlacements;
	static CLapTrackerComponentManager* ourInstance;
	float myUpdatePlacementCountdown;

	bool myStartedWithOnlyOnePlayer;
};

