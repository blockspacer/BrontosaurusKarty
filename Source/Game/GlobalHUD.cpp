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
#include "ThreadedPostmaster/PushState.h"
#include "ThreadedPostmaster/PostOffice.h"
#include "ThreadedPostmaster/RaceStartedMessage.h"
#include "ThreadedPostmaster/PlayerPassedGoalMessage.h"

#include "..\Components\KartControllerComponentManager.h"
#include "..\Components\TimeTrackerComponentManager.h"

#define DEFAULT	{0.3f, 0.3f, 0.3f, 1.0f}
#define YELLOW	{1.0f, 1.0f, 0.0f, 1.0f}
#define GREEN	{0.0f, 1.0f, 0.0f, 1.0f}
#define PINK	{1.0f, 0.0f, 1.0f, 1.0f}
#define BLUE	{0.0f, 0.0f, 1.0f, 1.0f}


CGlobalHUD::CGlobalHUD(int aLevelIndex):myNrOfPlayers(0), myPortraitSprite(nullptr), myMinimapPosIndicator(nullptr), myRaceOver(false), myTimeText(nullptr), myLevelIndex(aLevelIndex)
{
	myKartObjects = CPollingStation::GetInstance()->GetKartList();
}

CGlobalHUD::~CGlobalHUD()
{
	POSTMASTER.Unsubscribe(this);

	SAFE_DELETE(myPortraitSprite);
	SAFE_DELETE(myMinimapPosIndicator);
	//SAFE_DELETE(myCountdownSprite);
	SAFE_DELETE(myTimeText);
}

void CGlobalHUD::LoadHUD()
{
	(unsigned char)myNrOfPlayers = CPollingStation::GetInstance()->GetAmmountOfPlayer();
	CU::CJsonValue jsonDoc;
	jsonDoc.Parse("Json/HUD/HUDGlobal.json");

	LoadMiniMap(jsonDoc.at("minimap"));
	LoadScoreboard(jsonDoc.at("scoreboard"));
	LoadCountDown(jsonDoc.at("countdown"));
}

