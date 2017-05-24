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

	for (unsigned int i = 0; i < racers.Size(); i++)
	{
		for (unsigned int j = i; j < racers.Size(); j++)
		{
			if(racers[i].lapIndex > racers[j].lapIndex && racers[i].reversePlacement <  racers[j].reversePlacement)
			{
				racers[i].reversePlacement = racers[j].reversePlacement + 1;
			}
		}
	}

	for (unsigned int i = 0; i < racers.Size(); i++)
	{
		for (unsigned int j = i; j < racers.Size(); j++)
		{
			if (racers[i].splineIndex > racers[j].splineIndex && racers[i].lapIndex == racers[j].lapIndex)
			{
				racers[i].reversePlacement = racers[j].reversePlacement + 1;
			}
		}
	}

	for (unsigned int i = 0; i < racers.Size(); i++)
	{
		for (unsigned int j = i; j < racers.Size(); j++)
		{
			if (racers[i].nextSplineDistance < racers[j].nextSplineDistance && racers[i].lapIndex == racers[j].lapIndex  && racers[i].splineIndex == racers[j].splineIndex)
			{
				racers[i].reversePlacement = racers[j].reversePlacement + 1;
			}
		}
	}

	for (unsigned int i = 0; i < racers.Size(); i++)
	{
		for (unsigned int j = i; j < racers.Size(); j++)
		{
			if(racers[i].reversePlacement < racers[j].reversePlacement)
			{

				SLapCalculateData tempLapCalculateData = racers[j];
				racers.RemoveAtIndex(j);
				racers.Insert(i, tempLapCalculateData);
			}
		}
	}

	for (unsigned int i = 0; i < racers.Size(); i++)
	{
		myRacerPlacements.Add(racers[i].lapTrackerComponent->GetParent());
	}
}