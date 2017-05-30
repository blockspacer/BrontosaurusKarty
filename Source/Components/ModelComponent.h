#pragma once
#include "Component.h"

enum class eAnimationState;
class CModelInstance;

class CModelComponent : public CComponent
{
public:
	CModelComponent(CModelInstance& aModel);
	CModelComponent(const CModelComponent& aCopy) = delete;
	~CModelComponent();

	CU::Matrix44f GetToWorldTransform();

	void SetVisibility(const bool aVisibility);
	void FlipVisibility();
	void Receive(const eComponentMessageType, const SComponentMessageData&) override;
	inline CModelInstance& GetModelInstance() { return myModel; }

	void SetAnimation(const eAnimationState aAnimationKey);
	void SetNextAnimation(const eAnimationState aAnimationKey);
	void SetAnimationLerpValue(const float aLerpValue);
	void SetAnimationLooping(const bool aIsLooping);
	void ResetAnimation();
	void SetIgnoreDepth(bool aShouldIgnoreDepth);

	bool GetAnimationStates(CU::GrowingArray<eAnimationState>& aAnimationStatesOut) const;
	const std::string& GetFilePath() const;

	float GetAnimationDuration(const eAnimationState aAnimationState) const;

private:
	CModelInstance& myModel;
};
