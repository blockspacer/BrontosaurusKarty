#pragma once
#include "../ThreadedPostmaster/Subscriber.h"
#include "HUDBase.h"
#include "PlacementData.h"

class CRaceOverMessage;

class CGlobalHUD : public CHUDBase, public Postmaster::ISubscriber
{
public:

	CGlobalHUD(int aLevelIndex);
	~CGlobalHUD();

public:
	void LoadHUD() override;
	void Render() override;
	bool GetRaceOVer() const;

private:
	void LoadScoreboard(const CU::CJsonValue& aJsonValue);
	void LoadMiniMap(const CU::CJsonValue& aJsonValue);
	void PresentScoreboard();
	void DisableRedundantGUI();

	eMessageReturn DoEvent(const CRaceOverMessage& aMessage) override;
	void ToMainMenu(const std::function<void(void)>& aCallback);
	eMessageReturn DoEvent(const KeyCharPressed& aMessage) override;

public:
	void Retry();
	void LoadNext();
private:
	const CU::GrowingArray<CGameObject*>* myKartObjects;
	CU::StaticArray<SPlacementData, 8> myWinners;

	SHUDElement myScoreboardElement;
	SHUDElement myMinimapElement;

	CSpriteInstance* myScoreboardBGSprite;
	CSpriteInstance* myPortraitSprite;

	CSpriteInstance* myMinimapBGSprite;
	CSpriteInstance* myMinimapPosIndicator;

	const unsigned char myNrOfPlayers;
	bool myRaceOver;
	int myLevelIndex;
};

