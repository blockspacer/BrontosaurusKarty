#pragma once
#include "State.h"

class CSpriteInstance;

class CSplashScreenState : public State
{
public:
	CSplashScreenState(StateStack& aStateStack);
	~CSplashScreenState();

	virtual void Init() override;
	virtual eStateStatus Update(const CU::Time& aDeltaTime) override;
	virtual void Render() override;
	virtual void OnEnter(const bool aLetThroughRender) override;
	virtual void OnExit(const bool aLetThroughRender) override;
	virtual CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;

private:
	void ChooseSprite();

#define SPRITE_COUNT 2
	CU::VectorOnStack<CSpriteInstance*, SPRITE_COUNT> mySprites;
	float myTimer;
	int myCurrentSprite;
};
