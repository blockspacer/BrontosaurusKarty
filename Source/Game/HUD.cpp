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

#include <TimerManager.h>
#include <ThreadPool.h>

#include "..\ThreadedPostmaster\Postmaster.h"
#include "..\ThreadedPostmaster\PostOffice.h"

#include "..\ThreadedPostmaster\MessageType.h"
#include "..\ThreadedPostmaster\RaceOverMessage.h"
#include "..\ThreadedPostmaster\KeyCharPressed.h"


CHUD::CHUD(unsigned char aPlayerID, unsigned short aAmountOfPlayers)
{
	myPlayerID = aPlayerID;
	myAmountOfPlayers = aAmountOfPlayers;

	myPlayer = CPollingStation::GetInstance()->GetPlayerAtID(aPlayerID);
	if (myPlayer == nullptr)
		DL_ASSERT("HUD - The player retrieved with ID %d was nullptr", aPlayerID);

	//POSTMASTER.Subscribe(this, eMessageType::eCharPressed);
	//POSTMASTER.Subscribe(this, eMessageType::eRaceOver); // why is this crapper?

	myCameraOffset = CU::Vector2f(0.0f, 0.0f);

	myLapAdjusterCheat = 0;

	float offsetX = 0.5f;
	float offsetY = 0.5f;

	if (aPlayerID == 1)
	{
		if (myAmountOfPlayers == 2)
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
	POSTMASTER.Unsubscribe(this);
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
	unsigned char currentLap = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerLapIndex(myPlayer) + myLapAdjusterCheat;
	unsigned char currentPlacement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(myPlayer);

	if (myLapCounterElement.myShouldRender == true)
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

	if (myPlacementElement.myShouldRender == true)
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

	if (myFinishTextElement.myShouldRender == true)
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
		SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"itemGui" + myPlayerID, myItemGuiElement.myGUIElement, myItemGuiElement.myPixelSize);

	RENDERER.AddRenderMessage(guiElement);
	SetGUIToEmilBlend(L"itemGui" + myPlayerID);
	myItemGuiElement.mySprite->RenderToGUI(L"itemGui" + myPlayerID);
	SetGUIToEndBlend(L"itemGui" + myPlayerID);
}

SHUDElement CHUD::LoadHUDElement(const CU::CJsonValue& aJsonValue, eGuiType aGuiType)
{
	SHUDElement hudElement;

	hudElement.myGUIElement.myOrigin = { 0.5f,0.5f }; // { 0.5f, 0.5f };
	hudElement.myGUIElement.myAnchor[(char)eAnchors::eTop] = true;
	hudElement.myGUIElement.myAnchor[(char)eAnchors::eLeft] = true;

	if(myAmountOfPlayers == 1 && aGuiType == eGuiType::ePlacement)
	{
		LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(0.0f, 0.40f), CU::Vector2f(0.09f, 0.08f));
	}
	else if (myAmountOfPlayers == 1 && aGuiType == eGuiType::eLapCounter)
	{
		LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(0.0f, 0.48f), CU::Vector2f());
	}
	else if (myAmountOfPlayers == 1 && aGuiType == eGuiType::eFinish)
	{
		LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(0.25f, 0.25f), CU::Vector2f());
	}
	else if (myAmountOfPlayers == 2 && aGuiType == eGuiType::ePlacement)
	{
		LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(0.0f, 0.0f), CU::Vector2f(0.09f, 0.08f));
	}
	else if (myAmountOfPlayers == 2 && aGuiType == eGuiType::eLapCounter && myPlayerID == 0)
	{
		LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(0.9f, -0.38f), CU::Vector2f());
	}
	else if (myAmountOfPlayers == 2 && aGuiType == eGuiType::eLapCounter && myPlayerID == 1)
	{
		LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(0.9f, 0.02f), CU::Vector2f());
	}
	else if (myAmountOfPlayers == 2 && aGuiType == eGuiType::eFinish)
	{
		LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(0.25f, 0.0f), CU::Vector2f());
	}
	else if (myAmountOfPlayers >= 3 && aGuiType == eGuiType::eLapCounter && myPlayerID % 2 == 1)
	{
		LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(0.4f, 0.02f), CU::Vector2f());
	}
	else if (myAmountOfPlayers >= 3 && aGuiType == eGuiType::ePlacement && myPlayerID % 2 == 1)
	{
		LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(0.38f, 0.02f), CU::Vector2f());
	}
	else
	{
		LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(), CU::Vector2f());
	}
	

	return hudElement;
}

