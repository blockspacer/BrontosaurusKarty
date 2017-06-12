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
	void SetIsShadowCasting(bool aShouldCastShadows);
	inline CModelInstance& GetModelInstance() { return myModel; }

	void SetAnimation(const eAnimationState aAnimationKey);
	void SetNextAnimation(const eAnimationState aAnimationKey);
	void SetAnimationLerpValue(const float aLerpValue);
	void SetAnimationLooping(const bool aIsLooping);
	void SetAnimationManualUpdate(const bool aShouldManuallyUpdate);
	void SetAnimationCounter(const float aValue);
	void ResetAnimation();
	bool GetAnimationStates(CU::GrowingArray<eAnimationState>& aAnimationStatesOut) const;
	const std::string& GetFilePath() const;

	float GetAnimationDuration(const eAnimationState aAnimationState) const;
	void SetIsBillboard(bool aIsBillboard);

private:
	CModelInstance& myModel;
	bool myIsBillboard;
};
