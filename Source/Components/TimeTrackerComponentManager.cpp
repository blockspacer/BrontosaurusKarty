#include "stdafx.h"
#include "TimeTrackerComponentManager.h"
#include "TimeTrackerComponent.h"

CTimeTrackerComponentManager::CTimeTrackerComponentManager()
{
	myComponents.Init(8);
}


CTimeTrackerComponentManager::~CTimeTrackerComponentManager()
{
}

void CTimeTrackerComponentManager::Update(float aDeltaTime)
{
	for(unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}
void CTimeTrackerComponentManager::RaceStart()
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->StartAddingTime();
	}
}

CTimeTrackerComponent* CTimeTrackerComponentManager::CreateAndRegisterComponent()
{
	CTimeTrackerComponent* newComponent = nullptr;
	if(CComponentManager::GetInstancePtr() != nullptr)
	{
		newComponent = new CTimeTrackerComponent();
		CComponentManager::GetInstancePtr()->RegisterComponent(newComponent);
		myComponents.Add(newComponent);
	}
	return newComponent;
}