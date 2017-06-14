#include "stdafx.h"
#include "Game.h" 

#include "LuaWrapper/SSlua/SSlua.h"
#include "ScriptLoader.h"

#include "KevinLoader/KevinLoader.h"
#include "BackgroundLoadingManager.h"
#include "ThreadedPostmaster/Postmaster.h"
#include "ThreadedPostmaster/PostOffice.h"

#include "MenuState.h"
#include "SplashScreenState.h"

#include "LoadState.h"
#include "../Game/PollingStation.h"
#include "AnimationEventFactory.h"

CGame::CGame()
{
}

CGame::~CGame()
{
	KLoader::CKevinLoader::DestroyInstance();
	CBackgroundLoadingManager::DestroyInstance();
	SSlua::LuaWrapper::DestroyIfCreated();
	delete CAnimationEventFactory::GetInstance();
}


void CGame::Init()
{
	CBackgroundLoadingManager::CreateInstance();
	KLoader::CKevinLoader::CreateInstance();
	SSlua::LuaWrapper::GetInstance().RegisterFunctions(&ScriptLoader::RegisterLuaFunctions);
	
	myGameEventMessenger.Init({ 0.5f, 0.1f });
	myStateStack.PushState(new CMenuState(myStateStack,"Json/Menu/MainMenu.json"));
	myStateStack.PushState(new CSplashScreenState(myStateStack));
}

bool CGame::Update(const CU::Time& aDeltaTime)
{
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
	myGameEventMessenger.Update(aDeltaTime.GetSeconds());
	if (myStateStack.Update(aDeltaTime) == false)
	{
		return false;
	}

	return true;
}

void CGame::Render()
{
	myStateStack.Render();
	myGameEventMessenger.Render();
}