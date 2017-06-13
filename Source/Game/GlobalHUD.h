#pragma once
#include "../ThreadedPostmaster/Subscriber.h"
#include "HUDBase.h"
#include "PlacementData.h"

class CRaceOverMessage;
class CTextInstance;

class CGlobalHUD : public CHUDBase, public Postmaster::ISubscriber
{
public:

	CGlobalHUD(int aLevelIndex);
	~CGlobalHUD();

public:
	void LoadHUD() override;
	void Render() override;
	bool GetRaceOver() const { return myRaceOver; }

	void StartCountDown();

private:
	void LoadCountDown(const CU::CJsonValue& aJsonValue);
	void LoadScoreboard(const CU::CJsonValue& aJsonValue);
	void LoadMiniMap(const CU::CJsonValue& aJsonValue);
	void PresentScoreboard();
	void DisableRedundantGUI();

	eMessageReturn DoEvent(const CRaceOverMessage& aMessage) override;
	eMessageReturn DoEvent(const KeyCharPressed& aMessage) override;
	void ToMainMenu(const std::function<void(void)>& aCallback);

public:
	void Retry();
	void LoadNext();
private:
	const CU::GrowingArray<CGameObject*>* myKartObjects;
	CU::StaticArray<SPlacementData, 8> myWinners;

	SHUDElement myScoreboardElement;
	SHUDElement myMinimapElement;
	SHUDElement myCountdownElement;

	CU::Vector2f myTimeTextOffset;

	CSpriteInstance* myScoreboardBGSprite;
	CSpriteInstance* myPortraitSprite;

	CSpriteInstance* myMinimapBGSprite;
	CSpriteInstance* myMinimapPosIndicator;

	CSpriteInstance* myCountdownSprite;

	CTextInstance* myTimeText;

	const unsigned char myNrOfPlayers;
	bool myRaceOver;
	int myLevelIndex;
};

