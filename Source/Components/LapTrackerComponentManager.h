#pragma once
#include "../ThreadedPostmaster/Subscriber.h"
class CLapTrackerComponent;

struct SLapCalculateData
{
	SLapCalculateData()
	{
		lapTrackerComponent = nullptr;
		reversePlacement = 0;
		placementValue = 0;
		nextSplineDistance = 0;
	}
	SLapCalculateData(const SLapCalculateData& aLapCalcData) 
	{
		lapTrackerComponent = aLapCalcData.lapTrackerComponent;
		reversePlacement = aLapCalcData.reversePlacement;
		placementValue = aLapCalcData.placementValue;
		nextSplineDistance = aLapCalcData.nextSplineDistance;
	}
	SLapCalculateData & operator=(const SLapCalculateData &aLapCalcData)
	{
		lapTrackerComponent = aLapCalcData.lapTrackerComponent;
		reversePlacement = aLapCalcData.reversePlacement;
		placementValue = aLapCalcData.placementValue;
		nextSplineDistance = aLapCalcData.nextSplineDistance;
		return *this;
	}
	CLapTrackerComponent* lapTrackerComponent;
	unsigned short reversePlacement;
	unsigned short placementValue;
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
	unsigned char GetSpecificRacerPlacement(CGameObject* aRacer);
	unsigned char GetSpecificRacerLapIndex(CGameObject* aRacer);

	eMessageReturn DoEvent(const CPlayerFinishedMessage& aPlayerFinishedMessage) override;
	eMessageReturn DoEvent(const CAIFinishedMessage& aAIFinishedMessage) override;


private:
	CLapTrackerComponentManager();
	void CalculateReversePlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	void SortPlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	void AddToRacerPlacements(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	bool HaveAllPlayersFinished();
	void SendRaceOverMessage();
	void AddEveryoneToVictoryList();
private:
	CU::GrowingArray<CLapTrackerComponent*> myComponents;
	CU::GrowingArray<CGameObject*> myRacerPlacements;
	CU::GrowingArray<CGameObject*> myWinnerPlacements;
	static CLapTrackerComponentManager* ourInstance;
	float myUpdatePlacementCountdown;

	bool myStartedWithOnlyOnePlayer;
};

