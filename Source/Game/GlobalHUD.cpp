#include "stdafx.h"
#include "GlobalHUD.h"

#include "Engine.h"
#include "Renderer.h"
#include "RenderMessages.h"
#include "SpriteInstance.h"
#include "TextInstance.h"
#include "JsonValue.h"

#include "SParticipant.h"
#include "PollingStation.h"
#include "..\ThreadedPostmaster\RaceOverMessage.h"
#include "..\ThreadedPostmaster\KeyCharPressed.h"

#include <ThreadPool.h>
#include "ThreadedPostmaster/PopCurrentState.h"
#include "ThreadedPostmaster/ControllerInputMessage.h"


#define DEFAULT	{0.3f, 0.3f, 0.3f, 1.0f}
#define YELLOW	{1.0f, 1.0f, 0.0f, 1.0f}
#define GREEN	{0.0f, 1.0f, 0.0f, 1.0f}
#define PINK	{1.0f, 0.0f, 1.0f, 1.0f}
#define BLUE	{0.0f, 0.0f, 1.0f, 1.0f}

CGlobalHUD::CGlobalHUD()
{
	myKartObjects = CPollingStation::GetInstance()->GetKartList();
}

CGlobalHUD::~CGlobalHUD()
{
}

void CGlobalHUD::LoadHUD()
{
	CU::CJsonValue jsonDoc;
	jsonDoc.Parse("Json/HUD/HUDGlobal.json");

	LoadMiniMap(jsonDoc.at("minimap"));
	LoadScoreboard(jsonDoc.at("scoreboard"));
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
				switch (myWinners[i].character)
				{
				case SParticipant::eCharacter::eVanBrat:
					myPortraitSprite->SetRect({ 0.f, 0.875f, 1.f, 1.f });
					break;
				case SParticipant::eCharacter::eGrandMa:
					myPortraitSprite->SetRect({ 0.f, 0.750f, 1.f, 0.875f });
					break;
				default:
					myPortraitSprite->SetRect({ 0.f, 0.750f, 1.f, 0.875f });
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

	if (myMinimapElement.myShouldRender == true)
	{
		SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"minimap", myMinimapElement.myGUIElement, myMinimapElement.myPixelSize);
		RENDERER.AddRenderMessage(guiElement);
	
		SetGUIToAlphaBlend(L"minimap");
		{
			SParticipant::eInputDevice whoTheFuck = SParticipant::eInputDevice::eController1;
	
			myMinimapElement.mySprite->RenderToGUI(L"minimap");


			for (int i = 0; i < myKartObjects->Size(); ++i)
			{
				SComponentQuestionData percentDoneQuestion;

				if (myKartObjects->At(i)->AskComponents(eComponentQuestionType::eGetLapTraversedPercentage, percentDoneQuestion) == true)
				{
					float distancePercent = percentDoneQuestion.myFloat;
					myMinimapPosIndicator->SetPosition({ myMinimapElement.mySprite->GetPosition().x + distancePercent, 0.5f });
				}

				myMinimapPosIndicator->SetColor(DEFAULT);
				switch (i)
				{
				case (int)SParticipant::eInputDevice::eController1:
					if (myKartObjects->At(i)->AskComponents(eComponentQuestionType::eHasCameraComponent, SComponentQuestionData()) == true)
					{
						myMinimapPosIndicator->SetColor(YELLOW);
					}
					break;
				case (int)SParticipant::eInputDevice::eController2:
					if (myKartObjects->At(i)->AskComponents(eComponentQuestionType::eHasCameraComponent, SComponentQuestionData()) == true)
					{
						myMinimapPosIndicator->SetColor(GREEN);
					}
					break;
				case (int)SParticipant::eInputDevice::eController3:
					if (myKartObjects->At(i)->AskComponents(eComponentQuestionType::eHasCameraComponent, SComponentQuestionData()) == true)
					{
						myMinimapPosIndicator->SetColor(PINK);
					}
					break;
				case (int)SParticipant::eInputDevice::eController4:
					if (myKartObjects->At(i)->AskComponents(eComponentQuestionType::eHasCameraComponent, SComponentQuestionData()) == true)
					{
						myMinimapPosIndicator->SetColor(BLUE);
					}
					break;
				default:
					// Also make mark a bit smaller.
					break;
				}

				myMinimapPosIndicator->RenderToGUI(L"minimap");

			}
		}	
		SetGUIToEndBlend(L"minimap");
	}
}

void CGlobalHUD::LoadScoreboard(const CU::CJsonValue& aJsonValue)
{
	CU::CJsonValue jsonElementData = aJsonValue.at("elementData");
	CU::CJsonValue jsonSprites = aJsonValue.at("sprites");

	myScoreboardElement = LoadHUDElement(jsonElementData);

	const std::string scoreboardBGSpritePath = jsonSprites.at("background").GetString();
	const std::string portraitSpritePath = jsonSprites.at("characterPortrait").GetString();

	myScoreboardBGSprite = new CSpriteInstance(scoreboardBGSpritePath.c_str(), { 1.f,1.0f });
	myPortraitSprite = new CSpriteInstance(portraitSpritePath.c_str(), { 0.5f, 0.125f }, { 0.196f, -0.004f });

	myScoreboardElement.mySprite = myScoreboardBGSprite;
	myScoreboardElement.myShouldRender = false;
}

void CGlobalHUD::LoadMiniMap(const CU::CJsonValue& aJsonValue)
{
	CU::CJsonValue jsonElementData = aJsonValue.at("elementData");
	CU::CJsonValue jsonSprites = aJsonValue.at("sprites");

	myMinimapElement = LoadHUDElement(jsonElementData);

	const std::string backgroundSpritePath = jsonSprites.at("background").GetString();
	const std::string posIndicatorSpritePath = jsonSprites.at("mark").GetString();

	myMinimapElement.mySprite = new CSpriteInstance(backgroundSpritePath.c_str(), { 1.0f, 1.0f });
	myMinimapPosIndicator = new CSpriteInstance(posIndicatorSpritePath.c_str(), { 0.016f, 0.16f });

	// IF SINGLE PLAYER -> POS AT THE BOTTOM
}

void CGlobalHUD::PresentScoreboard()
{
	myRaceOver = true;
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

void CGlobalHUD::ToMainMenu()
{
	POSTMASTER.Broadcast(new PopCurrentState());
}

// debugging
eMessageReturn CGlobalHUD::DoEvent(const KeyCharPressed & aMessage)
{
	if (aMessage.GetKey() == 'p')
		PresentScoreboard();
	if (aMessage.GetKey() == 'c' && myRaceOver == true)
		ToMainMenu();

	return eMessageReturn::eContinue;
}

void CGlobalHUD::Retry()
{
}

eMessageReturn CGlobalHUD::DoEvent(const Postmaster::Message::CControllerInputMessage& aControllerInputMessage)
{
	const Postmaster::Message::InputEventData& data = aControllerInputMessage.GetData();

	if(myRaceOver == true && data.eventType == Postmaster::Message::EventType::ButtonChanged &&
		data.data.boolValue == true && data.buttonIndex == Postmaster::Message::ButtonIndex::A)
	{
		ToMainMenu();
	}
	if (myRaceOver == true && data.eventType == Postmaster::Message::EventType::ButtonChanged &&
		data.data.boolValue == true && data.buttonIndex == Postmaster::Message::ButtonIndex::A)
	{
		Retry();
	}

	return eMessageReturn::eContinue;
}
