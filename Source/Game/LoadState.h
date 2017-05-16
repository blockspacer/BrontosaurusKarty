#pragma once
#include "../StateStack/State.h"
#include "../ThreadedPostmaster/Subscriber.h"

class StateStack;
class CPlayState;
class CSpriteInstance;

class CLoadState :public State, Postmaster::ISubscriber
{
public:
	CLoadState(StateStack& aStateStack, const int aLevelIndex);
	~CLoadState();

	virtual void Init() override;
	virtual eStateStatus Update(const CU::Time& aDeltaTime) override;
	virtual void Render() override;

	virtual void OnEnter(const bool aLetThroughRender) override;
	virtual void OnExit(const bool aLetThroughRender) override;

private:
	CPlayState* myPlayState;
	//CLoadingAnimation myLoadingAnimation;
	CSpriteInstance* myControlSchemeSprite;
	CSpriteInstance* myTDLogoSprite;

	const int myLevelIndex;
};

