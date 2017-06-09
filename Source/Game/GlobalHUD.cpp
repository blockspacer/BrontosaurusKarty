#include "stdafx.h"
#include "GlobalHUD.h"

#include "Engine.h"
#include "Renderer.h"
#include "RenderMessages.h"
#include "SpriteInstance.h"
#include "TextInstance.h"
#include "JsonValue.h"

#include "SParticipant.h"
#include "..\ThreadedPostmaster\RaceOverMessage.h"
#include "..\ThreadedPostmaster\KeyCharPressed.h"

#include <ThreadPool.h>

CGlobalHUD::CGlobalHUD()
{

}

CGlobalHUD::~CGlobalHUD()
{
}

void CGlobalHUD::LoadHUD()
{
	LoadMiniMap();
	LoadScoreboard();
}

void CGlobalHUD::Render()
{

	if (myScoreboardElement.myShouldRender == true)
	{
		SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"scoreboard", myScoreboardElement.myGUIElement, myScoreboardElement.myPixelSize);
		RENDERER.AddRenderMessage(guiElement);

		SetGUIToAlphaBlend(L"scoreboard");
		{
			float yOffset = 0.11333f; //shh..

			CU::Vector2f initialPortraitPos = myPortraitSprite->GetPosition();

			myScoreboardElement.mySprite->RenderToGUI(L"scoreboard");

			for (int i = 0; i < 8; ++i)
			{
				CTextInstance charNameTxt;

				switch (myWinners[i].character)
				{
				case SParticipant::eCharacter::eVanBrat:
					myPortraitSprite->SetRect({ 0.f, 0.875f, 1.f, 1.f });
					charNameTxt.SetText(L"Yoshi");
					break;
				case SParticipant::eCharacter::eGrandMa:
					myPortraitSprite->SetRect({ 0.f, 0.750f, 1.f, 0.875f });
					charNameTxt.SetText(L"Mario");
					break;
				default:
					//myPortraitSprite->SetRect({ 0.f, 0.875f, 1.f, 1.f });
					myPortraitSprite->SetRect({ 0.f, 0.750f, 1.f, 0.875f });
					charNameTxt.SetText(L"Error");
					break;
				}

				CU::Vector2f lastPortraitPos = myPortraitSprite->GetPosition();
				CU::Vector2f newPortraitPos = { lastPortraitPos.x, lastPortraitPos.y + yOffset };

				myPortraitSprite->RenderToGUI(L"scoreboard");
				myPortraitSprite->SetPosition(newPortraitPos);
			}

			myPortraitSprite->SetPosition(initialPortraitPos);

		}
		SetGUIToEndBlend(L"scoreboard");
	}

	//for (unsigned int i = 0; i < myPlacementLinesGUIElement.Size(); i++)
	//{
	//	SComponentQuestionData lapTraversedPercentageQuestionData;
	//	if (myKartObjects[i]->AskComponents(eComponentQuestionType::eGetLapTraversedPercentage, lapTraversedPercentageQuestionData) == true)
	//	{
	//	float lapTraversedPlacement = lapTraversedPercentageQuestionData.myFloat;
	//	myPlacementLinesGUIElement[i]->myGUIElement.myScreenRect.x = lapTraversedPlacement;
	//	myPlacementLinesGUIElement[i]->myGUIElement.myScreenRect.z = myPlacementLineScreenSpaceWidth + lapTraversedPlacement;
	//	}
	//
	//	SCreateOrClearGuiElement* createOrClear = new SCreateOrClearGuiElement(L"placementLine" + i, myPlacementLinesGUIElement[i]->myGUIElement, CU::Vector2ui(WINDOW_SIZE.x, WINDOW_SIZE.y));
	//	RENDERER.AddRenderMessage(createOrClear);
	//
	//	SChangeStatesMessage* const changeStatesMessage = new SChangeStatesMessage();
	//	changeStatesMessage->myBlendState = eBlendState::eAlphaBlend;
	//	changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
	//	changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
	//	changeStatesMessage->mySamplerState = eSamplerState::eClamp;
	//
	//	SRenderToGUI* const guiChangeState = new SRenderToGUI(L"placementLine" + i, changeStatesMessage);
	//	RENDERER.AddRenderMessage(guiChangeState);
	//
	//	myPlacementLinesGUIElement[i]->mySprite->RenderToGUI(L"placementLine" + i);
	//}
}

