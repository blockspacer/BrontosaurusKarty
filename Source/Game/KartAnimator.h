#pragma once

class CAnimationEvent;
class CModelComponent;

class CKartAnimator
{
public:
	CKartAnimator();
	~CKartAnimator();

	void AddAnimation();
	void Update(const float aDeltaTime);

private:
	std::vector<CAnimationEvent> myEventQueue;
};
