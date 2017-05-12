#include "stdafx.h"
#include "SpeedHandlerComponent.h"
#include "BoostData.h"
#include "SpeedHandlerManager.h"

CSpeedHandlerComponent::CSpeedHandlerComponent()
{
	myBoostList.Init(30);
}


CSpeedHandlerComponent::~CSpeedHandlerComponent()
{
}


void CSpeedHandlerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eGiveBoost:
	{
		for(unsigned int i = 0; i < myBoostList.Size(); i++)
		{
			if(myBoostList[i].data->hashedName == aMessageData.myBoostData->hashedName)
			{
				myBoostList[i].elapsedBoostingTime = 0.0f;
				return;
			}
		}

		SBoostListData boostdata;
		boostdata.data = aMessageData.myBoostData;
		boostdata.elapsedBoostingTime = 0.0f;
		myBoostList.Add(boostdata);
		SendSetBoostMessage();
	}
	default:
		break;
	}
}

void CSpeedHandlerComponent::Update(float aDeltaTime)
{
	for(unsigned int i = 0; i < myBoostList.Size(); i++)
	{
		if(myBoostList[i].data->duration > 0.0f)
		{
			if(myBoostList[i].elapsedBoostingTime < myBoostList[i].data->duration)
			{
				myBoostList[i].elapsedBoostingTime += aDeltaTime;
				if(myBoostList[i].elapsedBoostingTime >= myBoostList[i].data->duration)
				{
					myBoostList.RemoveCyclicAtIndex(i);
					SendSetBoostMessage();
				}
			}
		}
	}
}

const SBoostData* CSpeedHandlerComponent::GetFastestBoost() const
{
	float fastestBoostSpeed = -10000.0f;
	const SBoostData* returnData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("NoBoost"));
	for(unsigned int i = 0; i < myBoostList.Size(); i++)
	{
		if(fastestBoostSpeed < myBoostList[i].data->maxSpeedBoost)
		{
			fastestBoostSpeed = myBoostList[i].data->maxSpeedBoost;
			returnData = myBoostList[i].data;
		}
	}

	return returnData;
}

void CSpeedHandlerComponent::SendSetBoostMessage()
{
	SComponentMessageData setBoostData;
	setBoostData.myBoostData = GetFastestBoost();
	GetParent()->NotifyComponents(eComponentMessageType::eSetBoost, setBoostData);
}