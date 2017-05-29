#include "stdafx.h"
#include "HUD.h"

#include "BrontosaurusEngine/SpriteInstance.h"
#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/Renderer.h"

#include "JsonValue.h"
#include "PollingStation.h"
#include "RenderMessages.h"
#include "LapTrackerComponentManager.h"

CHUD::CHUD(unsigned char aPlayerID, bool aIsOneSplit)
{
	myPlayer = CPollingStation::GetInstance()->GetPlayerAtID(aPlayerID);
	if (myPlayer == nullptr)
		DL_ASSERT("HUD - The player retrieved with ID %d was nullptr", aPlayerID);
	myCameraOffset = CU::Vector2f(0.0f, 0.0f);

	float offsetX = 0.516f;
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
}

void CHUD::Update()
{
	
}

void CHUD::Render()
{
	unsigned char currentLap = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerLapIndex(myPlayer);

	if (myLapCounterElement.myHasChanged == true) // används inte atm. om det behövs, fixa någon timer lösnings shizz.
	{

		if(currentLap == 1)
			myLapCounterElement.mySprite->SetRect({ 0.f, 0.5f, 1.f, 0.75f });
		else if (currentLap == 2)
			myLapCounterElement.mySprite->SetRect({ 0.f, 0.25f, 1.f, 0.5f });
		else if(currentLap == 3)
			myLapCounterElement.mySprite->SetRect({ 0.f, 0.f, 1.f, 0.25f });


		SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"lapCounter", myLapCounterElement.myGUIElement, myLapCounterElement.myPixelSize);

		RENDERER.AddRenderMessage(guiElement);
		SetGUIToEmilBlend(L"lapCounter");
		myLapCounterElement.mySprite->RenderToGUI(L"lapCounter");
		SetGUIToEndBlend(L"lapCounter");
	}


	unsigned short playerPlacement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(myPlayer);
	float placementRektValue1 = 1.0f - (1.0f / 8.0f) * playerPlacement;
	float placementRektValue2 = (1.0f + (1.0f / 8.0f)) - (1.0f / 8.0f) * playerPlacement;
	if (myPlacementElement.myHasChanged == true)
	{
		unsigned short playerPlacement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(myPlayer);
		float placementRektValue1 = 1.0f - (1.0f / 8.0f) * playerPlacement;
		float placementRektValue2 = (1.0f + (1.0f / 8.0f)) - (1.0f / 8.0f) * playerPlacement;

		myPlacementElement.mySprite->SetPosition(myCameraOffset);
		myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, placementRektValue1, 1.7f, placementRektValue2));

		SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"placement", myPlacementElement.myGUIElement, myPlacementElement.myPixelSize);

		RENDERER.AddRenderMessage(guiElement);
		SetGUIToEmilBlend(L"placement");
		myPlacementElement.mySprite->RenderToGUI(L"placement");
		SetGUIToEndBlend(L"placement");
	}

	if (myFinishTextElement.myHasChanged == true)
	{
		if (currentLap > 3)
		{
			SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"finishText", myFinishTextElement.myGUIElement, myFinishTextElement.myPixelSize);

			RENDERER.AddRenderMessage(guiElement);
			SetGUIToEmilBlend(L"finishText");
			myFinishTextElement.mySprite->RenderToGUI(L"finishText");
			SetGUIToEndBlend(L"finishText");
		}
	}
}

SHUDElement CHUD::LoadHUDElement(const CU::CJsonValue& aJsonValue)
{
	SHUDElement hudElement;

	hudElement.myGUIElement.myOrigin = { 0.5f,0.5f }; // { 0.5f, 0.5f };
	hudElement.myGUIElement.myAnchor[(char)eAnchors::eTop] = true;
	hudElement.myGUIElement.myAnchor[(char)eAnchors::eLeft] = true;

	hudElement.myGUIElement.myScreenRect = CU::Vector4f(aJsonValue.at("position").GetVector2f());

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

	myLapCounterElement.mySprite = new CSpriteInstance(spritePath.c_str(), { 1.f, 0.25f });
	myLapCounterElement.mySprite->SetRect({ 0.f, 0.5f, 1.f, 0.75f });
}

void CHUD::LoadPlacement(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myPlacementElement = LoadHUDElement(aJsonValue);
	myPlacementElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	myPlacementElement.mySprite = new CSpriteInstance(spritePath.c_str(), { 0.25f,0.06125f });
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

	myFinishTextElement.mySprite = new CSpriteInstance(spritePath.c_str(), { 1.f,1.f });
	myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, 0.f, 1.0f, 1.0f));
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
	//Detta låter som en bra ide! -mig själv.
}
