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

private:
	void LoadScoreboard(const CU::CJsonValue& aJsonValue);
	void LoadMiniMap(const CU::CJsonValue& aJsonValue);
	void PresentScoreboard();
	void DisableRedundantGUI();

	eMessageReturn DoEvent(const CRaceOverMessage& aMessage) override;
	void ToMainMenu();
	eMessageReturn DoEvent(const KeyCharPressed& aMessage) override;

public:
	void Retry();
	eMessageReturn DoEvent(const Postmaster::Message::CControllerInputMessage& aControllerInputMessage) override;
private:

	CU::StaticArray<SPlacementData, 8> myWinners;

	SHUDElement myScoreboardElement;
	SHUDElement myMinimapElement;

	CSpriteInstance* myScoreboardBGSprite;
	CSpriteInstance* myPortraitSprite;

	CSpriteInstance* myMinimapBGSprite;
	CSpriteInstance* myMinimapPosIndicator;

	bool myRaceOver;
};

