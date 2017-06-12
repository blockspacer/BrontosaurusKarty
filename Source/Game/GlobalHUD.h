#pragma once
#include "../ThreadedPostmaster/Subscriber.h"
#include "HUDBase.h"
#include "PlacementData.h"

class CRaceOverMessage;

class CGlobalHUD : public CHUDBase, public Postmaster::ISubscriber
{
public:

	CGlobalHUD();
	~CGlobalHUD();

public:
	void LoadHUD() override;
	void Render() override;

	void StartCountDown();

private:
	void LoadCountDown(const CU::CJsonValue& aJsonValue);
	void LoadScoreboard(const CU::CJsonValue& aJsonValue);
	void LoadMiniMap(const CU::CJsonValue& aJsonValue);
	void PresentScoreboard();
	void DisableRedundantGUI();

	eMessageReturn DoEvent(const CRaceOverMessage& aMessage) override;
	eMessageReturn DoEvent(const KeyCharPressed& aMessage) override;
	void ToMainMenu();

public:
	void Retry();
	eMessageReturn DoEvent(const Postmaster::Message::CControllerInputMessage& aControllerInputMessage) override;
private:
	const CU::GrowingArray<CGameObject*>* myKartObjects;
	CU::StaticArray<SPlacementData, 8> myWinners;

	SHUDElement myScoreboardElement;
	SHUDElement myMinimapElement;
	SHUDElement myCountdownElement;

	CSpriteInstance* myScoreboardBGSprite;
	CSpriteInstance* myPortraitSprite;

	CSpriteInstance* myMinimapBGSprite;
	CSpriteInstance* myMinimapPosIndicator;

	CSpriteInstance* myCountdownSprite;

	const unsigned char myNrOfPlayers;
	bool myRaceOver;
};

