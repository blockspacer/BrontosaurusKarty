#pragma once
#include "../ThreadedPostmaster/Subscriber.h"
#include "HUDBase.h"

class KeyCharPressed; //temp
class CBlueShellWarningMessage;

class CLocalHUD : public CHUDBase, public Postmaster::ISubscriber
{
public:
	CLocalHUD(unsigned char aPlayerID, unsigned short aAmountOfPlayers);
	~CLocalHUD();

	void LoadHUD() override;
	void Render() override;
	void Update(const float aDeltatTime);

private:
	void LoadLapCounter(const CU::CJsonValue& aJsonValue);
	void LoadPlacement(const CU::CJsonValue& aJsonValue);
	void LoadFinishText(const CU::CJsonValue& aJsonValue);
	void LoadItemGui(const CU::CJsonValue& aJsonValue);
	void LoadDangerGui(const CU::CJsonValue& aJsonValue);

	void DisableRedundantGUI();

	eMessageReturn DoEvent(const KeyCharPressed& aMessage) override;
	eMessageReturn DoEvent(const CBlueShellWarningMessage& aMessage) override;
	eMessageReturn DoEvent(const CRedShellWarningMessage& aMessage) override;
	eMessageReturn DoEvent(const CRaceOverMessage& aMessage) override;

private:
	SHUDElement myLapCounterElement;
	SHUDElement myPlacementElement;
	SHUDElement myFinishTextElement;
	SHUDElement myItemGuiElement;
	SHUDElement myDangerGuiElement;

	CSpriteInstance* myMushroomSprite;
	CSpriteInstance* myGreenShellSprite;
	CSpriteInstance* myRedShellSprite;
	CSpriteInstance* myBananaSprite;
	CSpriteInstance* myStarSprite;
	CSpriteInstance* myLightningSprite;
	CSpriteInstance* myBlueShellSprite;
	CSpriteInstance* myFakeItemBoxSprite;

	CSpriteInstance* myBlueShellDangerSprite;
	CSpriteInstance* myRedShellDangerSprite;
	
	CSpriteInstance* myNullSprite;

	CGameObject* myPlayer;

	float myIsDangerVisibleCountdown;
	unsigned short myAmountOfPlayers;
	unsigned char myLapAdjusterCheat;
	unsigned char myPlayerID;

	bool myShouldRenderLocalHUD;
};
