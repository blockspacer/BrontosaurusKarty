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
		if(myShouldAddTime == true)
		{
			aQuestionData.myInt = 0;
		}
		else
		{
			float minutesPassed = myElapsedRaceTime / 60.0f;
			minutesPassed = floor(minutesPassed);
			aQuestionData.myInt = minutesPassed;
		}
		return true;
	}
	case eComponentQuestionType::eGetFinishTimeSeconds:
	{
		if (myShouldAddTime == true)
		{
			aQuestionData.myInt = 0;
		}
		else
		{
			float minutesPassed = myElapsedRaceTime / 60.0f;
			minutesPassed = floor(minutesPassed);
			float secondsPassed = myElapsedRaceTime - minutesPassed * 60.0f;
			secondsPassed = floor(secondsPassed);
			aQuestionData.myInt = secondsPassed;
		}
		return true;
	}
	case eComponentQuestionType::eGetFinishTimeHundredthsSeconds:
	{
		if (myShouldAddTime == true)
		{
			aQuestionData.myInt = 0;
		}
		else
		{
			float minutesPassed = myElapsedRaceTime / 60.0f;
			minutesPassed = floor(minutesPassed);
			float secondsPassed = myElapsedRaceTime - minutesPassed * 60.0f;
			float hundrethsSecondsPassed = secondsPassed - floor(secondsPassed);
			hundrethsSecondsPassed *= 100.0f;
			hundrethsSecondsPassed = floor(hundrethsSecondsPassed);
			aQuestionData.myInt = hundrethsSecondsPassed;
		}
		return true;
	}
	break;
	default:
		break;
	}
	return false;
}