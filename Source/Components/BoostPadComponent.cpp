#include "stdafx.h"
#include "BoostPadComponent.h"
#include "SpeedHandlerManager.h"

CBoostPadComponent::CBoostPadComponent()
{
	myCollidesWithObjects.Init(10);
}


CBoostPadComponent::~CBoostPadComponent()
{
}

void CBoostPadComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnCollisionEnter:
	case eComponentMessageType::eOnTriggerEnter:
	{
		myCollidesWithObjects.Add(aMessageData.myComponent->GetParent());
		break;
	}
	case eComponentMessageType::eOnCollisionExit:
	case eComponentMessageType::eOnTriggerExit:
	{
		unsigned int foundIndex = myCollidesWithObjects.Find(aMessageData.myComponent->GetParent());
		if(foundIndex != myCollidesWithObjects.FoundNone)
		{
			myCollidesWithObjects.RemoveCyclicAtIndex(foundIndex);
		}
		break;
	}
	default:
		break;
	}
}

void CBoostPadComponent::Update()
{
	for(unsigned int i = 0; i < myCollidesWithObjects.Size(); i++)
	{
		SComponentMessageData boostMessageData;
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("BoostPad"));
		myCollidesWithObjects[i]->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
	}
}