void CGlobalHUD::Update(const float aDeltaTime)
{
	float cappedDeltaTime = aDeltaTime;
	if(cappedDeltaTime > 1.0f / 30.0f)
	{
		cappedDeltaTime = 1.0f / 30.0f;
	}

	for (int i = 0; i < myKartObjects->Size(); ++i)
	{
		SComponentQuestionData percentDoneQuestion;

		if (myKartObjects->At(i)->AskComponents(eComponentQuestionType::eGetLapTraversedPercentage, percentDoneQuestion) == true)
		{
			float distancePercent = percentDoneQuestion.myFloat;
			float xPos = ((myMinimapElement.mySprite->GetPosition().x + distancePercent) * 0.87f) + 0.055f;
			CLAMP(xPos, 0.1f, 0.8f);
			myMinimapXPositions[i] = myMinimapXPositions[i] + 1 * cappedDeltaTime * (xPos - myMinimapXPositions[i]);

		}
	}
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
				myTimeText->SetPosition(newPortraitPos + myTimeTextOffset);
				int timePassedSum = myWinners[i].minutesPassed + myWinners[i].secondsPassed + myWinners[i].hundredthsSecondsPassed;
				if(timePassedSum > 0)
				{
					std::wstring minutesPassedString = L"";
					int minutesPassed = myWinners[i].minutesPassed;

					if(minutesPassed < 10)
					{
						minutesPassedString += L"0";
					}
					minutesPassedString += std::to_wstring(minutesPassed);

					std::wstring secondsPassedString = L"";
					int secondsPassed = myWinners[i].secondsPassed;

					if (secondsPassed < 10)
					{
						secondsPassedString += L"0";
					}
					secondsPassedString += std::to_wstring(secondsPassed);

					std::wstring hundredthsSecondsPassedString = L"";
					int hundredthsSecondsPassed = myWinners[i].hundredthsSecondsPassed;

					if (hundredthsSecondsPassed < 100)
					{
						hundredthsSecondsPassedString += L"0";
					}
					if (hundredthsSecondsPassed < 10)
					{
						hundredthsSecondsPassedString += L"0";
					}
					hundredthsSecondsPassedString += std::to_wstring(hundredthsSecondsPassed);

					myTimeText->SetText(minutesPassedString + L":" + secondsPassedString + L":" + hundredthsSecondsPassedString);
				}
				else
				{
					myTimeText->SetText(L"--:--:---");
				}
				myTimeText->RenderToGUI(L"scoreboard");
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
			myMinimapElement.mySprite->RenderToGUI(L"minimap");


			for (int i = 0; i < myKartObjects->Size(); ++i)
			{
				myMinimapPosIndicator->SetPosition({ myMinimapXPositions[i], 0.43f });

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

	if (myCountdownElement.myShouldRender == true)
	{
		SCreateOrClearGuiElement* guiElement = new SCreateOrClearGuiElement(L"countdown", myCountdownElement.myGUIElement, myCountdownElement.myPixelSize);
		RENDERER.AddRenderMessage(guiElement);

		SetGUIToAlphaBlend(L"countdown");

		myCountdownElement.mySprite->RenderToGUI(L"countdown");

		SetGUIToEndBlend(L"countdown");

	}
}

void CGlobalHUD::StartCountDown()
{

	auto countdownLambda = [this]() {

		CU::TimerManager timerManager;
		TimerHandle timer = timerManager.CreateTimer();
		unsigned char startCountdownTime = 0;
		float floatTime = 0.f;


		while (floatTime <= 4.5f)
		{
			timerManager.UpdateTimers();
			floatTime = timerManager.GetTimer(timer).GetLifeTime().GetSeconds();

			if ((unsigned char)floatTime > startCountdownTime)
			{
				startCountdownTime = std::floor(floatTime);
				if (startCountdownTime == 0)		myCountdownSprite->SetAlpha(0);
				else if (startCountdownTime == 1) { myCountdownSprite->SetRect({ 0.f,0.75f,1.f,1.00f }); myCountdownSprite->SetAlpha(1); }
				else if (startCountdownTime == 2) 	myCountdownSprite->SetRect({ 0.f,0.50f,1.f,0.75f });
				else if (startCountdownTime == 3) 	myCountdownSprite->SetRect({ 0.f,0.25f,1.f,0.50f });
				else if (startCountdownTime == 4)
				{
					myCountdownSprite->SetRect({ 0.f,0.00f,1.f,0.25f });
					POSTMASTER.Broadcast(new CRaceStartedMessage());
					POSTMASTER.GetThreadOffice().HandleMessages();

					for (unsigned int i = 0; i < myMinimapXPositions.Size(); i++)
					{
						myMinimapXPositions[i] = 0.0f;
					}
				}
			}
		}
	};

	CU::Work work(countdownLambda);
	work.SetName("Countdown thread");

	std::function<void(void)> callback = [this]()
	{
		CU::TimerManager timerManager;
		TimerHandle timer = timerManager.CreateTimer();

		while (timerManager.GetTimer(timer).GetLifeTime().GetSeconds() < .05f)
		{
			timerManager.UpdateTimers(); // f�r att komma runt att trippelbuffringen sl�nger bort meddelanden.
			myCountdownSprite->SetAlpha(0);
		}

		myCountdownElement.myShouldRender = false;
		myMinimapElement.myShouldRender = true;
	};

	work.SetFinishedCallback(callback);
	CU::ThreadPool::GetInstance()->AddWork(work);
}

void CGlobalHUD::LoadCountDown(const CU::CJsonValue & aJsonValue)
{
	CU::CJsonValue jsonElementData = aJsonValue.at("elementData");
	myCountdownElement = LoadHUDElement(jsonElementData);

	const std::string countdownSprite = aJsonValue.at("sprite").GetString();

	myCountdownSprite = new CSpriteInstance(countdownSprite.c_str(), { 1.f, 1.f });
	myCountdownElement.mySprite = myCountdownSprite;
	myCountdownSprite->SetRect({ 0.f,0.75f,1.f,1.00f });
}

void CGlobalHUD::LoadScoreboard(const CU::CJsonValue& aJsonValue)
{
	CU::CJsonValue jsonElementData = aJsonValue.at("elementData");
	CU::CJsonValue jsonSprites = aJsonValue.at("sprites");
	CU::CJsonValue jsonTimeText = aJsonValue.at("timeText");
	CU::CJsonValue jsonTimeTextPosition = jsonTimeText.at("position");
	CU::CJsonValue jsonTimeTextColor = jsonTimeText.at("color");

	myScoreboardElement = LoadHUDElement(jsonElementData);

	const std::string scoreboardBGSpritePath = jsonSprites.at("background").GetString();
	const std::string portraitSpritePath = jsonSprites.at("characterPortrait").GetString();

	myPortraitSprite = new CSpriteInstance(portraitSpritePath.c_str(), { 0.5f, 0.125f }, { 0.196f, -0.004f });

	myScoreboardElement.mySprite = new CSpriteInstance(scoreboardBGSpritePath.c_str(), { 1.0f, 1.0f });
	myScoreboardElement.myShouldRender = false;

	myTimeTextOffset.x = jsonTimeTextPosition.at("x").GetFloat();
	myTimeTextOffset.y = jsonTimeTextPosition.at("y").GetFloat();

	CU::Vector4f color(jsonTimeTextColor.at("r").GetFloat(), jsonTimeTextColor.at("g").GetFloat(), jsonTimeTextColor.at("b").GetFloat(), jsonTimeTextColor.at("a").GetFloat());

	myTimeText = new CTextInstance();
	myTimeText->Init("FinishTime");
	myTimeText->SetAlignment(eAlignment::eLeft);
	myTimeText->SetColor(color);
	myTimeText->SetPosition(CU::Vector2f(myTimeTextOffset));
	myTimeText->SetText(L"");
}

void CGlobalHUD::LoadMiniMap(const CU::CJsonValue& aJsonValue)
{
	CU::CJsonValue jsonElementData;

	if (myNrOfPlayers == 1)
		jsonElementData = aJsonValue.at("elementDataSP");
	else
		jsonElementData = aJsonValue.at("elementDataMP");

	CU::CJsonValue jsonSprites = aJsonValue.at("sprites");

	myMinimapElement = LoadHUDElement(jsonElementData);

	const std::string backgroundSpritePath = jsonSprites.at("background").GetString();
	const std::string posIndicatorSpritePath = jsonSprites.at("mark").GetString();

	myMinimapElement.mySprite = new CSpriteInstance(backgroundSpritePath.c_str(), { 1.0f, 1.0f });
	myMinimapPosIndicator = new CSpriteInstance(posIndicatorSpritePath.c_str(), { 0.016f, 0.29f });

	myMinimapElement.myShouldRender = false;
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
	for (unsigned short i = 0; i <  aMessage.GetWinners().Size(); i++)
	{
		myWinners[i].character = aMessage.GetWinners()[i].character;
		myWinners[i].isPlayer = aMessage.GetWinners()[i].isPlayer;
		myWinners[i].placement = aMessage.GetWinners()[i].placement;
		myWinners[i].minutesPassed = aMessage.GetWinners()[i].minutesPassed;
		myWinners[i].secondsPassed = aMessage.GetWinners()[i].secondsPassed;
		myWinners[i].hundredthsSecondsPassed = aMessage.GetWinners()[i].hundredthsSecondsPassed;
	}

	PresentScoreboard();
	return eMessageReturn::eContinue;
}

void CGlobalHUD::ToMainMenu(const std::function<void(void)>& aCallback)
{
	PopCurrentState* message = new PopCurrentState();
	message->SetCallback(aCallback);
	POSTMASTER.BroadcastLocal(message);
}

// debugging
eMessageReturn CGlobalHUD::DoEvent(const KeyCharPressed & aMessage)
{
	if (aMessage.GetKey() == 'p')
		PresentScoreboard();
	if (aMessage.GetKey() == 'c' && myRaceOver == true)
		ToMainMenu([](){});

	return eMessageReturn::eContinue;
}

void CGlobalHUD::Retry()
{
	ToMainMenu([this]() {POSTMASTER.BroadcastLocal(new PushState(PushState::eState::ePlayState, myLevelIndex)); });
}

void CGlobalHUD::LoadNext()
{
	ToMainMenu([this](){POSTMASTER.BroadcastLocal(new PushState(PushState::eState::ePlayState, myLevelIndex + 1)); });
}

eMessageReturn CGlobalHUD::DoEvent(const CPlayerPassedGoalMessage& aMessage)
{
	for (int i = 0; i < myKartObjects->Size(); ++i)
	{
		if(myKartObjects->At(i)->GetId() == aMessage.GetGameObject()->GetId())
		{
			myMinimapXPositions[i] = 0.05f;
		}
	}

	return eMessageReturn::eContinue;
}