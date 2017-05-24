#include "stdafx.h"
#include "GoalComponent.h"


CGoalComponent::CGoalComponent()
{
}


CGoalComponent::~CGoalComponent()
{
}

void CGoalComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnTriggerEnter:
	{
		aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eEnteredGoal, SComponentMessageData());
	}
	default:
		break;
	}
}