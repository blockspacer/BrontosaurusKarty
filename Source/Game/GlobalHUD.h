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
	void LoadScoreboard();
	void LoadMiniMap();
	void PresentScoreboard();
	void DisableRedundantGUI();

	eMessageReturn DoEvent(const CRaceOverMessage& aMessage) override;
	eMessageReturn DoEvent(const KeyCharPressed& aMessage) override;

private:

	CU::StaticArray<SPlacementData, 8> myWinners;
	CU::GrowingArray<SHUDElement*> myPlacementLinesGUIElement;

	SHUDElement myScoreboardElement;

	CSpriteInstance* myScoreboardBGSprite;
	CSpriteInstance* myPortraitSprite;
};

