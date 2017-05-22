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
		GetParent()->NotifyComponents(eComponentMessageType::eGotHit, SComponentMessageData());
	}
	break;
	default:
		break;
	}
}
