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
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	inline const unsigned short GetPlacementValue() const;
	void Init();
	const float GetTotalTravelledDistance();
	const float GetLapDistanceTravelledPercentage();
private:
	const float GetDistanceToNextSpline();
private:
	float myTravelledDistance;
	float myLapsTotalDistance;
	unsigned short myLapIndex;
	unsigned short mySplineIndex;
	unsigned short myPlacementValue;
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

inline const unsigned short CLapTrackerComponent::GetPlacementValue() const
{
	return myPlacementValue;
}