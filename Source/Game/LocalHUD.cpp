#include "stdafx.h"
#include "LocalHUD.h"

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
#include "..\ThreadedPostmaster\RaceOverMessage.h"

//Debug
#include "..\ThreadedPostmaster\KeyCharPressed.h"
#include "..\ThreadedPostmaster\PlayerFinishedMessage.h"
#include "..\ThreadedPostmaster\RaceStartedMessage.h"
#include "../ThreadedPostmaster/BlueShellWarningMessage.h"

#include "TextInstance.h"


CLocalHUD::CLocalHUD(unsigned char aPlayerID, unsigned short aAmountOfPlayers)
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

CLocalHUD::~CLocalHUD()
{
	POSTMASTER.Unsubscribe(this);
}

void CLocalHUD::LoadHUD()
{
	CU::CJsonValue jsonDoc;

	if(myAmountOfPlayers == 1)
	{
		jsonDoc.Parse("Json/HUD/HUD1Player.json");
	}
	else if (myAmountOfPlayers == 2)
	{
		jsonDoc.Parse("Json/HUD/HUD2Player.json");
	}
	else if (myAmountOfPlayers == 3)
	{
		jsonDoc.Parse("Json/HUD/HUD3Player.json");
	}
	else if (myAmountOfPlayers == 4)
	{
		jsonDoc.Parse("Json/HUD/HUD4Player.json");		
	}

	CU::CJsonValue jsonPlayerObject;

	if(myPlayerID == 0)
	{
		jsonPlayerObject = jsonDoc.at("1stPlayer");
	}
	else if (myPlayerID == 1)
	{
		jsonPlayerObject = jsonDoc.at("2ndPlayer");
	}
	else if (myPlayerID == 2)
	{
		jsonPlayerObject = jsonDoc.at("3rdPlayer");
	}
	else if (myPlayerID == 3)
	{
		jsonPlayerObject = jsonDoc.at("4thPlayer");
	}

	LoadLapCounter(jsonPlayerObject.at("lapCounter"));
	LoadPlacement(jsonPlayerObject.at("placement"));
	LoadFinishText(jsonPlayerObject.at("finishText"));
	LoadItemGui(jsonPlayerObject.at("itemGui"));
	LoadDangerGui(jsonPlayerObject.at("dangerGui"));
	
}

void CLocalHUD::Render()
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
		SetGUIToAlphaBlend(L"lapCounter" + myPlayerID);
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
		SetGUIToAlphaBlend(L"placement" + myPlayerID);
		myPlacementElement.mySprite->RenderToGUI(L"placement" + myPlayerID);
		SetGUIToEndBlend(L"placement" + myPlayerID);
	}

	if (myFinishTextElement.myShouldRender == true)
	{
		if (currentLap > 3)
		{
			myItemGuiElement.mySprite = myNullSprite;
			myItemGuiElement.myShouldRender = false;
			myLapCounterElement.mySprite = myNullSprite;
			myPlacementElement.mySprite = myNullSprite;

			SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"finishText" + myPlayerID, myFinishTextElement.myGUIElement, myFinishTextElement.myPixelSize);

			RENDERER.AddRenderMessage(guiElement);
			SetGUIToAlphaBlend(L"finishText" + myPlayerID);
			myFinishTextElement.mySprite->RenderToGUI(L"finishText" + myPlayerID);
			SetGUIToEndBlend(L"finishText" + myPlayerID);
		}
	}
	if (myDangerGuiElement.myShouldRender == false)
	{
		myDangerGuiElement.mySprite = myNullSprite;
	}

	SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"dangerGui" + myPlayerID, myDangerGuiElement.myGUIElement, myDangerGuiElement.myPixelSize);

	RENDERER.AddRenderMessage(guiElement);
	SetGUIToAlphaBlend(L"dangerGui" + myPlayerID);
	myDangerGuiElement.mySprite->RenderToGUI(L"dangerGui" + myPlayerID);
	SetGUIToEndBlend(L"dangerGui" + myPlayerID);

	myDangerGuiElement.myShouldRender = false;


	if(myItemGuiElement.myShouldRender == true)
	{
		SComponentQuestionData itemQuestionData;
		if (myPlayer->AskComponents(eComponentQuestionType::eGetHoldItemType, itemQuestionData) == true)
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
			case eItemTypes::eLightning:
			{
				myItemGuiElement.mySprite = myLightningSprite;
				break;
			}
			case eItemTypes::eBlueShell:
			{
				myItemGuiElement.mySprite = myBlueShellSprite;
				
				break;
			}
			case eItemTypes::eFakeItemBox:
			{
				myItemGuiElement.mySprite = myFakeItemBoxSprite;
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
		SetGUIToAlphaBlend(L"itemGui" + myPlayerID);
		myItemGuiElement.mySprite->RenderToGUI(L"itemGui" + myPlayerID);
		SetGUIToEndBlend(L"itemGui" + myPlayerID);
	}
}

