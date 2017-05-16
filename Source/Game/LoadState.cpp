#include "stdafx.h"
#include "LoadState.h"
#include "PlayState.h"
#include "Engine.h"
#include "ThreadPool.h"
#include "SpriteInstance.h"
#include "StateStack/StateStack.h"

#include "Renderer.h"
#include "BackgroundLoadingManager.h"

#include "ThreadedPostmaster/Postmaster.h"
#include "ThreadedPostmaster/PostOffice.h"
#include "../Audio/AudioInterface.h"

//#include "LoadingAnimation.h"

CLoadState::CLoadState(StateStack& aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::eLoadState)
	, myLevelIndex(aLevelIndex)
{
	myPlayState = nullptr;
}

CLoadState::~CLoadState()
{
}

void CLoadState::Init()
{
	CBackgroundLoadingManager &bLM = CBackgroundLoadingManager::GetInstance();

	myControlSchemeSprite = new CSpriteInstance("Sprites/Menu/Main/Loading/default.dds");
	myControlSchemeSprite->SetPivot({ 0.5f, 0.5f });
	myControlSchemeSprite->SetPosition({ 0.5f, 0.5f });

	myTDLogoSprite = new CSpriteInstance("Sprites/LoadingScreen/Rotating.dds");
	myTDLogoSprite->SetPivot({ 0.5f, -0.5f });
	myTDLogoSprite->SetPosition({ 0.96f, 0.88f });

	if (bLM.GetIfActive() == true)
	{
		if (bLM.GetLevelIndex() == myLevelIndex)
		{
			myPlayState = bLM.GetPlaystate();
		}
		else
		{
			myPlayState = new CPlayState(myStateStack, myLevelIndex);
			bLM.LoadAnotherState(myPlayState);
		}
	}
	else
	{
		bLM.CreateStateToLoad(myStateStack, myLevelIndex);
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
}

void CLoadState::OnExit(const bool /*aLetThroughRender*/)
{
}
