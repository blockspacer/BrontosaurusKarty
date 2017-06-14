#pragma once
#include "State.h"

class CSpriteInstance;

class CCreditsState : public State
{
public:
	CCreditsState(StateStack& aStateStack);
	~CCreditsState();

	virtual void Init() override;
	virtual eStateStatus Update(const CU::Time& aDeltaTime) override;
	virtual void Render() override;
	virtual void OnEnter(const bool aLetThroughRender) override;
	virtual void OnExit(const bool aLetThroughRender) override;

	virtual CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;

private:
	std::unique_ptr<CSpriteInstance> myCreditsSprite;
};

