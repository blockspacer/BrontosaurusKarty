#include "stdafx.h"
#include "HUD.h"

#include "BrontosaurusEngine/SpriteInstance.h"
#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/Renderer.h"

#include "JsonValue.h"
#include "PollingStation.h"
#include "RenderMessages.h"
#include "LapTrackerComponentManager.h"
#include "ItemTypes.h"

CHUD::CHUD(unsigned char aPlayerID, bool aIsOneSplit)
{
	myPlayerID = aPlayerID;
	myPlayer = CPollingStation::GetInstance()->GetPlayerAtID(aPlayerID);
	if (myPlayer == nullptr)
		DL_ASSERT("HUD - The player retrieved with ID %d was nullptr", aPlayerID);
	myCameraOffset = CU::Vector2f(0.0f, 0.0f);

	float offsetX = 0.5f;
	float offsetY = 0.5f;

	if (aPlayerID == 1)
	{
		if (aIsOneSplit == true)
		{
			myCameraOffset.y = offsetY;
		}
		else
		{
			myCameraOffset.x = offsetX;
		}
	}
	else if (aPlayerID == 2)
	{
		myCameraOffset.y = offsetY;
	}
	else if (aPlayerID == 3)
	{
		myCameraOffset.x = offsetX;
		myCameraOffset.y = offsetY;
	}
}

CHUD::~CHUD()
{
}

void CHUD::LoadHUD()
{
	CU::CJsonValue jsonDoc;

	jsonDoc.Parse("Json/HUD.json");
	LoadLapCounter(jsonDoc.at("lapCounter"));
	LoadPlacement(jsonDoc.at("placement"));
	LoadFinishText(jsonDoc.at("finishText"));
	LoadItemGui(jsonDoc.at("itemGui"));
}

void CHUD::Update()
{
	
}

void CHUD::Render()
{
	unsigned char currentLap = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerLapIndex(myPlayer);
	unsigned char currentPlacement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(myPlayer);

	if (myLapCounterElement.myHasChanged == true) // anv�nds inte atm. om det beh�vs, fixa n�gon timer l�snings shizz.
	{

		if(currentLap == 1)
			myLapCounterElement.mySprite->SetRect({ 0.f, 0.5f, 1.f, 0.75f });
		else if (currentLap == 2)
			myLapCounterElement.mySprite->SetRect({ 0.f, 0.25f, 1.f, 0.5f });
		else if(currentLap == 3)
			myLapCounterElement.mySprite->SetRect({ 0.f, 0.f, 1.f, 0.25f });


		SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"lapCounter" + myPlayerID, myLapCounterElement.myGUIElement, myLapCounterElement.myPixelSize);

		RENDERER.AddRenderMessage(guiElement);
		SetGUIToEmilBlend(L"lapCounter" + myPlayerID);
		myLapCounterElement.mySprite->RenderToGUI(L"lapCounter" + myPlayerID);
		SetGUIToEndBlend(L"lapCounter" + myPlayerID);
	}

	if (myPlacementElement.myHasChanged == true)
	{
		float placementRektValue1 = 1.0f - (1.0f / 8.0f) * currentPlacement;
		float placementRektValue2 = (1.0f + (1.0f / 8.0f)) - (1.0f / 8.0f) * currentPlacement;

		//myPlacementElement.mySprite->SetPosition(myCameraOffset);
		myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, placementRektValue1, 1.7f, placementRektValue2));

		SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"placement" + myPlayerID, myPlacementElement.myGUIElement, myPlacementElement.myPixelSize);

		RENDERER.AddRenderMessage(guiElement);
		SetGUIToEmilBlend(L"placement" + myPlayerID);
		myPlacementElement.mySprite->RenderToGUI(L"placement" + myPlayerID);
		SetGUIToEndBlend(L"placement" + myPlayerID);
	}

	if (myFinishTextElement.myHasChanged == true)
	{
		if (currentLap > 3)
		{
			SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"finishText" + myPlayerID, myFinishTextElement.myGUIElement, myFinishTextElement.myPixelSize);

			RENDERER.AddRenderMessage(guiElement);
			SetGUIToEmilBlend(L"finishText" + myPlayerID);
			myFinishTextElement.mySprite->RenderToGUI(L"finishText" + myPlayerID);
			SetGUIToEndBlend(L"finishText" + myPlayerID);
		}
	}

	SComponentQuestionData itemQuestionData;
	if(myPlayer->AskComponents(eComponentQuestionType::eGetHoldItemType, itemQuestionData) == true)
	{
		switch (static_cast<eItemTypes>(itemQuestionData.myInt))
		{
		case eItemTypes::eBanana:
		{
			myItemGuiElement.mySprite = myBananaSprite;
			break;
		}
		case eItemTypes::eMushroom:
		{
			myItemGuiElement.mySprite = myMushroomSprite;
			break;
		}
		case eItemTypes::eStar:
		{
			myItemGuiElement.mySprite = myStarSprite;
			break;
		}
		case eItemTypes::eGreenShell:
		{
			myItemGuiElement.mySprite = myGreenShellSprite;
			break;
		}
		case eItemTypes::eRedShell:
		{
			myItemGuiElement.mySprite = myRedShellSprite;
			break;
		}
		default:
			break;
		}
	}
	else
	{
		myItemGuiElement.mySprite = myNullSprite;
	}
	if (myItemGuiElement.myHasChanged == true)
	{
		SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"itemGui" + myPlayerID, myItemGuiElement.myGUIElement, myItemGuiElement.myPixelSize);

		RENDERER.AddRenderMessage(guiElement);
		SetGUIToEmilBlend(L"itemGui" + myPlayerID);
		myItemGuiElement.mySprite->RenderToGUI(L"itemGui" + myPlayerID);
		SetGUIToEndBlend(L"itemGui" + myPlayerID);
	}
}

