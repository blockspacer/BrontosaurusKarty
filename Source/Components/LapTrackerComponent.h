#pragma once
#include "Component.h"
class CLapTrackerComponent : public CComponent
{
public:
	CLapTrackerComponent();
	~CLapTrackerComponent();

	void Update();
	inline const unsigned short GetLapIndex() const;
	inline const unsigned short GetSplineIndex() const;
	const float GetDistanceToNextSpline();
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
private:
	unsigned short myLapIndex;
	unsigned short mySplineIndex;
	bool myIsReadyToEnterGoal;
};

inline const unsigned short CLapTrackerComponent::GetLapIndex() const
{
	return myLapIndex;
}

inline const unsigned short CLapTrackerComponent::GetSplineIndex() const
{
	return mySplineIndex;
}
