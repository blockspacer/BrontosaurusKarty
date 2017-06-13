#include "stdafx.h"
#include "LoadState.h"
#include "PlayState.h"
#include "Engine.h"
#include "ThreadPool.h"
#include "SpriteInstance.h"
#include "StateStack.h"

#include "Renderer.h"
#include "BackgroundLoadingManager.h"

#include "ThreadedPostmaster/Postmaster.h"
#include "ThreadedPostmaster/PostOffice.h"
#include "../Audio/AudioInterface.h"
//#include "LoadingAnimation.h"

CLoadState::CLoadState(StateStack & aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::eLoadState)
	, myLevelIndex(aLevelIndex)
{
	myPlayState = nullptr;
	myPlayers.Init(1);
}

CLoadState::CLoadState(StateStack& aStateStack, const int aLevelIndex, const CU::GrowingArray<SParticipant>& aPlayers)
	: State(aStateStack, eInputMessengerType::eLoadState)
	, myLevelIndex(aLevelIndex)
{
	myPlayState = nullptr;
	myPlayers.Init(aPlayers.Size());
	for (unsigned int i = 0; i< aPlayers.Size(); ++i)
	{
		myPlayers.Add(aPlayers[i]);
	}
}

CLoadState::~CLoadState()
{
	SAFE_DELETE(myControlSchemeSprite);
	SAFE_DELETE(myTDLogoSprite);
}

void CLoadState::Init()
{
	CBackgroundLoadingManager &bLM = CBackgroundLoadingManager::GetInstance();

	myControlSchemeSprite = new CSpriteInstance("Sprites/Loading/background.dds");
	myControlSchemeSprite->SetPivot({ 0.5f, 0.5f });
	myControlSchemeSprite->SetPosition({ 0.5f, 0.5f });

	myTDLogoSprite = new CSpriteInstance("Sprites/Loading/birdy.dds");
	myTDLogoSprite->SetPivot({ 0.5f, -0.5f });
	myTDLogoSprite->SetPosition({ 0.76f, 0.68f });

	if (bLM.GetIfActive() == true)
	{
		if (bLM.GetLevelIndex() == myLevelIndex)
		{
			myPlayState = bLM.GetPlaystate();
		}
		else
		{
			myPlayState = new CPlayState(myStateStack, myLevelIndex,myPlayers);
			bLM.LoadAnotherState(myPlayState);
		}
	}
	else
	{
		bLM.CreateStateToLoad(myStateStack, myLevelIndex,myPlayers);
		myPlayState = bLM.GetPlaystate();
	}
}

eStateStatus CLoadState::Update(const CU::Time& aDeltaTime)
{
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
	//myLoadingAnimation.Update(aDeltaTime);
	myTDLogoSprite->Rotate(5.f * aDeltaTime.GetSeconds());

	if (myPlayState->IsLoaded() == true)
	{
		myStateStack.SwapState(myPlayState);
		CBackgroundLoadingManager::GetInstance().Clear();
	}

	return eStateStatus::eKeep;
}

void CLoadState::Render()
{
	SChangeStatesMessage msg;

	msg.myBlendState = eBlendState::eAlphaBlend;
	msg.myDepthStencilState = eDepthStencilState::eDisableDepth;
	msg.myRasterizerState = eRasterizerState::eDefault;
	msg.mySamplerState = eSamplerState::eClamp;


	RENDERER.AddRenderMessage(new SChangeStatesMessage(msg));
	//myLoadingAnimation.Render();
	myControlSchemeSprite->Render();
	myTDLogoSprite->Render();
}

void CLoadState::OnEnter(const bool /*aLetThroughRender*/)
{
	Audio::CAudioInterface::GetInstance()->PostEvent("StopAll");
}

void CLoadState::OnExit(const bool /*aLetThroughRender*/)
{
}
