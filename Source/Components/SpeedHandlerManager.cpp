#include "stdafx.h"
#include "SpeedHandlerManager.h"
#include "SpeedHandlerComponent.h"
#include "BoostData.h"
#include "../CommonUtilities/JsonValue.h"

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
	noBoostData->hashedName = std::hash<std::string>()("NoBoost");
	myBoostDataList.Add(noBoostData);
	

	CU::CJsonValue boostList;
	std::string filePath = "Json/Boost/Boost.json";
	const std::string& errorString = boostList.Parse(filePath);
	CU::CJsonValue levelsArray = boostList.at("Boost");
	for (int i = 0; i < levelsArray.Size(); ++i)
	{
		std::string boostName = levelsArray[i].at("BoostName").GetString();
		SBoostData* loadedBoostData = new SBoostData();
		loadedBoostData->maxSpeedBoost = levelsArray[i].at("MaxSpeedPercentModifier").GetFloat() / 100.0f;
		loadedBoostData->accerationBoost = levelsArray[i].at("AccelerationPercentModifier").GetFloat() / 100.0f;
		loadedBoostData->duration = levelsArray[i].at("DurationSeconds").GetFloat();
		short hashedName = std::hash<std::string>()(boostName);
		loadedBoostData->hashedName = hashedName;
		myBoostDataList.Add(loadedBoostData);
	}
}
const SBoostData* CSpeedHandlerManager::GetData(short aHashedName) const
{
	for(unsigned int i = 0; i < myBoostDataList.Size(); i++)
	{
		if(myBoostDataList[i]->hashedName == aHashedName)
		{
			return myBoostDataList[i];
		}
	}

	DL_ASSERT("Couldn't find what boost to give type was %i", aHashedName);
	return nullptr;
}

void CSpeedHandlerManager::Update(float aDeltaTime)
{
	for (unsigned int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}

CSpeedHandlerComponent* CSpeedHandlerManager::CreateAndRegisterComponent()
{
	if(CComponentManager::GetInstancePtr() != nullptr)
	{
		CSpeedHandlerComponent* component = new CSpeedHandlerComponent();
		myComponents.Add(component);
		CComponentManager::GetInstancePtr()->RegisterComponent(component);
		return component;
	
	}
	return nullptr;
}