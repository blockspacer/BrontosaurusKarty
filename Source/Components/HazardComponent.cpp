#include "stdafx.h"
#include "HazardComponent.h"


CHazardComponent::CHazardComponent()
{
}


CHazardComponent::~CHazardComponent()
{
}

void CHazardComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnTriggerEnter:
	{
		aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eGotHit, SComponentMessageData());
		GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
	}
	break;
	default:
		break;
	}
}
