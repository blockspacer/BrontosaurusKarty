#pragma once
#include "Component.h"
class CTimeTrackerComponent : public CComponent
{
public:
	CTimeTrackerComponent();
	~CTimeTrackerComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Update(float aDeltaTime);
	inline void StartAddingTime();
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
private:
	float myElapsedRaceTime;

	bool myShouldAddTime;
};


inline void CTimeTrackerComponent::StartAddingTime()
{
	myShouldAddTime = true;
}
