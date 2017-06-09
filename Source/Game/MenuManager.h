#pragma once
#include "../BrontosaurusEngine/SpriteInstance.h"
#include "../BrontosaurusEngine/TextInstance.h"
#include "../CommonUtilities/CommonUtilities.h"
#include "GUIElement.h"
#include <functional>
#include "InputManager.h"

enum class eMenuThingType
{
	eClickArea,
	eText,
	eSprite
};

enum eInputAction
{
	eActionPressed,
	eBackPressed,
	eLeftPressed,
	eRightPressed,
	eStartPressed,
	eSelectPressed,
	eNone,
};


enum class eMenuButtonState
{
	eDefault,
	eOnHover,
	eOnClick,
	eInnactive
};

struct SClickArea
{
	int mySpriteID;
	CU::Vector4f myRect;
	CU::GrowingArray<std::function<bool(void)>> myActions;
};

struct SLayerData
{
	unsigned char myLayer;
	unsigned myIndex;
	eMenuThingType myMenuThing;
};

struct SMenuSprite
{
	SMenuSprite(): myState(eMenuButtonState::eDefault), myDafaultSprite(nullptr), myOnHoverSprite(nullptr), myOnClickSprite(nullptr), myInactiveSprite(nullptr)
	{
	}

	eMenuButtonState myState;
	CSpriteInstance* myDafaultSprite;
	CSpriteInstance* myOnHoverSprite;
	CSpriteInstance* myOnClickSprite;
	CSpriteInstance* myInactiveSprite;

	short myPlayerIndex;
};

class CMenuManager
{
public:
	CMenuManager();
	~CMenuManager();

	void CreateClickArea(CU::GrowingArray<std::string> someActions, CU::GrowingArray<std::string> someArguments, const int aSpriteID, CU::Vector4f aRect, const unsigned char aLayer);

	int CreateSprite(const std::string& aFolder, const CU::Vector2f aPosition, const CU::Vector2f anOrigin, const unsigned char aLayer, const short aListeningToPlayer = -1);
	unsigned CreateText(const std::string& aFontName, const CU::Vector2f& aPosition, const std::wstring someText, const unsigned char aLayer, const eAlignment anAlignment = eAlignment::eLeft);

	void SetMousePointer(CSpriteInstance* aMousePointer);

	void Update(const CU::Time& aDeltaTime);
	void Render();

	void UpdateMousePosition(const CU::Vector2f& aPosition);
	CU::Vector2f GetMopusePosition() const;
	void MousePressed();
	void MouseReleased();

	void LeftPressed(const short aPlayerIndex = 0);
	void RightPressed(const short aPlayerIndex = 0);
	void LeftReleased(const short aPlayerIndex = 0);
	void RightReleased(const short aPlayerIndex = 0);

	void ActionPressed(const short aPlayerIndex = 0);
	void BackButtonPressed(const short aPlayerIndex = 0);

	void RecieveGamePadInput(const CU::GAMEPAD);

	const SMenuSprite& GetSprite(unsigned aSpriteId);

	void AddAction(const std::string& aActionName, const std::function<bool(std::string)>& aFunction);
	CTextInstance* GetTextInstance(const int aTextInputTextInstanceIndex);
private:
	static CSpriteInstance* ChoseSpriteInstance(const SMenuSprite& aMenuSprite);

	std::map<std::string, std::function<bool(std::string)>> myActions;

	CU::GrowingArray<SMenuSprite> mySpriteInstances;
	CU::GrowingArray<CTextInstance*> myTextInstances;
	CU::GrowingArray<SClickArea> myClickAreas;
	CU::GrowingArray<SLayerData> myLayers;


	static CU::Vector2f ourMousePosition;
	CSpriteInstance* myPointerSprite;

	bool myShouldRender;

	SGUIElement myGUIElement;
	int myCurentlyHoveredClickarea;

	short myPlayerThatpressed;

	eInputAction myInput;

	bool myMouseIsPressed;
	bool myIsLeftPressed;
	bool myIsRightPressed;
	bool myHasPlayedHoverSound;
	bool myHasPlayedClickSound;

	CU::GrowingArray<CU::GAMEPAD, int> myUsedGamepadButtons;
	CU::GrowingArray<CU::GrowingArray<std::function<bool(void)>>> myGamepadAction;
};

inline void CMenuManager::UpdateMousePosition(const CU::Vector2f& aPosition)
{
	ourMousePosition = aPosition;

	ourMousePosition.x = CLAMP(ourMousePosition.x, 0, 1);
	ourMousePosition.y = CLAMP(ourMousePosition.y, 0, 1);

	if (myPointerSprite != nullptr)
	{
		myPointerSprite->SetPosition(ourMousePosition);
	}
}

inline CU::Vector2f CMenuManager::GetMopusePosition() const
{
	return ourMousePosition;
}