void CGlobalHUD::LoadScoreboard()
{
	CU::CJsonValue scoreboard;
	scoreboard.Parse("Json/HUD/HUDGlobal.json");

	CU::CJsonValue jsonElementData = scoreboard.at("elementData");
	CU::CJsonValue jsonSprites = scoreboard.at("sprites");

	myScoreboardElement = LoadHUDElement(jsonElementData);

	const std::string scoreboardBGSpritePath = jsonSprites.at("background").GetString();
	const std::string portraitSpritePath = jsonSprites.at("characterPortrait").GetString();

	myScoreboardBGSprite = new CSpriteInstance(scoreboardBGSpritePath.c_str(), { 1.f,1.0f });
	myPortraitSprite = new CSpriteInstance(portraitSpritePath.c_str(), { 0.5f, 0.125f }, { 0.196f, -0.004f });

	myScoreboardElement.mySprite = myScoreboardBGSprite;

	myScoreboardElement.myShouldRender = false;
}

void CGlobalHUD::LoadMiniMap()
{
	//CU::CJsonValue jsonDoc;
	//if (myPlayers.Size() == 1)
	//{
	//	jsonDoc.Parse("Json/HUD/HUD1Player.json");
	//}
	//else if (myPlayers.Size() == 2)
	//{
	//	jsonDoc.Parse("Json/HUD/HUD2Player.json");
	//}
	//else if (myPlayers.Size() == 3)
	//{
	//	jsonDoc.Parse("Json/HUD/HUD3Player.json");
	//}
	//else if (myPlayers.Size() == 4)
	//{
	//	jsonDoc.Parse("Json/HUD/HUD4Player.json");
	//}
	//
	//CU::CJsonValue jsonPlacementLine = jsonDoc.at("placementLine");
	//for (unsigned int i = 0; i < myKartObjects.Size(); i++)
	//{
	//	SHUDElement* hudElement = new SHUDElement();
	//
	//	hudElement->myGUIElement.myOrigin = { 0.f,0.f }; // { 0.5f, 0.5f };
	//	hudElement->myGUIElement.myAnchor[(char)eAnchors::eTop] = true;
	//	hudElement->myGUIElement.myAnchor[(char)eAnchors::eLeft] = true;
	//
	//	hudElement->myGUIElement.myScreenRect = CU::Vector4f(jsonPlacementLine.at("position").GetVector2f());
	//
	//	const CU::CJsonValue sizeObject = jsonPlacementLine.at("size");
	//	hudElement->myPixelSize.x = sizeObject.at("pixelWidth").GetUInt();
	//	hudElement->myPixelSize.y = sizeObject.at("pixelHeight").GetUInt();
	//
	//	float rectWidth = sizeObject.at("screenSpaceWidth").GetFloat();
	//	float rectHeight = sizeObject.at("screenSpaceHeight").GetFloat();
	//	myPlacementLineScreenSpaceWidth = rectWidth;
	//
	//	float topLeftX = hudElement->myGUIElement.myScreenRect.x;
	//	float topLeftY = hudElement->myGUIElement.myScreenRect.y;
	//
	//	hudElement->myGUIElement.myScreenRect.z = rectWidth + topLeftX;
	//	hudElement->myGUIElement.myScreenRect.w = rectHeight + topLeftY;
	//	hudElement->mySprite = new CSpriteInstance("Sprites/GUI/Scoreboard/characterPortraitYoshi.dds", { 1.0f, 1.0f });
	//	myPlacementLinesGUIElement.Add(hudElement);
	//}
}

void CGlobalHUD::PresentScoreboard()
{
	DisableRedundantGUI();
	myScoreboardElement.myShouldRender = true;
}

void CGlobalHUD::DisableRedundantGUI()
{
	auto lambda = [this]()
	{
		CU::TimerManager timerManager;
		TimerHandle timer = timerManager.CreateTimer();

		while (timerManager.GetTimer(timer).GetLifeTime().GetSeconds() < .05f)
		{
			timerManager.UpdateTimers();
			//myLapCounterElement.mySprite->SetAlpha(0);
			
		}

		//myLapCounterElement.myShouldRender = false;
	};

	CU::Work work(lambda);
	work.SetName("disableRedundantGUI");
	CU::ThreadPool::GetInstance()->AddWork(work);
}

eMessageReturn CGlobalHUD::DoEvent(const CRaceOverMessage & aMessage)
{
	myWinners = aMessage.GetWinners();
	PresentScoreboard();
	return eMessageReturn::eContinue;
}

// debugging
eMessageReturn CGlobalHUD::DoEvent(const KeyCharPressed & aMessage)
{
	if (aMessage.GetKey() == 'p')
		PresentScoreboard();

	return eMessageReturn::eContinue;
}