void CHUD::LoadHUDElementValues(const CU::CJsonValue& aJsonValue, SHUDElement& aHUDElement, CU::Vector2f aPositionOffset, CU::Vector2f aSizeOffset)
{
	aHUDElement.myGUIElement.myScreenRect = CU::Vector4f(aJsonValue.at("position").GetVector2f() + myCameraOffset + aPositionOffset);

	const CU::CJsonValue sizeObject = aJsonValue.at("size");
	aHUDElement.myPixelSize.x = sizeObject.at("pixelWidth").GetUInt();
	aHUDElement.myPixelSize.y = sizeObject.at("pixelHeight").GetUInt();

	float rectWidth = sizeObject.at("screenSpaceWidth").GetFloat() + aSizeOffset.x;
	float rectHeight = sizeObject.at("screenSpaceHeight").GetFloat() + aSizeOffset.y;

	float topLeftX = aHUDElement.myGUIElement.myScreenRect.x;
	float topLeftY = aHUDElement.myGUIElement.myScreenRect.y;

	aHUDElement.myGUIElement.myScreenRect.z = rectWidth + topLeftX;
	aHUDElement.myGUIElement.myScreenRect.w = rectHeight + topLeftY;

}
// Remember - Sprites are bot-left based.

void CHUD::LoadLapCounter(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myLapCounterElement = LoadHUDElement(aJsonValue, eGuiType::eLapCounter);
	myLapCounterElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	myLapCounterElement.mySprite = new CSpriteInstance(spritePath.c_str(), { 0.8f, 0.35f });
	myLapCounterElement.mySprite->SetRect({ 0.f, 0.5f, 1.f, 0.75f });
}

void CHUD::LoadPlacement(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myPlacementElement = LoadHUDElement(aJsonValue, eGuiType::ePlacement);
	myPlacementElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	CU::Vector2f spriteSize(1.0f, 1.0f);
	myPlacementElement.mySprite = new CSpriteInstance(spritePath.c_str(), spriteSize);
	myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, 0.875f, 1.0f, 1.0f));

}

void CHUD::LoadFinishText(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myFinishTextElement = LoadHUDElement(aJsonValue, eGuiType::eFinish);
	myFinishTextElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	myFinishTextElement.mySprite = new CSpriteInstance(spritePath.c_str(), { 1.f,1.f });
	/*myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, 0.f, 1.0f, 1.0f));*/

}

void CHUD::LoadItemGui(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myItemGuiElement = LoadHUDElement(aJsonValue, eGuiType::eItem);
	myItemGuiElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	float itemGuiWidth = 1.0f;
	float itemGuiHeight = 1.1f;
	myMushroomSprite = new CSpriteInstance("Sprites/GUI/mushroom.dds", { itemGuiWidth,itemGuiHeight });
	myBananaSprite = new CSpriteInstance("Sprites/GUI/banana.dds", { itemGuiWidth,itemGuiHeight });
	myStarSprite = new CSpriteInstance("Sprites/GUI/star.dds", { itemGuiWidth,itemGuiHeight });
	myGreenShellSprite = new CSpriteInstance("Sprites/GUI/greenShell.dds", { itemGuiWidth,itemGuiHeight });
	myRedShellSprite = new CSpriteInstance("Sprites/GUI/redShell.dds", { itemGuiWidth,itemGuiHeight });
	myNullSprite = new CSpriteInstance("Sprites/GUI/redShell.dds", { 0.0f,0.0f });
	myItemGuiElement.mySprite = myNullSprite;
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
	//Detta låter som en bra ide! -mig själv.
}

void CHUD::PresentScoreboard()
{
	DisableRedundantGUI();

	myScoreboardElement.myShouldRender = true;
}

void CHUD::DisableRedundantGUI()
{
	auto lambda = [this]()
	{
		CU::TimerManager timerManager;
		TimerHandle timer = timerManager.CreateTimer();

		while (timerManager.GetTimer(timer).GetLifeTime().GetSeconds() < .05f)
		{
			myLapCounterElement.mySprite->SetAlpha(0);
			myPlacementElement.mySprite->SetAlpha(0);
			myItemGuiElement.mySprite->SetAlpha(0);
		}

		myLapCounterElement.myShouldRender = false;
		myPlacementElement.myShouldRender = false;
		myItemGuiElement.myShouldRender = false;
	};

	CU::Work work(lambda);
	work.SetName("disableRedundantGUI");
	CU::ThreadPool::GetInstance()->AddWork(work);
}

// When the race is finished for all players.
eMessageReturn CHUD::DoEvent(const CRaceOverMessage& aMessage)
{
	aMessage.GetWinnerPlacements();
	// Present scoreboard. (over the entire screen.)
	PresentScoreboard();
	return eMessageReturn::eContinue;
}

// Debuging
eMessageReturn CHUD::DoEvent(const KeyCharPressed& aMessage)
{
	if (aMessage.GetKey() == 'p')
		myLapAdjusterCheat += 1;

	unsigned char currentLap = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerLapIndex(myPlayer) + myLapAdjusterCheat;
	if (currentLap > 3)
		PresentScoreboard();

	return eMessageReturn::eContinue;
}
