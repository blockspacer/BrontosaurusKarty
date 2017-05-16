#pragma once
#include "../StateStack/State.h"
#include "../ThreadedPostmaster/Subscriber.h"
#include "SParticipant.h"

class StateStack;
class CPlayState;
class CSpriteInstance;

class CLoadState :public State, Postmaster::ISubscriber
{
public:
	CLoadState(StateStack& aStateStack, const int aLevelIndex);
	CLoadState(StateStack& aStateStack, const int aLevelIndex,const CU::GrowingArray<SParticipant> aPlayers);
	~CLoadState();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;

	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;

	eMessageReturn DoEvent(const CServerReadyMessage& aSendNetowrkMessageMessage) override;
private:

	CPlayState* myPlayState;
	//CLoadingAnimation myLoadingAnimation;
	CSpriteInstance* myControlSchemeSprite;
	CSpriteInstance* myTDLogoSprite;
	CU::GrowingArray<SParticipant> myPlayers;

	const int myLevelIndex;
	bool myGotOkFromServer;

	short myNumberOfPlayersToSpawnBeforeStarting;
};

