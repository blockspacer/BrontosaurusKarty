#pragma once
#include "GUIElement.h"

typedef unsigned int TimerHandle;

class CSpriteInstance;

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

class CHUDBase
{
public:
	CHUDBase();
	virtual ~CHUDBase();

	virtual void LoadHUD() = 0;
	virtual void Render() = 0;

protected:
	virtual SHUDElement LoadHUDElement(const CU::CJsonValue& aJsonValue);
	virtual void LoadHUDElementValues(const CU::CJsonValue& aJsonValue, SHUDElement& aHUDElement, CU::Vector2f aPositionOffset, CU::Vector2f aSizeOffset);

	virtual void SetGUIToAlphaBlend(std::wstring aStr);
	virtual void SetGUIToEndBlend(std::wstring aStr);

protected:
	CU::Vector2f myCameraOffset;
};

