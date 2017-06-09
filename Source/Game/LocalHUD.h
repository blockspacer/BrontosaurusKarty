#pragma once
#include "../ThreadedPostmaster/Subscriber.h"
#include "HUDBase.h"

class CRaceOverMessage;
class KeyCharPressed; //temp

class CLocalHUD : public CHUDBase, public Postmaster::ISubscriber
{
public:
	CLocalHUD(unsigned char aPlayerID, unsigned short aAmountOfPlayers);
	~CLocalHUD();

	void LoadHUD() override;
	void Render() override;

private:
	void LoadLapCounter(const CU::CJsonValue& aJsonValue);
	void LoadPlacement(const CU::CJsonValue& aJsonValue);
	void LoadFinishText(const CU::CJsonValue& aJsonValue);
	void LoadItemGui(const CU::CJsonValue& aJsonValue);

	void DisableRedundantGUI();

	//eMessageReturn DoEvent(const CRaceOverMessage& aMessage) override;
	eMessageReturn DoEvent(const KeyCharPressed& aMessage) override;


private:
	SHUDElement myLapCounterElement;
	SHUDElement myPlacementElement;
	SHUDElement myFinishTextElement;
	SHUDElement myItemGuiElement;


	CSpriteInstance* myMushroomSprite;
	CSpriteInstance* myGreenShellSprite;
	CSpriteInstance* myRedShellSprite;
	CSpriteInstance* myBananaSprite;
	CSpriteInstance* myStarSprite;
	CSpriteInstance* myLightningSprite;
	CSpriteInstance* myBlueShellSprite;
	CSpriteInstance* myFakeItemBoxSprite;
	CSpriteInstance* myNullSprite;

	CGameObject* myPlayer;

	unsigned short myAmountOfPlayers;
	unsigned char myLapAdjusterCheat;
	unsigned char myPlayerID;
};
