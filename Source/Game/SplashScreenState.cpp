#include "stdafx.h"
#include "SplashScreenState.h"

#include "../BrontosaurusEngine/SpriteInstance.h"
#include "CommonUtilities/InputMessage.h"
#include "CommonUtilities/EInputReturn.h"

const float StayTime = 1.5f;

CSplashScreenState::CSplashScreenState(StateStack& aStateStack)
	: State(aStateStack, eInputMessengerType::eSplashScreen, 1)
	, myTimer(0.f)
	, myCurrentSprite(0)
{
}

CSplashScreenState::~CSplashScreenState()
{
	mySprites.DeleteAll();
}

void CSplashScreenState::Init()
{
	const char* paths[SPRITE_COUNT] = { "Sprites/SplashScreen/tga.dds", "Sprites/SplashScreen/timedust.dds" };
	for (int i = 0; i < SPRITE_COUNT; ++i)
	{
		mySprites.Add(new CSpriteInstance(paths[i]));
		mySprites.GetLast()->SetPosition({ 0.5f, 0.5f });
		mySprites.GetLast()->SetPivot({ 0.5f, 0.5f });
	}
}

eStateStatus CSplashScreenState::Update(const CU::Time& aDeltaTime)
{
	myTimer += aDeltaTime.GetSeconds();

	if (myCurrentSprite >= SPRITE_COUNT)
	{
		myStatus = eStateStatus::ePop;
	}
	else
	{
		ChooseSprite();
	}

	return myStatus;
}

void CSplashScreenState::Render()
{
	if (myCurrentSprite < mySprites.Size())
	{
		mySprites[myCurrentSprite]->Render();
	}
}

void CSplashScreenState::OnEnter(const bool /*aLetThroughRender*/)
{
}

void CSplashScreenState::OnExit(const bool /*aLetThroughRender*/)
{
}

CU::eInputReturn CSplashScreenState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eMousePressed ||
		aInputMessage.myType == CU::eInputType::eGamePadButtonPressed ||
		aInputMessage.myType == CU::eInputType::eKeyboardPressed)
	{
		myCurrentSprite++;
		myTimer = 0.f;
	}

	return CU::eInputReturn::eKeepSecret;
}

void CSplashScreenState::ChooseSprite()
{
	if (myTimer <= StayTime)
	{
		mySprites[myCurrentSprite]->SetAlpha(myTimer / StayTime);
	}
	else
	{
		mySprites[myCurrentSprite]->SetAlpha(1.f - (myTimer - StayTime) / StayTime);
	}

	if (myTimer > StayTime * 2.f)
	{
		myCurrentSprite++;
		myTimer = 0.f;
	}
}
