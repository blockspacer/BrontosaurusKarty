#include "stdafx.h"
#include "TimeTrackerComponent.h"


CTimeTrackerComponent::CTimeTrackerComponent()
{
	myElapsedRaceTime = 0.0f;
	myShouldAddTime = false;
}


CTimeTrackerComponent::~CTimeTrackerComponent()
{
}

void CTimeTrackerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case  eComponentMessageType::eFinishedRace :
	{
		myShouldAddTime = false;
		break;
	}
	default:
		break;
	}
}
void CTimeTrackerComponent::Update(float aDeltaTime)
{
	if(myShouldAddTime == true)
	{
		myElapsedRaceTime += aDeltaTime;
	}
}

bool CTimeTrackerComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eGetFinishTime:
	{
		aQuestionData.myFloat = myElapsedRaceTime;
		return true;
	}
	break;
	default:
		break;
	}
	return false;
}