SHUDElement CHUD::LoadHUDElement(const CU::CJsonValue& aJsonValue)
{
	SHUDElement hudElement;

	hudElement.myGUIElement.myOrigin = { 0.5f,0.5f }; // { 0.5f, 0.5f };
	hudElement.myGUIElement.myAnchor[(char)eAnchors::eTop] = true;
	hudElement.myGUIElement.myAnchor[(char)eAnchors::eLeft] = true;

	hudElement.myGUIElement.myScreenRect = CU::Vector4f(aJsonValue.at("position").GetVector2f()+ myCameraOffset);

	const CU::CJsonValue sizeObject = aJsonValue.at("size");
	hudElement.myPixelSize.x = sizeObject.at("pixelWidth").GetUInt();
	hudElement.myPixelSize.y = sizeObject.at("pixelHeight").GetUInt();

	float rectWidth = sizeObject.at("screenSpaceWidth").GetFloat();
	float rectHeight = sizeObject.at("screenSpaceHeight").GetFloat();

	float topLeftX = hudElement.myGUIElement.myScreenRect.x;
	float topLeftY = hudElement.myGUIElement.myScreenRect.y;

	hudElement.myGUIElement.myScreenRect.z = rectWidth + topLeftX ;
	hudElement.myGUIElement.myScreenRect.w = rectHeight + topLeftY ;

	return hudElement;
}

// Remember - Sprites are bot-left based.

void CHUD::LoadLapCounter(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myLapCounterElement = LoadHUDElement(aJsonValue);
	myLapCounterElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	myLapCounterElement.mySprite = new CSpriteInstance(spritePath.c_str(), { 1.f, 0.25f });
	myLapCounterElement.mySprite->SetRect({ 0.f, 0.5f, 1.f, 0.75f });
}

void CHUD::LoadPlacement(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myPlacementElement = LoadHUDElement(aJsonValue);
	myPlacementElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	myPlacementElement.mySprite = new CSpriteInstance(spritePath.c_str(), { 1.0f,0.4125f });
	myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, 0.875f, 1.0f, 1.0f));
	//myPlacementElement.myGUIElement.myScreenRect.x = myCameraOffset.x;
	//myPlacementElement.myGUIElement.myScreenRect.y = myCameraOffset.y;
	//myPlacementElement.myGUIElement.myScreenRect.z = myCameraOffset.x + 0.2f;
	//myPlacementElement.myGUIElement.myScreenRect.w = myCameraOffset.y + 0.5185f;
}

void CHUD::LoadFinishText(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myFinishTextElement = LoadHUDElement(aJsonValue);
	myFinishTextElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	myFinishTextElement.mySprite = new CSpriteInstance(spritePath.c_str(), { 1.f,1.f });
	/*myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, 0.f, 1.0f, 1.0f));*/

}

void CHUD::LoadItemGui(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myItemGuiElement = LoadHUDElement(aJsonValue);
	myItemGuiElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	float itemGuiWidth = 1.0f;
	float itemGuiHeight = 1.0f;
	myMushroomSprite = new CSpriteInstance("Sprites/GUI/mushroom.dds", { itemGuiWidth,itemGuiHeight });
	myBananaSprite = new CSpriteInstance("Sprites/GUI/banana.dds", { itemGuiWidth,itemGuiHeight });
	myStarSprite = new CSpriteInstance("Sprites/GUI/star.dds", { itemGuiWidth,itemGuiHeight });
	myGreenShellSprite = new CSpriteInstance("Sprites/GUI/greenShell.dds", { itemGuiWidth,itemGuiHeight });
	myRedShellSprite = new CSpriteInstance("Sprites/GUI/redShell.dds", { itemGuiWidth,itemGuiHeight });
	myNullSprite = new CSpriteInstance("Sprites/GUI/redShell.dds", { 0.0f,0.0f });
	myItemGuiElement.mySprite = myMushroomSprite;
	/*myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, 0.f, 1.0f, 1.0f));*/

}

void CHUD::SetGUIToEmilBlend(std::wstring aStr)
{
	SChangeStatesMessage* changeStatesMessage = new SChangeStatesMessage();
	changeStatesMessage->myBlendState = eBlendState::eEmilBlend;
	changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
	changeStatesMessage->mySamplerState = eSamplerState::eClamp;

	SRenderToGUI* guiChangeState = new SRenderToGUI(aStr, changeStatesMessage);
	RENDERER.AddRenderMessage(guiChangeState);
}

void CHUD::SetGUIToEndBlend(std::wstring aStr)
{
	SChangeStatesMessage* changeStatesMessage = new SChangeStatesMessage();
	changeStatesMessage = new SChangeStatesMessage();
	changeStatesMessage->myBlendState = eBlendState::eEndBlend;
	changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
	changeStatesMessage->mySamplerState = eSamplerState::eClamp;

	SRenderToGUI* guiChangeState = new SRenderToGUI(aStr, changeStatesMessage);
	RENDERER.AddRenderMessage(guiChangeState);
}

void CHUD::AdjustPosBasedOnNrOfPlayers(CU::Vector2f aTopLeft, CU::Vector2f aBotRight)
{
	//Detta l�ter som en bra ide! -mig sj�lv.
}
