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
}

void CGlobalHUD::LoadScoreboard()
{
	CU::CJsonValue scoreboard;
	scoreboard.Parse("Json/HUD/HUDScoreboard.json");

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
