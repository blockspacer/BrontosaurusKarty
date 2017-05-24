#pragma once
class CLapTrackerComponent;

struct SLapCalculateData
{
	CLapTrackerComponent* lapTrackerComponent;
	unsigned short reversePlacement;
	unsigned short lapIndex;
	unsigned short splineIndex;
	float nextSplineDistance;
};

class CLapTrackerComponentManager
{
public:
	CLapTrackerComponentManager();
	~CLapTrackerComponentManager();

	CLapTrackerComponent* CreateAndRegisterComponent();
	void Update(float aDeltaTime);
	void CalculateRacerPlacement();

private:
	void DoLapPlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	void DoSplinePlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	void DoDistanceToNextSplinePlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	void SortPlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
	void AddToRacerPlacements(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList);
private:
	CU::GrowingArray<CLapTrackerComponent*> myComponents;
	CU::GrowingArray<CGameObject*> myRacerPlacements;
	float myUpdatePlacementCountdown;
};

