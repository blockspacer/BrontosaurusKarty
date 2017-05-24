#include "stdafx.h"
#include "LapTrackerComponentManager.h"
#include "LapTrackerComponent.h"

const float updatePlacementCooldown = 0.1f;

CLapTrackerComponentManager::CLapTrackerComponentManager()
{
	myComponents.Init(16);
	myRacerPlacements.Init(16);
	myUpdatePlacementCountdown = 0.0f;
}


CLapTrackerComponentManager::~CLapTrackerComponentManager()
{
}

CLapTrackerComponent* CLapTrackerComponentManager::CreateAndRegisterComponent()
{
	CLapTrackerComponent* lapTrackerComponent = nullptr;
	if(CComponentManager::GetInstancePtr() != nullptr)
	{
		lapTrackerComponent = new CLapTrackerComponent();
		myComponents.Add(lapTrackerComponent);
		CComponentManager::GetInstancePtr()->RegisterComponent(lapTrackerComponent);
	}
	return lapTrackerComponent;
}
void CLapTrackerComponentManager::Update(float aDeltaTime)
{
	for(unsigned int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update();
	}

	myUpdatePlacementCountdown -= aDeltaTime;
	if(myUpdatePlacementCountdown <= 0.0f)
	{
		CalculateRacerPlacement();
		myUpdatePlacementCountdown = updatePlacementCooldown;
	}
}

void CLapTrackerComponentManager::CalculateRacerPlacement()
{
	myRacerPlacements.RemoveAll();

	CU::GrowingArray<SLapCalculateData> racers;
	racers.Init(myComponents.Size() + 1);
	for (unsigned int i = 0; i < myComponents.Size(); i++)
	{
		SLapCalculateData lapCalculateData;
		lapCalculateData.lapTrackerComponent = myComponents[i];
		lapCalculateData.reversePlacement = 0;
		lapCalculateData.lapIndex = lapCalculateData.lapTrackerComponent->GetLapIndex();
		lapCalculateData.splineIndex = lapCalculateData.lapTrackerComponent->GetSplineIndex();
		lapCalculateData.nextSplineDistance = lapCalculateData.lapTrackerComponent->GetDistanceToNextSpline();
		racers.Add(lapCalculateData);
	}

	DoLapPlacement(racers);

	DoSplinePlacement(racers);

	DoDistanceToNextSplinePlacement(racers);

	SortPlacement(racers);

	AddToRacerPlacements(racers);
}

void CLapTrackerComponentManager::DoLapPlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList)
{
	for (unsigned int i = 0; i < aLapCalculateDataList.Size(); i++)
	{
		for (unsigned int j = i; j < aLapCalculateDataList.Size(); j++)
		{
			if (aLapCalculateDataList[i].lapIndex > aLapCalculateDataList[j].lapIndex && aLapCalculateDataList[i].reversePlacement < aLapCalculateDataList[j].reversePlacement)
			{
				aLapCalculateDataList[i].reversePlacement = aLapCalculateDataList[j].reversePlacement + 1;
			}
		}
	}
}
void CLapTrackerComponentManager::DoSplinePlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList)
{
	for (unsigned int i = 0; i < aLapCalculateDataList.Size(); i++)
	{
		for (unsigned int j = i; j < aLapCalculateDataList.Size(); j++)
		{
			if (aLapCalculateDataList[i].splineIndex > aLapCalculateDataList[j].splineIndex && aLapCalculateDataList[i].lapIndex == aLapCalculateDataList[j].lapIndex)
			{
				aLapCalculateDataList[i].reversePlacement = aLapCalculateDataList[j].reversePlacement + 1;
			}
		}
	}
}
void CLapTrackerComponentManager::DoDistanceToNextSplinePlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList)
{
	for (unsigned int i = 0; i < aLapCalculateDataList.Size(); i++)
	{
		for (unsigned int j = i; j < aLapCalculateDataList.Size(); j++)
		{
			if (aLapCalculateDataList[i].nextSplineDistance < aLapCalculateDataList[j].nextSplineDistance && aLapCalculateDataList[i].lapIndex == aLapCalculateDataList[j].lapIndex  && aLapCalculateDataList[i].splineIndex == aLapCalculateDataList[j].splineIndex)
			{
				aLapCalculateDataList[i].reversePlacement = aLapCalculateDataList[j].reversePlacement + 1;
			}
		}
	}
}
void CLapTrackerComponentManager::SortPlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList)
{
	for (unsigned int i = 0; i < aLapCalculateDataList.Size(); i++)
	{
		for (unsigned int j = i; j < aLapCalculateDataList.Size(); j++)
		{
			if (aLapCalculateDataList[i].reversePlacement < aLapCalculateDataList[j].reversePlacement)
			{

				SLapCalculateData tempLapCalculateData = aLapCalculateDataList[j];
				aLapCalculateDataList.RemoveAtIndex(j);
				aLapCalculateDataList.Insert(i, tempLapCalculateData);
			}
		}
	}
}
void CLapTrackerComponentManager::AddToRacerPlacements(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList)
{
	for (unsigned int i = 0; i < aLapCalculateDataList.Size(); i++)
	{
		myRacerPlacements.Add(aLapCalculateDataList[i].lapTrackerComponent->GetParent());
	}
}