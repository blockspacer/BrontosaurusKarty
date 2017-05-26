#include "stdafx.h"
#include "LapTrackerComponentManager.h"
#include "LapTrackerComponent.h"
#include "../ThreadedPostmaster/PlayerFinishedMessage.h"
#include "../ThreadedPostmaster/AIFinishedMessage.h"
#include "../ThreadedPostmaster/MessageType.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/RaceOverMessage.h"

CLapTrackerComponentManager* CLapTrackerComponentManager::ourInstance = nullptr;
const float updatePlacementCooldown = 0.1f;

CLapTrackerComponentManager::CLapTrackerComponentManager()
{
	myComponents.Init(16);
	myRacerPlacements.Init(16);
	myWinnerPlacements.Init(16);
	myUpdatePlacementCountdown = 0.0f;
	myStartedWithOnlyOnePlayer = false;
}


CLapTrackerComponentManager::~CLapTrackerComponentManager()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

void CLapTrackerComponentManager::CreateInstance()
{
	if(ourInstance == nullptr)
	{
		ourInstance = new CLapTrackerComponentManager();
	}
}
CLapTrackerComponentManager* CLapTrackerComponentManager::GetInstance()
{
	return ourInstance;
}

void CLapTrackerComponentManager::DestoyInstance()
{
	if(ourInstance != nullptr)
	{
		SAFE_DELETE(ourInstance);
	}
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

CU::GrowingArray<CGameObject*>& CLapTrackerComponentManager::GetRacerPlacements()
{
	return myRacerPlacements;
}

eMessageReturn CLapTrackerComponentManager::DoEvent(const CPlayerFinishedMessage& aPlayerFinishedMessage)
{
	for (unsigned int i = 0; i < myComponents.Size(); i++)
	{
		if(myComponents[i]->GetParent() == aPlayerFinishedMessage.GetGameObject())
		{
			myWinnerPlacements.Add((myComponents[i]->GetParent()));
			myComponents.RemoveCyclicAtIndex(i);
		}
	}

	if(HaveAllPlayersFinished() == true)
	{
		SendRaceOverMessage();
	}
	else if(myComponents.Size() <= 1 && myStartedWithOnlyOnePlayer == false)
	{
		SendRaceOverMessage();
	}

	return eMessageReturn::eContinue;
}

eMessageReturn CLapTrackerComponentManager::DoEvent(const CAIFinishedMessage& aAIFinishedMessage)
{
	for (unsigned int i = 0; i < myComponents.Size(); i++)
	{
		if (myComponents[i]->GetParent() == aAIFinishedMessage.GetGameObject())
		{
			myWinnerPlacements.Add((myComponents[i]->GetParent()));
			myComponents.RemoveCyclicAtIndex(i);
		}
	}

	if (myComponents.Size() <= 1 && myStartedWithOnlyOnePlayer == false)
	{
		SendRaceOverMessage();
	}

	return eMessageReturn::eContinue;
}

bool CLapTrackerComponentManager::HaveAllPlayersFinished()
{
	bool havePlayersFinished = true;
	
	for (unsigned int i = 0; i < myComponents.Size(); i++)
	{
		if(myComponents[i]->GetParent()->AskComponents(eComponentQuestionType::eHasCameraComponent, SComponentQuestionData()) == true)
		{
			havePlayersFinished = false;
		}
	}

	return havePlayersFinished;
}

void CLapTrackerComponentManager::Init()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::ePlayerFinished);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eAIFInished);

	if(myComponents.Size() == 1)
	{
		myStartedWithOnlyOnePlayer = true;
	}
}

void CLapTrackerComponentManager::SendRaceOverMessage()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CRaceOverMessage(myWinnerPlacements));
}