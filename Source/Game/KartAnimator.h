#pragma once



class CKartAnimator
{
public:
	CKartAnimator();
	~CKartAnimator();
};
//class CAnimationEvent;
//class CModelComponent;
//
//class CKartAnimatorComponent : public CComponent
//{
//public:
//	CKartAnimatorComponent();
//	CKartAnimatorComponent(const CKartAnimatorComponent& aCopy) = delete;
//	~CKartAnimatorComponent();
//
//	static void UpdateAnimations(const float aDeltaTime);
//	static void DeleteAnimations();
//
//private:
//	void Update(const float aDeltaTime);
//
//	std::queue<CAnimationEvent, std::vector<CAnimationEvent>> myEventQueue;
//
//	static CU::GrowingArray<CKartAnimatorComponent*> ourComponents;
//};
