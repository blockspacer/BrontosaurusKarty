#include "stdafx.h"
#include "SpeedHandlerManager.h"
#include "SpeedHandlerComponent.h"
#include "BoostData.h"

CSpeedHandlerManager* CSpeedHandlerManager::ourInstance = nullptr;

CSpeedHandlerManager::CSpeedHandlerManager()
{
}


CSpeedHandlerManager::~CSpeedHandlerManager()
{
}

void CSpeedHandlerManager::CreateInstance()
{
	if(ourInstance == nullptr)
	{
		ourInstance = new CSpeedHandlerManager();
	}
}
void CSpeedHandlerManager::Destroy()
{
	if(ourInstance != nullptr)
	{
		SAFE_DELETE(ourInstance);
	}
}
CSpeedHandlerManager* CSpeedHandlerManager::GetInstance()
{
	return ourInstance;
}
void CSpeedHandlerManager::Init()
{
	myComponents.Init(16);
	myBoostDataList.Init(16);
	LoadBoostData();
}
void CSpeedHandlerManager::LoadBoostData()
{
	SBoostData* noBoostData = new SBoostData();
	noBoostData->accerationBoost = 0.0f;
	noBoostData->duration = 0.0f;
	noBoostData->maxSpeedBoost = 0.0f;
	noBoostData->type = eBoostType::eNoBoost;
	myBoostDataList.Add(noBoostData);
	

	// DO JSon stuff or something;
}
const SBoostData* CSpeedHandlerManager::GetData(eBoostType aType) const
{
	for(unsigned int i = 0; i < myBoostDataList.Size(); i++)
	{
		if(myBoostDataList[i]->type == aType)
		{
			return myBoostDataList[i];
		}
	}
	DL_ASSERT("Couldn't find what boost to give type was %u", aType);
	return nullptr;
}

void CSpeedHandlerManager::Update(float aDeltaTime)
{
	for (unsigned int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}