#pragma once
#include "GUIElement.h"

class CSpriteInstance;

namespace CU 
{
	class CJsonValue;
}

struct SHUDElement
{
	SHUDElement() { mySprite = nullptr; myHasChanged = true; }

	CSpriteInstance* mySprite;
	SGUIElement myGUIElement;
	CU::Vector2ui myPixelSize;
	bool myHasChanged;

	const CU::Vector2f& GetPos() { return *myPosition; }

private:
	const CU::Vector2f* myPosition = &myGUIElement.myScreenRect.xy;
};


class CHUD
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
	void SetGUIToEmilBlend(std::wstring aStr);
	void SetGUIToEndBlend(std::wstring aStr);
private:
	SHUDElement myLapCounterElement;
	SHUDElement myPlacementElement;

	CU::Vector2f myCameraOffset; //best solution 10/10
	CU::Vector2f mySpriteOffset;
	CGameObject* myPlayer;
};

