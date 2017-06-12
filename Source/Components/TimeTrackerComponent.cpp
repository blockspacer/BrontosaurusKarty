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
	case eComponentQuestionType::eGetFinishTimeMinutes:
	{
		float minutesPassed = myElapsedRaceTime / 60.0f;
		minutesPassed = floor(minutesPassed);
		aQuestionData.myFloat = minutesPassed;
		return true;
	}
	case eComponentQuestionType::eGetFinishTimeSeconds:
	{
		float minutesPassed = myElapsedRaceTime / 60.0f;
		minutesPassed = floor(minutesPassed);
		float secondsPassed = myElapsedRaceTime - minutesPassed * 60.0f;
		aQuestionData.myFloat = secondsPassed;
		return true;
	}
	break;
	default:
		break;
	}
	return false;
}