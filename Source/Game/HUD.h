#pragma once
#include "GUIElement.h"
#include "../ThreadedPostmaster/Subscriber.h"

class CSpriteInstance;
class CRaceOverMessage;

class KeyCharPressed; //temp

typedef unsigned int TimerHandle;


namespace CU 
{
	class CJsonValue;
}

struct SHUDElement
{
	SHUDElement() { mySprite = nullptr; myShouldRender = true; }

	CSpriteInstance* mySprite;
	SGUIElement myGUIElement;
	CU::Vector2ui myPixelSize;
	bool myShouldRender;
};


class CHUD : public Postmaster::ISubscriber
{
public:
	CHUD(unsigned char aPlayerID, bool aIsOneSplit);
	~CHUD();

	void LoadHUD();
	void Update();
	void Render();

private:
	SHUDElement LoadHUDElement(const CU::CJsonValue& aJsonValue);
	void LoadLapCounter(const CU::CJsonValue& aJsonValue);
	void LoadPlacement(const CU::CJsonValue& aJsonValue);
	void LoadFinishText(const CU::CJsonValue& aJsonValue);
	void LoadItemGui(const CU::CJsonValue& aJsonValue);

	void SetGUIToEmilBlend(std::wstring aStr);
	void SetGUIToEndBlend(std::wstring aStr);

	void AdjustPosBasedOnNrOfPlayers(CU::Vector2f aTopLeft, CU::Vector2f aBotRight);

	void PresentScoreboard();
	void DisableRedundantGUI();

	eMessageReturn DoEvent(const CRaceOverMessage& aMessage) override;
	eMessageReturn DoEvent(const KeyCharPressed& aMessage) override;


private:
	SHUDElement myLapCounterElement;
	SHUDElement myPlacementElement;
	SHUDElement myFinishTextElement;
	SHUDElement myScoreboardElement;
	SHUDElement myItemGuiElement;

	CSpriteInstance* myMushroomSprite;
	CSpriteInstance* myGreenShellSprite;
	CSpriteInstance* myRedShellSprite;
	CSpriteInstance* myBananaSprite;
	CSpriteInstance* myStarSprite;
	CSpriteInstance* myNullSprite;

	CU::Vector2f myCameraOffset; //best solution 10/10
	CU::Vector2f mySpriteOffset;
	CGameObject* myPlayer;
	unsigned char myLapAdjusterCheat;

	unsigned char myPlayerID;
};
