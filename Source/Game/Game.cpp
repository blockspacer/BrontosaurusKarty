#include "stdafx.h"
#include "Game.h" 

#include "LuaWrapper/SSlua/SSlua.h"
#include "ScriptLoader.h"

#include "KevinLoader/KevinLoader.h"
#include "BackgroundLoadingManager.h"
#include "ThreadedPostmaster/Postmaster.h"
#include "ThreadedPostmaster/PostOffice.h"

#include "LoadState.h"
#include "../Game/PollingStation.h"

CGame::CGame()
{
}

CGame::~CGame()
{
	KLoader::CKevinLoader::DestroyInstance();
	CBackgroundLoadingManager::DestroyInstance();
	SSlua::LuaWrapper::DestroyIfCreated();
}


void CGame::Init()
{
	CBackgroundLoadingManager::CreateInstance();
	KLoader::CKevinLoader::CreateInstance();
	SSlua::LuaWrapper::GetInstance().RegisterFunctions(&ScriptLoader::RegisterLuaFunctions);
	CPollingStation::Create();
	myGameEventMessenger.Init({ 0.5f, 0.1f });
	myClient.StartClient();
	myClient.Connect("127.0.0.1", "temp.cccp");
	myStateStack.PushState(new CLoadState(myStateStack, 0));
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