void CLocalHUD::LoadLapCounter(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myLapCounterElement = LoadHUDElement(aJsonValue);
	myLapCounterElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	myLapCounterElement.mySprite = new CSpriteInstance(spritePath.c_str(), { 0.8f, 0.35f });
	myLapCounterElement.mySprite->SetRect({ 0.f, 0.5f, 1.f, 0.75f });
}

void CLocalHUD::LoadPlacement(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myPlacementElement = LoadHUDElement(aJsonValue);
//	myPlacementElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	CU::Vector2f spriteSize(1.0f, 1.0f);
	myPlacementElement.mySprite = new CSpriteInstance(spritePath.c_str(), spriteSize);
	myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, 0.875f, 1.0f, 1.0f));

}

void CLocalHUD::LoadFinishText(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myFinishTextElement = LoadHUDElement(aJsonValue);
//	myFinishTextElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	myFinishTextElement.mySprite = new CSpriteInstance(spritePath.c_str(), { 1.f,1.f });
	/*myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, 0.f, 1.0f, 1.0f));*/
}

void CLocalHUD::LoadItemGui(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myItemGuiElement = LoadHUDElement(aJsonValue);
	//myItemGuiElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	float itemGuiWidth = 1.0f;
	float itemGuiHeight = 1.0f;
	myMushroomSprite = new CSpriteInstance("Sprites/GUI/Items/mushroom.dds", { itemGuiWidth,itemGuiHeight });
	myBananaSprite = new CSpriteInstance("Sprites/GUI/Items/banana.dds", { itemGuiWidth,itemGuiHeight });
	myStarSprite = new CSpriteInstance("Sprites/GUI/Items/star.dds", { itemGuiWidth,itemGuiHeight });
	myGreenShellSprite = new CSpriteInstance("Sprites/GUI/Items/greenShell.dds", { itemGuiWidth,itemGuiHeight });
	myRedShellSprite = new CSpriteInstance("Sprites/GUI/Items/redShell.dds", { itemGuiWidth,itemGuiHeight });
	myLightningSprite = new CSpriteInstance("Sprites/GUI/Items/lightning.dds", { itemGuiWidth,itemGuiHeight });
	myBlueShellSprite = new CSpriteInstance("Sprites/GUI/Items/blueShell.dds", { itemGuiWidth,itemGuiHeight });
	myFakeItemBoxSprite = new CSpriteInstance("Sprites/GUI/Items/fakeItemBox.dds", { itemGuiWidth,itemGuiHeight });
	myNullSprite = new CSpriteInstance("Sprites/GUI/Items/redShell.dds", { 0.0f,0.0f });
	myItemGuiElement.mySprite = myNullSprite;
	/*myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, 0.f, 1.0f, 1.0f));*/

}

void CLocalHUD::LoadDangerGui(const CU::CJsonValue& aJsonValue)
{
	const std::string spritePath = aJsonValue.at("spritePath").GetString();

	myDangerGuiElement = LoadHUDElement(aJsonValue);
	//myItemGuiElement.myGUIElement.myOrigin = CU::Vector2f(0.0f, 0.0f);

	float itemGuiWidth = 1.0f;
	float itemGuiHeight = 1.0f;
	myBlueShellDangerSprite = new CSpriteInstance("Sprites/GUI/blueShellWarning.dds", { itemGuiWidth,itemGuiHeight });
	myDangerGuiElement.mySprite = myNullSprite;
	/*myPlacementElement.mySprite->SetRect(CU::Vector4f(0.0f, 0.f, 1.0f, 1.0f));*/

}

void CLocalHUD::DisableRedundantGUI()
{
	auto lambda = [this]()
	{
		CU::TimerManager timerManager;
		TimerHandle timer = timerManager.CreateTimer();

		while (timerManager.GetTimer(timer).GetLifeTime().GetSeconds() < .05f)
		{
			timerManager.UpdateTimers();
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
//eMessageReturn CLocalHUD::DoEvent(const CRaceOverMessage& aMessage)
//{
//	myWinners = aMessage.GetWinners();
//	// Present scoreboard. (over the entire screen.)
//	PresentScoreboard();
//	return eMessageReturn::eContinue;
//}

// Debuging
eMessageReturn CLocalHUD::DoEvent(const KeyCharPressed& aMessage)
{
	if (aMessage.GetKey() == 'p')
		myLapAdjusterCheat += 1;

	if (aMessage.GetKey() == 'l')
		POSTMASTER.Broadcast(new CPlayerFinishedMessage(myPlayer));


	unsigned char currentLap = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerLapIndex(myPlayer) + myLapAdjusterCheat;
	if (currentLap > 3)
		//PresentScoreboard();

	return eMessageReturn::eContinue;
}


eMessageReturn CLocalHUD::DoEvent(const CBlueShellWarningMessage& aMessage)
{
	if(aMessage.GetKartToWarn() == myPlayer)
	{
		myDangerGuiElement.mySprite = myBlueShellDangerSprite;
		myDangerGuiElement.myShouldRender = true;
	}
	return eMessageReturn::eContinue;
}