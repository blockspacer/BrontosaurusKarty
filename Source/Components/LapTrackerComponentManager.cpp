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
		lapCalculateData.placementValue = lapCalculateData.lapTrackerComponent->GetPlacementValue();
		lapCalculateData.reversePlacement = 0;
		lapCalculateData.nextSplineDistance = lapCalculateData.lapTrackerComponent->GetDistanceToNextSpline();
		racers.Add(lapCalculateData);
	}

	CalculateReversePlacement(racers);
	CalculateReversePlacement(racers);

	SortPlacement(racers);

	//AddToRacerPlacements(racers);
}

void CLapTrackerComponentManager::CalculateReversePlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList)
{
	for (unsigned int i = 0; i < aLapCalculateDataList.Size(); i++)
	{
		for (unsigned int j = 0; j < aLapCalculateDataList.Size(); j++)
		{
			if (i == j)
			{
				continue;
			}

			if (aLapCalculateDataList[i].placementValue > aLapCalculateDataList[j].placementValue)
			{
				aLapCalculateDataList[i].reversePlacement++;
			}

			if(aLapCalculateDataList[i].placementValue == aLapCalculateDataList[j].placementValue && aLapCalculateDataList[i].nextSplineDistance < aLapCalculateDataList[j].nextSplineDistance)
			{
				aLapCalculateDataList[i].reversePlacement++;
			}
			
		}
	}
}
void CLapTrackerComponentManager::SortPlacement(CU::GrowingArray<SLapCalculateData>& aLapCalculateDataList)
{
	while (aLapCalculateDataList.Size() > 0)
	{
		unsigned short largestValuesIndex = 0;
		unsigned short largestValue = 0;
		for (unsigned int i = 0; i < aLapCalculateDataList.Size(); i++)
		{
			if (aLapCalculateDataList[i].reversePlacement > largestValue)
			{
				largestValue = aLapCalculateDataList[i].reversePlacement;
				largestValuesIndex = i;
			}
		}

		myRacerPlacements.Add(aLapCalculateDataList[largestValuesIndex].lapTrackerComponent->GetParent());
		aLapCalculateDataList.RemoveCyclicAtIndex(largestValuesIndex);
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
		if (myWinnerPlacements.Find(myRacerPlacements[i]) != myWinnerPlacements.FoundNone)
		{
			continue;
		}
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
	AddEveryoneToVictoryList();
	myWinnerPlacements;
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CRaceOverMessage(myWinnerPlacements));
}

unsigned char CLapTrackerComponentManager::GetSpecificRacerPlacement(CGameObject* aRacer)
{
	if(myRacerPlacements.Size() < 2)
	{
		return 1;
	}
	return myRacerPlacements.Find(aRacer) + 1;
}

unsigned char CLapTrackerComponentManager::GetSpecificRacerLapIndex(CGameObject* aRacer)
{
	SComponentQuestionData whatLap;
	aRacer->AskComponents(eComponentQuestionType::eGetLapIndex, whatLap);

	return whatLap.myChar;
}

void CLapTrackerComponentManager::AddEveryoneToVictoryList()
{
	for(unsigned int i = 0; i < myRacerPlacements.Size(); i++)
	{
		if(myWinnerPlacements.Find(myRacerPlacements[i]) == myWinnerPlacements.FoundNone)
		{
			myWinnerPlacements.Add(myRacerPlacements[i]);
		}
	}
}